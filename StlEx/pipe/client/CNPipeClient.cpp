#include "../../stdafx.h"

using CNPipeClient = stlex::pipe::client::CNPipeClient;

//------------------------------------------------------------------------------
CNPipeClient::CNPipeClient(
	const std::filesystem::path& npipePath,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) :
	IPipe(npipePath, pIocp)
{

}
//------------------------------------------------------------------------------
std::error_code CNPipeClient::connectToServer(
	const DWORD dwRepeatCount,
	const DWORD dwMilliseconds)
{
	if (!isInitialize())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	{
		const auto lock = lockGuard();

		/** на случай если npipe уже открыт */
		if (getState() != EIoState::close)
			return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());

		changeState(EIoState::openning);
	}

	try
	{
		handle::CHandle hHandle;

		for (DWORD i = 0; i <= dwRepeatCount; i++)
		{
			hHandle = CreateFile(
				getPath().c_str(),
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED, NULL);
			if (!hHandle.isValid())
			{
				/** получаем ошибку */
				auto ec = std::error_code(GetLastError(), std::system_category());
				if (ec.value() == ERROR_PIPE_BUSY && i < dwRepeatCount)
				{
					/** ожидаем сервер */
					ec = waitServerToConnect(dwMilliseconds);
					if (ec && ec.value() != ERROR_SEM_TIMEOUT)
					{
						changeState(EIoState::close);
						return ec;
					}

					/** второй шанс */
					continue;
				}

				/** сторонн¤¤ ошибка */
				changeState(EIoState::close);
				return ec;
			}

			/** измен¤ем реежим */
			DWORD dwMode = PIPE_READMODE_MESSAGE;
			if (!SetNamedPipeHandleState(hHandle, &dwMode, NULL, NULL))
			{
				changeState(EIoState::close);
				return std::error_code(GetLastError(), std::system_category());
			}

			/** инициализаци¤ асинхронных операций */
			if (const auto ec = bindHandle(hHandle); ec)
			{
				changeState(EIoState::close);
				return ec;
			}

			/** подключено */
			return {};
		}

		/** подключени¤ нет */
		const auto ec = std::error_code(GetLastError(), std::system_category());
		changeState(EIoState::close);
		return ec;
	}
	catch (const std::exception& ex)
	{
		changeState(EIoState::close);
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code CNPipeClient::waitClient() noexcept
{
	/** эта функци¤ только дл¤ клиента */
	return std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
}
//------------------------------------------------------------------------------
std::error_code CNPipeClient::waitServerToConnect(
	const DWORD dwMilliseconds) noexcept
{
	if (!isInitialize())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	return WaitNamedPipe(getPath().c_str(), dwMilliseconds) ?
		std::error_code() : std::error_code(GetLastError(), std::system_category());
}
//------------------------------------------------------------------------------
void CNPipeClient::close() noexcept
{
	closeHandle();
}
//------------------------------------------------------------------------------
void CNPipeClient::release(
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
CNPipeClient::~CNPipeClient()
{
	release(true);
}
//------------------------------------------------------------------------------