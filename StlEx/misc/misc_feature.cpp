#include "../stdafx.h"

/**
* 26481 - что-то вообще не понял.
* 26446 - обращение к индексу.
* 26482 - что-то вообще не понял.
*/
#pragma warning (push)
#pragma warning (disable: 26481 26446 26482)
//------------------------------------------------------------------------------
stlex::misc::EPathType stlex::misc::pathType(
	const std::filesystem::path& path) noexcept
{
	struct SPathTypes
	{
		EPathType eType;
		WCHAR name[32];
	};
	constexpr static SPathTypes pathTypes[] =
	{
		{ EPathType::npipe, L"\\\\.\\pipe\\" },
		{ EPathType::driver, L"\\\\.\\" }
	};
	
	for (DWORD i = 0; i < ARRAYSIZE(pathTypes); i++)
	{
		if (_wcsnicmp(
			&pathTypes[i].name[0],
			path.c_str(),
			wcslen(&pathTypes[i].name[0])) == 0)
		{
			return pathTypes[i].eType;
		}
	}

	return EPathType::file_or_directory;
}
//------------------------------------------------------------------------------
std::list<std::wstring> stlex::misc::toListStr(
	const std::wstring_view str)
{
	std::list<std::wstring> list;

	LPWSTR* szArgList = nullptr;
	int nArgs = 0;

	szArgList = CommandLineToArgvW(str.data(), &nArgs);
	if (szArgList == nullptr)
		throw std::runtime_error("CommandLineToArgvW failed with error:" +
			std::to_string(GetLastError()));

	try
	{
		for (int i = 0; i < nArgs; i++)
			list.push_back(szArgList[i]);
	}
	catch (const std::exception&)
	{
		LocalFree(szArgList);
		throw;
	}

	LocalFree(szArgList);
	return list;
}
//------------------------------------------------------------------------------
std::wstring stlex::misc::formatByte(
	const UINT64 nValue,
	const bool bIsSpeed,
	const bool bIsFullInformation,
	const UINT shift)
{
	std::wstring format;
	auto nValueFormat = nValue;
	if (nValueFormat < shift)
	{
		format = std::to_wstring(nValueFormat) + L" Байт";
	}
	else if (nValueFormat /= 1024; nValueFormat < shift)
	{
		format = std::to_wstring(nValueFormat) + L" КБ";
	}
	else if (nValueFormat /= 1024; nValueFormat < shift)
	{
		format = std::to_wstring(nValueFormat) + L" МБ";
	}
	else
	{
		format = std::to_wstring(nValueFormat) + L" ГБ";
	}

	if (bIsSpeed)
		format.append(L"/с");

	if (bIsFullInformation)
	{
		format.append(L" (" + std::to_wstring(nValue) +
			(bIsSpeed ? L" Байт/с)" : L" Байт)"));
	}

	return format;
}
//------------------------------------------------------------------------------
std::wstring stlex::misc::formatTime(
	const SYSTEMTIME systemTime,
	const bool bIsToLocal)
{
	SYSTEMTIME systemTimeFormat = {};

	if (bIsToLocal)
		SystemTimeToTzSpecificLocalTime(NULL, &systemTime, &systemTimeFormat);
	else
		memcpy(&systemTimeFormat, &systemTime, sizeof(SYSTEMTIME));

	WCHAR wStr[MAX_PATH] = {};
	swprintf_s(&wStr[0], ARRAYSIZE(wStr) - 1, L"%02u/%02u/%u %02u:%02u:%02u",
		systemTimeFormat.wMonth,
		systemTimeFormat.wDay,
		systemTimeFormat.wYear,
		systemTimeFormat.wHour,
		systemTimeFormat.wMinute,
		systemTimeFormat.wSecond);

	return &wStr[0];
}
//------------------------------------------------------------------------------
std::wstring stlex::misc::formatTime(
	const FILETIME fileTime,
	const bool bIsToLocal)
{
	SYSTEMTIME systemTime = {};
	FileTimeToSystemTime(&fileTime, &systemTime);

	return formatTime(systemTime, bIsToLocal);
}
//------------------------------------------------------------------------------
std::wstring stlex::misc::convertStr(
	const std::string_view str)
{
	std::wstring wstr;

	/** получаем количество символов */
	const int str_w_len = MultiByteToWideChar(
		CP_ACP,
		MB_ERR_INVALID_CHARS,
		str.data(),
		-1,
		NULL,
		NULL);
	if (str_w_len == 0)
		return wstr;

	/** преобразуем */
	wstr.resize(str.size());
	MultiByteToWideChar(
		CP_ACP,
		0,
		str.data(),
		-1,
		wstr.data(),
		str_w_len);

	return wstr;
}
//------------------------------------------------------------------------------
std::string stlex::misc::convertStr(
	const std::wstring_view wstr)
{
	std::string str;

	/** получаем количество символов */
	const int str_len = WideCharToMultiByte(
		CP_ACP,
		0,
		wstr.data(),
		-1,
		NULL,
		0,
		NULL,
		NULL);
	if (str_len == 0)
		return str;

	/** преобразуем */
	str.resize(wstr.size());
	WideCharToMultiByte(
		CP_ACP,
		0,
		wstr.data(),
		-1,
		str.data(),
		str_len,
		NULL,
		NULL);

	return str;
}
//------------------------------------------------------------------------------
#pragma warning (pop)