#include "../stdafx.h"

using CCriticalSectionLockGuard = stlex::cs::CCriticalSectionLockGuard;

//------------------------------------------------------------------------------
CCriticalSectionLockGuard::CCriticalSectionLockGuard(
	CCriticalSection& cs) noexcept :
	_cs(cs)
{
	_cs.lock();
}
//------------------------------------------------------------------------------
void CCriticalSectionLockGuard::free() noexcept
{
	if (_bIsLock)
	{
		_bIsLock = false;
		_cs.unLock();
	}
}
//------------------------------------------------------------------------------
CCriticalSectionLockGuard::~CCriticalSectionLockGuard()
{
	free();
}
//------------------------------------------------------------------------------