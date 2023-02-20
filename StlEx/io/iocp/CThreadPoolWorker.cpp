#include "../../stdafx.h"

using CIocp = stlex::io::iocp::CIocp;

//------------------------------------------------------------------------------
CIocp::CThreadPoolWorker::CThreadPoolWorker(
	CThreadPool& threadPool) :
	_threadPool(threadPool)
{
	try
	{
		misc::CCounterScoped counter(_threadPool);
		if (!counter.isStartOperation())
		{
			throw std::logic_error("!counter.isStartOperation()");
		}
		
		counter.release();
	}
	catch (const std::exception& ex)
	{
		threadPool._iocp.log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
void CIocp::CThreadPoolWorker::start(
	CThreadPool& threadPool)
{
	try
	{
		auto pThreadPoolWorker = std::make_shared<CThreadPoolWorker>(threadPool);
		pThreadPoolWorker->_thread = std::thread(
			&workerThread,
			&threadPool._iocp,
			std::ref(*pThreadPoolWorker.get()));

		threadPool._listWorker[pThreadPoolWorker.get()] = pThreadPoolWorker;
	}
	catch (const std::exception& ex)
	{
		threadPool._iocp.log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
void CIocp::CThreadPoolWorker::release(
	const bool bIsWait) noexcept
{
	/** ждать всех операций нужно тут */
	__super::release(bIsWait);
}
//------------------------------------------------------------------------------
CIocp::CThreadPoolWorker::~CThreadPoolWorker()
{
	/** ждем завершения */
	release(true);

	try
	{
		/** попытка закрыть ресурсы нити */
		if(_thread.joinable())
			_thread.detach();
	}
	catch (const std::exception& ex)
	{
		_threadPool._iocp.log(logger::ILogger::EMessageType::warning, ex);
	}

	_threadPool.endOperation();
}
//------------------------------------------------------------------------------