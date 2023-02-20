#include "../../stdafx.h"

using IListenPipeClient = stlex::pipe::server::IListenPipe::IListenPipeClient;

//------------------------------------------------------------------------------
IListenPipeClient::IListenPipeClient(
	IListenPipe& listenPipe) :
	_listenPipe(listenPipe),
	IPipe(listenPipe.getPath(), listenPipe._pIocp)
{
	try
	{
		misc::CCounterScoped counter(_listenPipe);
		if (!counter.isStartOperation())
		{
			throw std::logic_error("!counter.isStartOperation()");
		}

		counter.release();
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
void IListenPipeClient::deleteClient() noexcept
{
	/** удалим из списка клиентов */
	_listenPipe.deleteClient(this);
}
//------------------------------------------------------------------------------
void IListenPipeClient::connectPipeHandler(
	const io::iocp::CAsyncOperation& asyncOperation) noexcept
{
	/** получаем контекст клиента */
	assert(asyncOperation._pCompletionRoutineContext != nullptr);
	auto& pipeClient = *static_cast<IListenPipeClient*>(
		asyncOperation._pCompletionRoutineContext);

	if (asyncOperation._ec)
	{
		if (pipeClient._listenPipe.isInitialize())
		{
			/** создаем клиента если можно */
			if (pipeClient._listenPipe.connectErrorHandler(asyncOperation._ec))
				pipeClient._listenPipe.acceptClient();
		}

		/** отключаем текущего */
		pipeClient.changeState(EIoState::close);
		pipeClient.deleteClient();
	}
	else
	{
		/** создаем нового клиента */
		pipeClient._listenPipe.acceptClient();

		/** подключаем текущего */
		pipeClient.changeState(EIoState::open);
		pipeClient.openHandler();
	}

	pipeClient.endOperation();
}
//------------------------------------------------------------------------------
void IListenPipeClient::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);
}
//------------------------------------------------------------------------------
IListenPipeClient::~IListenPipeClient()
{
	release(true);

	deleteClient();

	/** необходимо сбросить механизм ввода/вывода,
	он используется в слушателе, а слушатель можешь разрушаться быстрее.
	если механизм ввода/вывода остается последний в этом объекте,
	то возможен его дедлок, его необходимо сбросить перед снятием ссылки со слушателя */
	_pIocp.reset();

	/** убираем ссылку с родительного объекта */
	_listenPipe.endOperation();
}
//------------------------------------------------------------------------------