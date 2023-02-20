#include "../stdafx.h"

using CCounter = stlex::misc::CCounter;
using IAsyncIo = stlex::io::IAsyncIo;

/**
* 26493 - приведения в стиле C.
*/
#pragma warning (push)
#pragma warning (disable: 26493)
//------------------------------------------------------------------------------
IAsyncIo::IAsyncIo(
	const std::shared_ptr<iocp::CIocp>& pIocp):
	_pIocp(pIocp)
{
	if (_pIocp == nullptr)
	{
		_pIocp = std::make_shared<iocp::CIocp>();
	}
}
//------------------------------------------------------------------------------
bool IAsyncIo::isOpen() const noexcept
{
	return getState() == EIoState::open;
}
//------------------------------------------------------------------------------
void IAsyncIo::setHandle(
	const handle::CHandle& hHandle) noexcept
{
	const auto lock = lockGuard();

	if (_hAsyncHandle.getHandle() != hHandle.getHandle())
		_hAsyncHandle = hHandle;	
}
//------------------------------------------------------------------------------
std::error_code IAsyncIo::bindHandle(
	const handle::CHandle& hHandle) noexcept
{
	if (const auto ec = _pIocp->bind(hHandle); ec)
		return ec;

	{
		const auto lock = lockGuard();

		setHandle(hHandle);
		changeState(EIoState::open);
	}
	openHandler();

	return {};
}
//------------------------------------------------------------------------------
bool IAsyncIo::isBindHandle() const noexcept
{
	const auto lock = lockGuard();

	return _hAsyncHandle.isValid();
}
//------------------------------------------------------------------------------
void IAsyncIo::changeState(
	const EIoState eIoState) noexcept
{
	const auto lock = lockGuard();

	_eIoState = eIoState;
}
//------------------------------------------------------------------------------
IAsyncIo::EIoState IAsyncIo::getState() const noexcept
{
	const auto lock = lockGuard();

	return _eIoState;
}
//------------------------------------------------------------------------------
void IAsyncIo::changeHandle(
	const handle::CHandle& hHandle)
{
	try
	{
		closeHandle();
		_counterIoPending.waitOperation();
		bindHandle(hHandle);	
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
bool IAsyncIo::closeCheckPending() noexcept
{
	const auto lock = lockGuard();

	if (getState() == EIoState::closing && !_counterIoPending.checkOperation())
	{
		/** операций в обработке нет */
		changeState(EIoState::close);
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------
void IAsyncIo::closeHandle() noexcept
{
	{
		const auto lock = lockGuard();

		/** закрываем все предыдущее */
		if (getState() == EIoState::open)
		{
			/** изменяем состояние */
			changeState(EIoState::closing);
		}	

		if (_hAsyncHandle.isValid())
		{
			_hAsyncHandle.close();
		}
	}

	if (closeCheckPending())
	{
		/** обрабатываем закрытие */
		closeHandler();
	}
}
//------------------------------------------------------------------------------
std::error_code IAsyncIo::startAsyncRead(
	BYTE* bufferRead,
	const DWORD dwBufferSize,
	const UINT64 offset)
{
	/** операция на весь объект */
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	/** операция на ввод/вывод */
	misc::CCounterScoped counterIoPending(_counterIoPending);
	if (!counterIoPending.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		auto& asyncOperation =
			_pIocp->getAsyncOperation(this, asyncReadIocpHandler);

		asyncOperation._buffer._p = bufferRead;
		asyncOperation._buffer._dwSize = dwBufferSize;
		*(UINT64*)&asyncOperation._overlapped.Offset = offset;

		if (!ReadFile(getHandle(),
			asyncOperation._buffer._p,
			asyncOperation._buffer._dwSize,
			nullptr,
			&asyncOperation._overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				closeCheckPending(counterIoPending);
				asyncOperation.cancel();
				return std::error_code(dwResult, std::system_category());
			}
		}

		counterIoPending.release();
		counter.release();
		return {};
	}
	catch (const std::exception& ex)
	{
		closeCheckPending(counterIoPending);
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code IAsyncIo::startRead(
	BYTE* bufferRead,
	const DWORD dwBufferSize,
	DWORD& dwReturnedBytes,
	const UINT64 offset)
{
	/** операция на весь объект */
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	/** операция на ввод/вывод */
	misc::CCounterScoped counterIoPending(_counterIoPending);
	if (!counterIoPending.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::error_code ec;
		handle::CEvent hEvent;
		SSyncIoComplete ioSync(hEvent, dwReturnedBytes, ec);

		auto& asyncOperation =
			_pIocp->getAsyncOperation(&ioSync, asyncIocpHandler);

		asyncOperation._buffer._p = bufferRead;
		asyncOperation._buffer._dwSize = dwBufferSize;
		*(UINT64*)&asyncOperation._overlapped.Offset = offset;

		if (!ReadFile(getHandle(),
			asyncOperation._buffer._p,
			asyncOperation._buffer._dwSize,
			nullptr,
			&asyncOperation._overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				closeCheckPending(counterIoPending);
				asyncOperation.cancel();
				return std::error_code(dwResult, std::system_category());
			}
		}
	
		if (const auto ecWait = hEvent.waitSignal(); ecWait)
		{
			_pIocp->log(
				logger::ILogger::EMessageType::warning,
				L"Wait event failed",
				ecWait);
			ec = ecWait;
		}

		_nCountReadByte += dwReturnedBytes;
		closeCheckPending(counterIoPending);
		return ec;
	}
	catch (const std::exception& ex)
	{
		closeCheckPending(counterIoPending);
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code IAsyncIo::startAsyncWrite(
	const BYTE* bufferWrite,
	const DWORD dwBufferSize,
	const UINT64 offset)
{
	/** операция на весь объект */
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	/** операция на ввод/вывод */
	misc::CCounterScoped counterIoPending(_counterIoPending);
	if (!counterIoPending.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		auto& asyncOperation =
			_pIocp->getAsyncOperation(this, asyncWriteIocpHandler);

		asyncOperation._buffer._pConst = bufferWrite;
		asyncOperation._buffer._dwSize = dwBufferSize;
		*(UINT64*)&asyncOperation._overlapped.Offset = offset;

		if (!WriteFile(getHandle(),
			asyncOperation._buffer._pConst,
			asyncOperation._buffer._dwSize,
			nullptr,
			&asyncOperation._overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				closeCheckPending(counterIoPending);
				asyncOperation.cancel();
				return std::error_code(dwResult, std::system_category());
			}
		}

		counterIoPending.release();
		counter.release();
		return {};
	}
	catch (const std::exception& ex)
	{
		closeCheckPending(counterIoPending);
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code IAsyncIo::startWrite(
	const BYTE* bufferWrite,
	const DWORD dwBufferSize,
	DWORD& dwReturnedBytes,
	const UINT64 offset)
{
	/** операция на весь объект */
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	/** операция на ввод/вывод */
	misc::CCounterScoped counterIoPending(_counterIoPending);
	if (!counterIoPending.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::error_code ec;
		handle::CEvent hEvent;
		SSyncIoComplete ioSync(hEvent, dwReturnedBytes, ec);

		auto& asyncOperation =
			_pIocp->getAsyncOperation(&ioSync, asyncIocpHandler);

		asyncOperation._buffer._pConst = bufferWrite;
		asyncOperation._buffer._dwSize = dwBufferSize;
		*(UINT64*)&asyncOperation._overlapped.Offset = offset;

		if (!WriteFile(getHandle(),
			asyncOperation._buffer._pConst,
			asyncOperation._buffer._dwSize,
			nullptr,
			&asyncOperation._overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				closeCheckPending(counterIoPending);
				asyncOperation.cancel();
				return std::error_code(dwResult, std::system_category());
			}
		}

		if (const auto ecWait = hEvent.waitSignal(); ecWait)
		{
			_pIocp->log(
				logger::ILogger::EMessageType::warning,
				L"Wait event failed",
				ecWait);
			ec = ecWait;
		}
		
		_nCountWriteByte += dwReturnedBytes;
		closeCheckPending(counterIoPending);
		return ec;
	}
	catch (const std::exception& ex)
	{
		closeCheckPending(counterIoPending);
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code IAsyncIo::flush() const noexcept
{
	const auto bResult = FlushFileBuffers(getHandle());

	return std::error_code(	
		bResult ? ERROR_SUCCESS : GetLastError(), 
		std::system_category());
}
//------------------------------------------------------------------------------
void IAsyncIo::closeCheckPending(
	misc::CCounterScoped& counterIoPending) noexcept
{
	counterIoPending.release(true);
	if (closeCheckPending())
	{
		/** обрабатываем закрытие */
		closeHandler();
	}
}
//------------------------------------------------------------------------------
HANDLE IAsyncIo::getHandle() const noexcept
{
	const auto lock = lockGuard();

	return _hAsyncHandle;
}
//------------------------------------------------------------------------------
void IAsyncIo::asyncReadCompletionHandler(
	BYTE* bufferRead,
	const DWORD dwReturnedBytes,
	const std::error_code& ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferRead);
	UNREFERENCED_PARAMETER(dwReturnedBytes);
	UNREFERENCED_PARAMETER(ec);
}
//------------------------------------------------------------------------------
void IAsyncIo::asyncWriteCompletionHandler(
	const BYTE* bufferWrite,
	const DWORD dwReturnedBytes,
	const std::error_code& ec) noexcept
{
	UNREFERENCED_PARAMETER(bufferWrite);
	UNREFERENCED_PARAMETER(dwReturnedBytes);
	UNREFERENCED_PARAMETER(ec);
}
//------------------------------------------------------------------------------
void IAsyncIo::openHandler() noexcept
{
	
}
//------------------------------------------------------------------------------
void IAsyncIo::closeHandler() noexcept
{
	
}
//------------------------------------------------------------------------------
void IAsyncIo::release(
	const bool bIsWait) noexcept
{
	__super::release(false);

	closeHandle();

	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//------------------------------------------------------------------------------
void IAsyncIo::asyncReadIocpHandler(
	const iocp::CAsyncOperation& asyncOperation) noexcept
{
	/** получаем контекст */
	assert(asyncOperation._pCompletionRoutineContext != nullptr);
	auto& asyncIo = *static_cast<IAsyncIo*>(
		asyncOperation._pCompletionRoutineContext);

	asyncIo._nCountReadByte += asyncOperation._dwReturnedBytes;

	/** обработчик асинхронного чтения */
	asyncIo.asyncReadCompletionHandler(
		asyncOperation._buffer._p,
		asyncOperation._dwReturnedBytes,
		asyncOperation._ec);

	asyncIo._counterIoPending.endOperation();
	if (asyncIo.closeCheckPending())
	{
		/** обрабатываем закрытие */
		asyncIo.closeHandler();
	}

	asyncIo.endOperation();
}
//------------------------------------------------------------------------------
void IAsyncIo::asyncWriteIocpHandler(
	const iocp::CAsyncOperation& asyncOperation) noexcept
{
	/** получаем контекст клиента */
	assert(asyncOperation._pCompletionRoutineContext != nullptr);
	auto& asyncIo = *static_cast<IAsyncIo*>(
		asyncOperation._pCompletionRoutineContext);

	asyncIo._nCountWriteByte += asyncOperation._dwReturnedBytes;

	/** обработчик асинхронной записи */
	asyncIo.asyncWriteCompletionHandler(
		asyncOperation._buffer._pConst,
		asyncOperation._dwReturnedBytes,
		asyncOperation._ec);

	asyncIo._counterIoPending.endOperation();
	if (asyncIo.closeCheckPending())
	{
		/** обрабатываем закрытие */
		asyncIo.closeHandler();
	}

	asyncIo.endOperation();
}
//------------------------------------------------------------------------------
void IAsyncIo::asyncIocpHandler(
	const iocp::CAsyncOperation& asyncOperation) noexcept
{
	/** получаем контекст клиента */
	assert(asyncOperation._pCompletionRoutineContext != nullptr);
	const auto& ioSyncComplete = *static_cast<SSyncIoComplete*>(
		asyncOperation._pCompletionRoutineContext);

	/** возвращенный размер операции */
	ioSyncComplete._dwReturnedBytes = asyncOperation._dwReturnedBytes;
	
	/** ошибка операции */
	ioSyncComplete._ec = asyncOperation._ec;

	/** событие завершения */
	if (const auto ec = ioSyncComplete._ev.notify(); ec)
	{
		asyncOperation._pIocp->log(
			logger::ILogger::EMessageType::warning,
			L"Notify failed",
			ec);
	}
}
//------------------------------------------------------------------------------
IAsyncIo::~IAsyncIo()
{
	release(true);
}
//------------------------------------------------------------------------------
#pragma warning (pop)