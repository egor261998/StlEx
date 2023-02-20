#include "../../stdafx.h"

using CIocp = stlex::io::iocp::CIocp;

/**
* 26481 - арифметика указателей.
* 26493 - приведения в стиле C.
*/
#pragma warning (push)
#pragma warning (disable: 26481 26493)
//------------------------------------------------------------------------------
CIocp::CIocp(
	const std::shared_ptr<logger::ILogger>& pLogger) noexcept(false) :
	CIocp(1, INFINITE, pLogger)
{
	
}
//------------------------------------------------------------------------------
CIocp::CIocp(
	const DWORD minThreadCount,
	const DWORD maxThreadCount,
	const std::shared_ptr<logger::ILogger>& pLogger) noexcept(false) :
	_pLogger(pLogger)
{
	/** лог в консоль если лог пустой */
	if (_pLogger == nullptr)
		_pLogger = std::make_shared<logger::CLoggerToConsole>();

	try
	{
		/** инициализируем порт завершения */
		_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		if (!_hIocp.isValid())
			throw std::runtime_error("CreateIoCompletionPort failed with error: " +
				std::to_string(GetLastError()));

		/** инициализируем пул потоков */
		_pThreadPool = std::make_unique<CThreadPool>(
			*this, minThreadCount, maxThreadCount);
	}
	catch (const std::exception& ex)
	{
		log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code CIocp::setLogger(
	const std::shared_ptr<logger::ILogger>& pLogger) noexcept
{
	if(pLogger == nullptr)
		return std::error_code(ERROR_INVALID_PARAMETER, std::system_category());

	const auto lock = lockGuard();

	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	_pLogger = pLogger;

	return {};
}
//------------------------------------------------------------------------------
std::shared_ptr<stlex::logger::ILogger> CIocp::getLogger() const noexcept
{
	const auto lock = lockGuard();

	return _pLogger;
}
//------------------------------------------------------------------------------
std::error_code CIocp::bind(
	const HANDLE hHandle,
	const ULONG_PTR ulCompletionKey) noexcept
{
	/** проверка описателя */
	if (!handle::CHandle::isValid(hHandle))
		return std::error_code(ERROR_INVALID_HANDLE, std::system_category());

	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	/** привязка к механизму ввода/вывода */
	if(auto hIocpRet = CreateIoCompletionPort(
		hHandle, _hIocp, ulCompletionKey, 0); hIocpRet != _hIocp)
		return std::error_code(GetLastError(), std::system_category());

	return {};
}
//------------------------------------------------------------------------------
CIocp::CAsyncOperation& CIocp::getAsyncOperation(
	const PVOID pCompletionRoutineContext,
	const FAsyncCompletion fCompletionRoutine,
	CCounter* pCounter)
{
	try
	{
		misc::CCounterScoped counter(*this);
		if (!counter.isStartOperation())
		{
			throw std::logic_error("!counter.isStartOperation()");
		}

		auto pAsyncOperationResult = _listFreeAsyncOperation.popEx();
		if (pAsyncOperationResult == nullptr)
		{
			/** создаем новую асинхронную операцию */
			pAsyncOperationResult =
				std::make_unique<CAsyncOperation>(
					this, pCompletionRoutineContext, fCompletionRoutine);
		}
		else
		{
			/** проверка контекста асинхронной операции */
			if (pAsyncOperationResult->_pIocp != this)
			{
				/** повреждена асинхронная операция */
				throw std::logic_error("pAsyncOperationResult->pIocp != this");
			}

			/** есть свободная асинхронная операция */
			pAsyncOperationResult->_pCompletionRoutineContext = pCompletionRoutineContext;
			pAsyncOperationResult->_fCompletionRoutine = fCompletionRoutine;
		}

		/** если нить пула, то вернется контекст нити */
		std::error_code ec;
		pAsyncOperationResult->_pThreadPoolWorker = _tls.getValue(ec);
		
		if (ec)
		{
			throw std::runtime_error("getValue failed with error: " +
				std::to_string(ec.value()));
		}

		if (pAsyncOperationResult->_pThreadPoolWorker != nullptr)
		{
			if (!pAsyncOperationResult->_pThreadPoolWorker->startOperation())
			{
				throw std::logic_error("!pAsyncOperationResult->_pThreadPoolWorker->startOperation()");
			}
		}

		/** заполняем счетчик */
		pAsyncOperationResult->updateCounter(pCounter);

		RtlZeroMemory(&pAsyncOperationResult->_overlapped, sizeof(OVERLAPPED));
		counter.release();
		return *pAsyncOperationResult.release();
	}
	catch (const std::exception& ex)
	{
		log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code CIocp::transit(
	const PVOID pCompletionRoutineContext,
	const FAsyncCompletion fCompletionRoutine,
	const ULONG_PTR ulCompletionKey,
	CCounter* pCounter)
{
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		auto& asyncOperation = getAsyncOperation(
			pCompletionRoutineContext, fCompletionRoutine, pCounter);

		if (pCounter && !asyncOperation.isStartOperation())
		{
			asyncOperation.cancel();
			return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
		}

		/** посылаем транзитный запрос */
		if (!PostQueuedCompletionStatus(
			_hIocp,
			0,
			ulCompletionKey,
			&asyncOperation._overlapped))
		{
			/** проблема транзита */
			asyncOperation.cancel();
			return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
		}

		return {};
	}
	catch (const std::exception& ex)
	{
		log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
void CIocp::release(
	const bool bIsWait) noexcept
{
	/** завершаем работу */
	__super::release(false);

	/** ждем завершения всех нитей */
	_pThreadPool->release(true);

	/** закрываем порт ввода/вывода */
	_hIocp.close();

	/** ожидаем */
	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//------------------------------------------------------------------------------
CIocp::~CIocp()
{
	/** завершаем работу */
	release(true);
}
//------------------------------------------------------------------------------
void CIocp::workerThread(
	CThreadPoolWorker& workerThread) noexcept
{
	/** заносим нить пула в TLS */
	if (const auto ec = _tls.setValue(&workerThread); ec)
	{
		log(logger::ILogger::EMessageType::warning,
			L"tls.setValue",
			ec);

		/** увольняемся */
		workerThread._threadPool.delWorker(workerThread);
		assert(false);
		return;
	}

	while (true)
	{
		/** код ошибки асинхронной операции */
		std::error_code ec;

		/** объект асинхронной операции */
		LPOVERLAPPED pOverlapped = nullptr;

		/** количество задействованных байт */
		DWORD dwReturnedBytes = 0;

		/** ключ асинхронной операции */
		ULONG_PTR ulCompletionKey = 0;

		/** ожидания завершения асинхронной операции */
		if (!GetQueuedCompletionStatus(
			_hIocp,
			&dwReturnedBytes,
			&ulCompletionKey,
			&pOverlapped,
			_nWorkerTimeIde))
		{
			ec = std::error_code(GetLastError(), std::system_category());

			if (!pOverlapped)
			{
				if (ec.value() == WAIT_TIMEOUT)
				{

					/** нас долго не используют, может уволиться? */
					if (workerThread._threadPool.canFreeWorker(workerThread))
						break;

					/** будет сложно но мы справимся */
					continue;
				}

				/** свалил Iocp, разбегаемся */
				log(logger::ILogger::EMessageType::warning,
					L"IOCP Closed",
					ec);

				break;
			}

			/** работа была выполнена, но ошибка в ТЗ.
			об это необходимо сообщить тому кто выдал ТЗ */
		}

		/** особая проверка на закрытие Iocp */
		if (!pOverlapped &&
			ulCompletionKey == (ULONG_PTR)_pThreadPool.get())
			break;

		/** пора немного поработать
			может нанять еще одного работника? */
		workerThread._bIsBusy = true;

		try
		{
			workerThread._threadPool.canAddWorker();
		}
		catch (const std::exception& ex)
		{
			log(logger::ILogger::EMessageType::warning, ex);
		}

		if (pOverlapped)
		{
			/** получаем контекст асинхронной операции */
			auto& asyncOperation =
				*CONTAINING_RECORD(pOverlapped, CAsyncOperation, _overlapped);

			/** проверяем что контекст именно нашей компании,
			сравним его с работодателем */
			if (asyncOperation._pIocp != this)
			{
				/** наша компания такое не обслуживает */
				log(logger::ILogger::EMessageType::warning, L"pOperation->_pIocp != this");
			}
			else
			{
				/** заполняем отчет */
				asyncOperation._ec = ec;
				asyncOperation._dwReturnedBytes = dwReturnedBytes;
				asyncOperation._ulCompletionKey = ulCompletionKey;

				try
				{
					if (asyncOperation._fCompletionRoutine != nullptr)
						asyncOperation._fCompletionRoutine(asyncOperation);
				}
				catch (const std::exception& ex)
				{
					log(logger::ILogger::EMessageType::warning, ex);
				}

				/** завершаем работу с асинхронной операцией */
				asyncOperation.cancel();
			}
		}
		else
		{
			log(logger::ILogger::EMessageType::warning, L"pOverlapped == nullptr");
		}

		/** мы снова готовы приступить к работе */
		workerThread._bIsBusy = false;
			
		_pThreadPool->decrementBusyWorker();
	}

	/** увольняемся */
	workerThread._threadPool.delWorker(workerThread);
}
//------------------------------------------------------------------------------
#pragma warning (pop)