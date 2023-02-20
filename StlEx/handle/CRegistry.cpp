#include "../stdafx.h"

using CRegistry = stlex::handle::CRegistry;

/**
* 26451 - арифметика за область размера типа.
* 26493 - привидения в стиле C.
*/
#pragma warning (push)
#pragma warning (disable: 26451 26493)
//------------------------------------------------------------------------------
CRegistry::CRegistry(
	const HKEY hKey,
	const std::wstring& nameSubKey,
	const REGSAM amDesiredAccess,
	const DWORD dwOptions)
{
	_hKey = hKey;
	_nameSubKey = nameSubKey;
	_amDesiredAccess = amDesiredAccess;
	_dwOptions = dwOptions;
}
//------------------------------------------------------------------------------
std::error_code CRegistry::createKey()
{
	/** создаем ключ */
	HKEY hKeyCreate = {};
	DWORD dwDisposition = 0;
	if (const auto ec = std::error_code(RegCreateKeyEx(
		_hKey,
		_nameSubKey.c_str(),
		NULL,
		NULL,
		_dwOptions,
		_amDesiredAccess,
		NULL,
		&hKeyCreate,
		&dwDisposition), std::system_category()); ec)
		return ec;

	if (dwDisposition == REG_OPENED_EXISTING_KEY)
	{
		RegCloseKey(hKeyCreate);
		/** ключ уже был создан */
		return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());
	}
	
	try
	{
		/** закрываем предыдущий описатель */
		close();

		if (isValid(hKeyCreate))
		{
			/** описатель валидный */
			_pHandle = std::shared_ptr<void>(
				hKeyCreate,
				[](HANDLE hKeyClose) noexcept 
				{
					RegCloseKey(static_cast<HKEY>(hKeyClose));
				});
			return {};
		}

		return std::error_code(GetLastError(), std::system_category());
	}
	catch (const std::exception&)
	{
		RegCloseKey(hKeyCreate);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code CRegistry::openKey()
{
	/** создаем ключ */
	HKEY hKeyCreate = {};
	if (const auto ec = std::error_code(RegOpenKeyEx(
		_hKey,
		_nameSubKey.c_str(),
		_dwOptions,
		_amDesiredAccess,
		&hKeyCreate), std::system_category()); ec)
		return ec;

	try
	{
		/** закрываем предыдущий описатель */
		close();

		if (isValid(hKeyCreate))
		{
			/** описатель валидный */
			_pHandle = std::shared_ptr<void>(
				hKeyCreate,
				[](const HANDLE hKeyClose) noexcept 
				{
					RegCloseKey(static_cast<HKEY>(hKeyClose));
				});
			return {};
		}

		return std::error_code(GetLastError(), std::system_category());
	}
	catch (const std::exception&)
	{
		RegCloseKey(hKeyCreate);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code CRegistry::queryInfo(
	DWORD& dwSubKeys,
	DWORD& dwMaxSubKeyLength,
	DWORD& dwValues,
	DWORD& dwMaxValueNameLength,
	FILETIME& lastWriteTime) const noexcept
{
	return std::error_code(RegQueryInfoKey(
		(HKEY)getHandle(),
		NULL,
		NULL,
		NULL,
		&dwSubKeys,
		&dwMaxSubKeyLength,
		NULL,
		&dwValues,
		&dwMaxValueNameLength,
		NULL,
		NULL,
		&lastWriteTime), std::system_category());
}
//------------------------------------------------------------------------------
std::error_code CRegistry::enumSubKey(
	const DWORD dwSubKeyIndex,
	std::wstring& nameSubKey,
	FILETIME& lastWriteTime) const
{
	DWORD dwNameSize = (DWORD)nameSubKey.size();

	/** запрос информации */
	if (const auto ec = std::error_code(RegEnumKeyEx(
		(HKEY)getHandle(),
		dwSubKeyIndex,
		nameSubKey.data(),
		&dwNameSize,
		NULL,
		NULL,
		NULL,
		&lastWriteTime), std::system_category()); ec)
		return ec;

	nameSubKey.resize(dwNameSize);
	return {};
}
//------------------------------------------------------------------------------
std::error_code CRegistry::enumValue(
	const DWORD dwSubKeyIndex,
	std::wstring& nameValue,
	DWORD& dwValueType) const
{
	DWORD dwNameSize = (DWORD)nameValue.size();

	/** запрос информации */
	if (const auto ec = std::error_code(RegEnumValue(
		(HKEY)getHandle(),
		dwSubKeyIndex,
		nameValue.data(),
		&dwNameSize,
		NULL,
		&dwValueType,
		NULL,
		NULL), std::system_category()); ec)
		return ec;

	nameValue.resize(dwNameSize);
	return {};
}
//------------------------------------------------------------------------------
std::error_code CRegistry::updateInfo(
	const bool bIsUpdateSubKey)
{
	DWORD dwKeys = 0;
	DWORD dwMaxSubKeyLength = 0;
	DWORD dwValues = 0;
	DWORD dwMaxValueNameLength = 0;

	/** запрос информации */
	if (const auto ec = queryInfo(
		dwKeys,
		dwMaxSubKeyLength,
		dwValues,
		dwMaxValueNameLength,
		_lastWriteTime); ec)
		return ec;

	/** обновляем подключи */
	_keyList.clear();
	for (DWORD i = 0; i < dwKeys; i++)
	{
		CRegistry registry;
		registry._nameSubKey.resize(dwMaxSubKeyLength*2);
		if (const auto ec = enumSubKey(
			i, registry._nameSubKey, registry._lastWriteTime); ec)
			return ec;

		registry._hKey = static_cast<HKEY>(getHandle());
		registry._dwOptions = _dwOptions;
		registry._amDesiredAccess = _amDesiredAccess;
	
		if (bIsUpdateSubKey)
		{		
			if (const auto ec = registry.openKey(); ec)
				return ec;

			if (const auto ec = registry.updateInfo(
				bIsUpdateSubKey); ec)
				return ec;
		}

		_keyList.push_back(std::move(registry));
	}

	/** обновляем значения */
	_valueList.clear();
	for (DWORD i = 0; i < dwValues; i++)
	{
		std::wstring nameValue;
		DWORD dwValueType = 0;
		nameValue.resize(dwMaxValueNameLength*2);
		if (const auto ec = enumValue(
			i, nameValue, dwValueType); ec)
			return ec;

		_valueList.push_back({ std::move(nameValue), dwValueType });
	}

	return {};
}
//------------------------------------------------------------------------------
std::error_code CRegistry::queryValue(
	const std::wstring& nameValue,
	DWORD& dwValue) const noexcept
{
	DWORD dwValueType = 0;
	DWORD dwValueSize = sizeof(dwValue);

	/** запрос значения */
	if (const auto ec = std::error_code(RegQueryValueEx(
		(HKEY)getHandle(),
		nameValue.c_str(),
		NULL,
		&dwValueType,
		(PBYTE)&dwValue,
		&dwValueSize), std::system_category()); ec)
		return ec;

	if (dwValueType != REG_DWORD || dwValueSize != sizeof(dwValue))
		return std::error_code(ERROR_INVALID_DATA, std::system_category());

	return {};
}
//------------------------------------------------------------------------------
std::error_code CRegistry::setValue(
	const std::wstring& nameValue,
	const DWORD dwValue) const noexcept
{
	/** установка значения */
	return std::error_code(RegSetValueEx(
		(HKEY)getHandle(),
		nameValue.c_str(),
		NULL,
		REG_DWORD,
		(PBYTE)&dwValue,
		sizeof(dwValue)), std::system_category());
}
//------------------------------------------------------------------------------
std::error_code CRegistry::queryValue(
	const std::wstring& nameValue,
	std::wstring& stringValue) const
{
	DWORD dwValueType = 0;
	DWORD dwValueSize = 0;

	/** запрос информации о значении */
	if (const auto ec = std::error_code(RegQueryValueEx(
		(HKEY)getHandle(),
		nameValue.c_str(),
		NULL,
		&dwValueType,
		NULL,
		&dwValueSize), std::system_category()); ec)
		return ec;

	if (dwValueType != REG_SZ)
		return std::error_code(ERROR_INVALID_DATA, std::system_category());

	stringValue.resize(dwValueSize / sizeof(WCHAR));

	/** запрос значения */
	if (const auto ec = std::error_code(RegQueryValueEx(
		(HKEY)getHandle(),
		nameValue.c_str(),
		NULL,
		&dwValueType,
		(PBYTE)stringValue.data(),
		&dwValueSize), std::system_category()); ec)
		return ec;

	while (true)
	{
		if (const auto npos = stringValue.find(L'\0');
			npos != std::wstring::npos)
		{
			stringValue.erase(npos);
			continue;
		}

		break;
	}	

	return {};
}
//------------------------------------------------------------------------------
std::error_code CRegistry::setValue(
	const std::wstring& nameValue,
	const std::wstring& stringValue) const noexcept
{
	/** установка значения */
	return std::error_code(RegSetValueEx(
		(HKEY)getHandle(),
		nameValue.c_str(),
		NULL,
		REG_SZ,
		(PBYTE)stringValue.data(),
		(DWORD)(stringValue.length() * sizeof(WCHAR))), std::system_category());
}
//------------------------------------------------------------------------------
std::error_code CRegistry::deleteValue(
	const std::wstring& nameValue) const noexcept
{
	/** удаления значения */
	return std::error_code(RegDeleteValue(
		(HKEY)getHandle(),
		nameValue.c_str()), std::system_category());
}
//------------------------------------------------------------------------------
std::error_code CRegistry::deleteSubKey(
	const std::wstring& nameSubKey,
	const REGSAM amDesiredAccess) const noexcept
{
	/** удаления подключа */
	return std::error_code(RegDeleteKeyEx(
		(HKEY)getHandle(),
		nameSubKey.c_str(),
		amDesiredAccess,
		0), std::system_category());
}
//------------------------------------------------------------------------------
CRegistry::~CRegistry()
{
	close();
}
//------------------------------------------------------------------------------
#pragma warning (pop)