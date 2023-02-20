#include "../../stdafx.h"

using CIocp = stlex::io::iocp::CIocp;

//------------------------------------------------------------------------------
CIocp::CThreadPool::CThreadPool(
	CIocp& iocp,
	const DWORD minThreadCount,
	const DWORD maxThreadCount) :
	_iocp(iocp)
{
	try
	{
		if (minThreadCount > maxThreadCount || minThreadCount < 1)
		{
			throw std::invalid_argument(
				"minThreadCount > maxThreadCount || minThreadCount < 1");
		}

		_nMinThreadCount = minThreadCount;		
		_nMaxThreadCount = maxThreadCount;

		try
		{
			/** добавляем минимальное количество нитей */
			for (DWORD i = 0; i < minThreadCount; i++)
				canAddWorker(true);
		}
		catch (const std::exception&)
		{
			/** нужно свернуться */
			release(true);

			/** падаем дальше */
			throw;
		}
	}
	catch (const std::exception& ex)
	{
		_iocp.log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
void CIocp::CThreadPool::delWorker(
	const CThreadPoolWorker& threadPoolWorker) noexcept
{
	const auto lock = lockGuard();

	if (threadPoolWorker._bIsFree)
		_nFreeThreadCount--;

	_listWorker.erase(&threadPoolWorker);
}
//------------------------------------------------------------------------------
bool CIocp::CThreadPool::canFreeWorker(
	CThreadPoolWorker& threadPoolWorker) noexcept
{
	const auto lock = lockGuard();

	const auto nCurrentThreadCount = _listWorker.size();

	/** количество нитей в пуле минимально */
	if (nCurrentThreadCount <= _nMinThreadCount)
		return false;

	/** остальные нити также высвобождаются, высвободим эту
		и в пуле боле не останется нитей */
	if ((nCurrentThreadCount - _nFreeThreadCount) <= _nMinThreadCount)
		return false;

	/** освободим эту нить и в пуле будут только занятые нити */
	if ((nCurrentThreadCount - _nFreeThreadCount - 1) == _nBusyThreadCount)
		return false;

	/** проверка на что на ните не висят операции в обработке */
	if (threadPoolWorker.checkOperation())
		return false;

	_nFreeThreadCount++;
	threadPoolWorker._bIsFree = true;

	return true;
}
//------------------------------------------------------------------------------
bool CIocp::CThreadPool::canAddWorker(
	bool bAddAnyway)
{
	const auto lock = lockGuard();

	if (!bAddAnyway)
		_nBusyThreadCount++;

	const auto nCurrentThreadCount = _listWorker.size();

	/** количество нитей в пуле максимально */
	if (nCurrentThreadCount >= _nMaxThreadCount && !bAddAnyway)
		return false;

	/** не все нити заняты, новая нить не нужна */
	if ((nCurrentThreadCount - _nFreeThreadCount) > _nBusyThreadCount &&
		!bAddAnyway)
		return false;

	/** добавляем новую нить */
	try
	{
		if (isInitialize())
			CThreadPoolWorker::start(*this);
		else
			return false;
	}
	catch (const std::exception& ex)
	{
		/** если во время создания объекта вызывается исключение
			то необходимо изменить количество нитей обратно */
		_iocp.log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}

	return true;
}
//------------------------------------------------------------------------------
void CIocp::CThreadPool::decrementBusyWorker() noexcept
{
	const auto lock = lockGuard();

	_nBusyThreadCount--;
}
//------------------------------------------------------------------------------
void CIocp::CThreadPool::release(
	const bool bIsWait) noexcept
{
	/** завершаем работу */
	__super::release(false);

	std::unordered_map<
		const CThreadPoolWorker*, std::shared_ptr<CThreadPoolWorker>> listWorker;

	{
		/** отдельная область видимости для синхронизации */
		const auto lock = lockGuard();

		listWorker = std::move(_listWorker);
		const auto nCurrentThreadCount = listWorker.size();
		for (DWORD i = 0; i < nCurrentThreadCount; i++)
		{
			/** посылаем запрос завершения */
			if (!PostQueuedCompletionStatus(
				_iocp._hIocp,
				0,
				(ULONG_PTR)this,
				NULL))
			{
				_iocp.log(
					logger::ILogger::EMessageType::warning,
					L"PostQueuedCompletionStatus failed",
					std::error_code(GetLastError(), std::system_category()));
			}
		}
	}

	/** необходим дождаться всех нитей */
	try
	{
		for (auto& it : listWorker)
		{
			try
			{
				it.second->_thread.join();
			}
			catch (const std::exception& ex)
			{
				_iocp.log(
					logger::ILogger::EMessageType::warning,
					ex);
			}
		}
	}
	catch (const std::exception& ex)
	{
		_iocp.log(
			logger::ILogger::EMessageType::warning,
			ex);
	}	
	listWorker.clear();

	/** ожидать нужно тут */
	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//------------------------------------------------------------------------------
CIocp::CThreadPool::~CThreadPool()
{
	/** ждем завершения */
	release(true);
}
//------------------------------------------------------------------------------