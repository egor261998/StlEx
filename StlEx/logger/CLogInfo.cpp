#include "../stdafx.h"

using CLogInfo = stlex::logger::CLogInfo;

//------------------------------------------------------------------------------
CLogInfo::CLogInfo(
	const char* szFunction,
	const int iLine,
	const char* szFile) noexcept :
	_szFunction(szFunction),
	_liLine(iLine),
	_szFile(szFile)
{

}
//--------------------------------------------------------------------------