#include "../../stdafx.h"

using IListenPipe = stlex::pipe::server::IListenPipe;

//------------------------------------------------------------------------------
IListenPipe::IListenPipe(
	const std::filesystem::path& pipePath,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) :
	CPathStorage(pipePath),
	_pIocp(pIocp)
{
	if (_pIocp == nullptr)
	{
		_pIocp = std::make_shared<io::iocp::CIocp>();
	}
}
//------------------------------------------------------------------------------
void IListenPipe::acceptClient() noexcept
{
	while (true)
	{
		try
		{				
			const auto lock = lockGuard();

			misc::CCounterScoped counter(*this);
			if (!counter.isStartOperation())
				break;

			/** создаем клиента */
			auto pClient = std::shared_ptr<IListenPipeClient>(
				createClient().release(),
				[](IListenPipeClient* const pClient) noexcept
				{
					if (pClient == nullptr)
						return;

					pClient->deleteAfterEndOperation();
				});

			_clients[pClient.get()] = pClient;			

			try
			{
				/** ожидание клиента асинхронно */
				if (const auto ec = pClient->waitClient(); ec)
				{
					deleteClient(pClient.get());
					if (connectErrorHandler(ec))
						continue;
				}

				break;
			}
			catch (const std::exception&)
			{
				deleteClient(pClient.get());
				throw;
			}		
		}
		catch (const std::exception& ex)
		{
			_pIocp->log(logger::ILogger::EMessageType::warning, ex);
			const auto ecException = 
				std::error_code(ERROR_EXTENDED_ERROR, std::system_category());
			if (!connectErrorHandler(ecException))
				break;
		}	
	}
}
//------------------------------------------------------------------------------
bool IListenPipe::connectErrorHandler(
	const std::error_code& ec) const noexcept
{
	UNREFERENCED_PARAMETER(ec);
	return true;
}
//------------------------------------------------------------------------------
void IListenPipe::deleteClient(
	IListenPipeClient* const pPipe) noexcept
{
	const auto lock = lockGuard();

	_clients.erase(pPipe);
}
//------------------------------------------------------------------------------
void IListenPipe::release(
	const bool bIsWait) noexcept
{
	__super::release(false);

	std::unordered_map<IListenPipeClient*, std::shared_ptr<IListenPipeClient>> clients;
	{
		const auto lock = lockGuard();

		clients = std::move(_clients);
	}

	/** закрываем всех клиентов */
	for (const auto& it : clients)
	{
		it.first->close();
	}

	clients.clear();

	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//------------------------------------------------------------------------------
IListenPipe::~IListenPipe()
{
	release(true);
}
//------------------------------------------------------------------------------