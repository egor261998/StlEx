#include "../stdafx.h"

using CLoggerToConsole = stlex::logger::CLoggerToConsole;

//------------------------------------------------------------------------------
std::error_code CLoggerToConsole::logWrite(
	const EMessageType eMessageType,
	const std::wstring& message) noexcept
{
	UNREFERENCED_PARAMETER(eMessageType);

	if (wprintf(message.c_str()) == -1)
		return std::error_code(GetLastError(), std::system_category());

	return {};
}
//------------------------------------------------------------------------------