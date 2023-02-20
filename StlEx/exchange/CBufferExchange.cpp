#include "../stdafx.h"

using CBufferExchange = stlex::exchange::CBufferExchange;

/**
* 26493 - привидения в стиле C.
*/
#pragma warning (push)
#pragma warning (disable: 26493)
//------------------------------------------------------------------------------
void CBufferExchange::putData(
	const std::vector<BYTE>& buffer)
{
	_pBuffer = buffer;
	_sHead.dwSizeData = (DWORD)_pBuffer.size();
}
//------------------------------------------------------------------------------
#pragma warning (pop)