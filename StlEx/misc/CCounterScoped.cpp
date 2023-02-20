#include "../stdafx.h"

using CCounterScoped = stlex::misc::CCounterScoped;

//------------------------------------------------------------------------------
CCounterScoped::CCounterScoped(
	CCounter& counter,
	const size_t nCount) noexcept :
	_counter(counter)
{
	if (_counter.startOperation(nCount))
		_nCountStart = nCount;
}
//------------------------------------------------------------------------------
bool CCounterScoped::isStartOperation() const noexcept
{
	return _nCountStart > 0;
}
//------------------------------------------------------------------------------
void CCounterScoped::release(
	const bool bIsEndOperation) noexcept
{
	if (bIsEndOperation)
	{
		if (_nCountStart > 0)
			_counter.endOperation(_nCountStart);
	}

	_nCountStart = 0;
}
//------------------------------------------------------------------------------
CCounterScoped::~CCounterScoped()
{
	release(true);
}
//------------------------------------------------------------------------------