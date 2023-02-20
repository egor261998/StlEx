#include "../stdafx.h"

using CCounter = stlex::misc::CCounter;

//------------------------------------------------------------------------------
stlex::cs::CCriticalSectionLockGuard CCounter::lockGuard() const noexcept
{
	return stlex::cs::CCriticalSectionLockGuard(_csCounter);
}
//------------------------------------------------------------------------------
bool CCounter::isInitialize() const noexcept
{
	const auto lock = lockGuard();

	return _isCounterInitialize;
}
//------------------------------------------------------------------------------
bool CCounter::startOperation(
	const size_t nCount) noexcept
{
	const auto lock = lockGuard();

	if (_isCounterInitialize)
		_nCounterCount += nCount;

	return _isCounterInitialize;
}
//------------------------------------------------------------------------------
bool CCounter::endOperation(
	const size_t nCount) noexcept
{
	bool bResultFree = false;
	bool isDelete = false;
	{
		/** отдельная область видимости для синхронизации */
		const auto lock = lockGuard();

		/** проверка на наличие операций */
		assert(_nCounterCount >= nCount);
		_nCounterCount -= nCount;
		
		/** обработка операции */
		processingOperation(bResultFree, isDelete);
	}

	if (bResultFree)
		_eventCounterFree.notify();

	if (isDelete)
	{
		/** увольняемся */
		delete this;
	}

	return bResultFree;
}
//------------------------------------------------------------------------------
void CCounter::processingOperation(
	bool& bResultFree,
	bool& isDelete) noexcept
{
	/** проверка произвольного ожидания */
	if (!_counterWait.empty())
	{
		try
		{
			if (const auto& it = _counterWait.find(_nCounterCount);
				it != _counterWait.end())
			{
				for (const auto& ev : it->second)
				{
					ev.notify();
				}

				_counterWait.erase(it);
			}
		}
		catch (const std::exception& ex)
		{
			UNREFERENCED_PARAMETER(ex);

			/** смерть */
			assert(false);
		}
	}

	/** завершение последней асинхронной операции */
	bResultFree = _nCounterCount == 0 && !_isCounterInitialize;
	isDelete = _isDeleteAfterEndOperation && !checkOperation();
}
//------------------------------------------------------------------------------
bool CCounter::checkOperation(
	const size_t nCount) const noexcept
{
	const auto lock = lockGuard();

	return _nCounterCount > nCount;
}
//------------------------------------------------------------------------------
void CCounter::waitOperation(
	const size_t nCount) const
{
	handle::CEvent ev;

	{
		/** отдельная область видимости для синхронизации */
		const auto lock = lockGuard();

		/** ожидание не нужно */
		if (_nCounterCount <= nCount)
			return;

		const auto& getList = [&]()
		{
			if (const auto it = _counterWait.find(nCount); it != _counterWait.end())
				return it->second;
			
			/** элемента нет */
			_counterWait[nCount] = std::list<handle::CEvent>();
			return _counterWait[nCount];
		};

		getList().push_back(ev);
	}

	/** ожидаем */
	ev.waitSignal();
}
//------------------------------------------------------------------------------
void CCounter::release(
	const bool bIsWait) noexcept
{
	bool isNeedWait = false;

	{
		/** отдельная область видимости для синхронизации */
		const auto lock = lockGuard();

		_isCounterInitialize = false;
		isNeedWait = _nCounterCount ? true : false;
	}

	if (bIsWait && isNeedWait)
		_eventCounterFree.waitSignal();
}
//------------------------------------------------------------------------------
bool CCounter::deleteAfterEndOperation() noexcept
{
	bool bResultFree = false;
	bool isDelete = false;
	{
		const auto lock = lockGuard();

		_isDeleteAfterEndOperation = true;

		/** обработка операции */
		processingOperation(bResultFree, isDelete);
	}

	if (bResultFree)
		_eventCounterFree.notify();

	if (isDelete)
	{
		/** увольняемся */
		delete this;
	}

	return isDelete;
}
//------------------------------------------------------------------------------
CCounter::~CCounter()
{
	release(true);
}
//------------------------------------------------------------------------------