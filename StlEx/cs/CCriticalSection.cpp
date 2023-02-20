#include "../stdafx.h"

using CCriticalSection = stlex::cs::CCriticalSection;

/**
* 26135 - SAL для критической секции.
*/
#pragma warning (push)
#pragma warning (disable: 26135)
//------------------------------------------------------------------------------
CCriticalSection::CCriticalSection() noexcept
{
	InitializeCriticalSection(this);
}
//------------------------------------------------------------------------------
void CCriticalSection::lock() noexcept
{
	EnterCriticalSection(this);
}
//------------------------------------------------------------------------------
bool CCriticalSection::tryLock() noexcept
{
	return TryEnterCriticalSection(this);
}
//------------------------------------------------------------------------------
void CCriticalSection::unLock() noexcept
{
	LeaveCriticalSection(this);
}
//------------------------------------------------------------------------------
CCriticalSection::~CCriticalSection()
{
	DeleteCriticalSection(this);
}
//------------------------------------------------------------------------------
#pragma warning (pop)