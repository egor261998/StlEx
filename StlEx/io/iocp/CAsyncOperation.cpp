#include "../../stdafx.h"

using CIocp = stlex::io::iocp::CIocp;

/**
* 26401 - предупреждение связанное с использованием delete.
* 26409 - предупреждение связанное с использованием delete.
*/
#pragma warning (push)
#pragma warning (disable: 26401 26409)
//------------------------------------------------------------------------------
CIocp::CAsyncOperation::CAsyncOperation(
	CIocp* pIocp,
	const PVOID pCompletionRoutineContext,
	const FAsyncCompletion fCompletionRoutine) :
	_pIocp(pIocp)
{
	if (_pIocp == nullptr)
	{
		throw std::logic_error("_pIocp == nullptr");
	}

	_pCompletionRoutineContext = pCompletionRoutineContext;
	_fCompletionRoutine = fCompletionRoutine;
}
//------------------------------------------------------------------------------
bool CIocp::CAsyncOperation::updateCounter(
	CCounter* pCounter) noexcept
{
	if (_pCounter != nullptr)
	{
		/** сброс счетчика */
		if (_isStartOperation)
		{		
			_pCounter->endOperation();
			_isStartOperation = false;
		}
		_pCounter = nullptr;
	}

	if (pCounter)
	{
		/** попытка старта новой асинхронной операции */
		_isStartOperation = pCounter->startOperation();
		if (_isStartOperation)
			_pCounter = pCounter;
	}
	else
	{
		/** счетчика нет */
		_isStartOperation = false;
	}

	return _isStartOperation;
}
//------------------------------------------------------------------------------
bool CIocp::CAsyncOperation::isStartOperation() const noexcept
{
	return _isStartOperation;
}
//------------------------------------------------------------------------------
void CIocp::CAsyncOperation::cancel() noexcept
{
	/** обновление счетчика */
	updateCounter();

	if (_pThreadPoolWorker != nullptr)
	{
		_pThreadPoolWorker->endOperation();
		_pThreadPoolWorker = nullptr;
	}

	auto& iocp = *_pIocp;
	if (iocp._listFreeAsyncOperation.size() < _nMaxReservAsyncOperation)
	{
		/** добавить операцию обратно в пул */
		iocp._listFreeAsyncOperation.push(this);
	}
	else
	{
		/** увольняемся */
		delete this;
	}

	iocp.endOperation();
}
//------------------------------------------------------------------------------
#pragma warning (pop)