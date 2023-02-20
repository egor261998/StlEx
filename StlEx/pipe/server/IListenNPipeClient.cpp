#include "../../stdafx.h"

using IListenNPipeClient = stlex::pipe::server::IListenPipe::IListenNPipeClient;

//------------------------------------------------------------------------------
IListenNPipeClient::IListenNPipeClient(
	IListenPipe& listenPipe) :
	IListenPipeClient(listenPipe)
{

}
//------------------------------------------------------------------------------
std::error_code IListenNPipeClient::connectToServer(
	const DWORD dwRepeatCount,
	const DWORD dwMilliseconds) noexcept
{
	UNREFERENCED_PARAMETER(dwRepeatCount);
	UNREFERENCED_PARAMETER(dwMilliseconds);

	/** эта функция только для клиента */
	return std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
}
//------------------------------------------------------------------------------
std::error_code IListenNPipeClient::waitClient()
{
	misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	{
		const auto lock = lockGuard();

		/** на случай если NPipe уже открыт */
		if (getState() != EIoState::close)
			return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());

		changeState(EIoState::openning);
	}

	try
	{
		/** создаем NPipe */
		handle::CHandle hHandle = CreateNamedPipe(
			getPath().c_str(),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_REJECT_REMOTE_CLIENTS,
			PIPE_UNLIMITED_INSTANCES,
			misc::EBufferSize::e64K,
			misc::EBufferSize::e64K,
			0,
			NULL);
		
		if (!hHandle.isValid())
		{
			changeState(EIoState::close);
			return std::error_code(GetLastError(), std::system_category());
		}

		/** устанавливаем описатель */
		setHandle(hHandle);
		if (const auto ec = _pIocp->bind(hHandle); ec)
		{
			changeState(EIoState::close);
			return ec;
		}

		/** пробуем подключить клиента */
		auto& asyncOperation = _pIocp->getAsyncOperation(
			this, connectPipeHandler);
		if (!ConnectNamedPipe(hHandle, &asyncOperation._overlapped))
		{
			const auto dwResult = GetLastError();

			if (dwResult != ERROR_IO_PENDING)
			{
				if (dwResult == ERROR_PIPE_CONNECTED)
				{
					/** подключились без ожидания */
					asyncOperation.cancel();
					if (const auto ec = _pIocp->transit(
						this, connectPipeHandler); ec)
					{
						changeState(EIoState::close);
						closeHandle();
						return ec;
					}

					/** успешно подключились */
					counter.release();
					return {};
				}

				/** все, сваливаем */
				changeState(EIoState::close);
				closeHandle();
				asyncOperation.cancel();
				return std::error_code(dwResult, std::system_category());
			}
		} 

		/** успешно ожидаем */
		counter.release();
		return {};
	}
	catch (const std::exception& ex)
	{
		changeState(EIoState::close);
		closeHandle();
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code IListenNPipeClient::waitServerToConnect(
	const DWORD dwMilliseconds) noexcept
{
	UNREFERENCED_PARAMETER(dwMilliseconds);

	/** эта функция только для клиента */
	return std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
}
//------------------------------------------------------------------------------
void IListenNPipeClient::close() noexcept
{
	closeHandle();
	deleteClient();
}
//------------------------------------------------------------------------------
void IListenNPipeClient::release(
	const bool bIsWait) noexcept
{
	__super::release(false);

	/** закрываем NPipe */
	close();

	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//------------------------------------------------------------------------------
IListenNPipeClient::~IListenNPipeClient()
{
	release(true);
}
//------------------------------------------------------------------------------