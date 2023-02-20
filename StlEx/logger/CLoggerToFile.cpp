#include "../stdafx.h"

using CLoggerToFile = stlex::logger::CLoggerToFile;

/**
* 6387 - проверка на 0
*/
#pragma warning (push)
#pragma warning (disable: 6387)
//------------------------------------------------------------------------------
CLoggerToFile::CLoggerToFile(
	const std::filesystem::path& logPath) :
	_logPath(logPath)
{
	
}
//------------------------------------------------------------------------------
std::error_code CLoggerToFile::logWrite(
	const EMessageType eMessageType,
	const std::wstring& wStr) noexcept
{
	UNREFERENCED_PARAMETER(eMessageType);
	
	FILE* f = nullptr;
	if (const auto ec = std::error_code(
		_wfopen_s(&f, _logPath.c_str(), L"a"), std::system_category()); ec)
		return ec;

	if (fwprintf(f, wStr.c_str()) == -1)
	{
		fclose(f);
		return std::error_code(GetLastError(), std::system_category());
	}

	fclose(f);
	return {};
}
//------------------------------------------------------------------------------
#pragma warning (pop)