#include "../stdafx.h"

using CFileSystemObject = stlex::filesystem::CFileSystemObject;

//------------------------------------------------------------------------------
CFileSystemObject::CFileSystemObject(
	const std::filesystem::path& fileSystemPath,
	const EFileSystemObjectType eFileSystemObjectType,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) :
	IAsyncIo(pIocp),
	CPathStorage(fileSystemPath),
	_eFileSystemObjectType(eFileSystemObjectType)
{

}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::getInfo(
	BY_HANDLE_FILE_INFORMATION& info,
	const bool bIsCache) const noexcept
{
	const auto lock = lockGuard();

	if (bIsCache && _bIsCache)
	{
		/** получение информации из кэша */
		return getCache(info);
	}

	/** получаем информацию */
	if (!GetFileInformationByHandle(getHandle(), &info))
		return std::error_code(GetLastError(), std::system_category());

	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::getInfo(
	BY_HANDLE_FILE_INFORMATION& info,
	const bool bIsCache) noexcept
{
	const auto lock = lockGuard();

	if (bIsCache && _bIsCache)
	{
		/** получение информации из кэша */
		return getCache(info);
	}

	/** получаем информацию */
	if (!GetFileInformationByHandle(getHandle(), &info))
		return std::error_code(GetLastError(), std::system_category());

	/** сохранения кэша */
	updateCache(info);

	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::setInfo(
	const FILE_BASIC_INFO& info,
	const bool bIsCache) noexcept
{
	const auto lock = lockGuard();

	/** устанавливаем информацию о файле */
	if (!SetFileInformationByHandle(
		getHandle(),
		FileBasicInfo,
		(PVOID)&info,
		sizeof(info)))
		return std::error_code(GetLastError(), std::system_category());

	if (bIsCache)
	{
		/** обновить кэш */
		BY_HANDLE_FILE_INFORMATION infoForCahce = {};
		if (const auto ec = getInfo(infoForCahce, false); ec)
			return ec;
	}

	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::getAttributes(
	DWORD& dwAttributes,
	const bool bIsCache) const noexcept
{
	const auto lock = lockGuard();

	/** получаем информацию об атрибутов */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache); ec)
		return ec;

	dwAttributes = info.dwFileAttributes;
	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::getAttributes(
	DWORD& dwAttributes,
	const bool bIsCache) noexcept
{
	const auto lock = lockGuard();

	/** получаем информацию об атрибутов */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache); ec)
		return ec;

	dwAttributes = info.dwFileAttributes;
	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::setAttributes(
	const DWORD dwAttributes,
	const bool bIsCache) noexcept
{
	const auto lock = lockGuard();

	/** получаем информацию о файле */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache); ec)
		return ec;

	/** копируем предыдущее */
	FILE_BASIC_INFO sFileBasicInfo = { };
	memcpy(&sFileBasicInfo.CreationTime,
		&info.ftLastAccessTime,
		sizeof(info.ftLastAccessTime));
	memcpy(&sFileBasicInfo.LastAccessTime,
		&info.ftLastAccessTime,
		sizeof(info.ftLastAccessTime));
	memcpy(&sFileBasicInfo.LastWriteTime,
		&info.ftLastWriteTime,
		sizeof(info.ftLastWriteTime));

	/** устанавливаем атрибуты */
	sFileBasicInfo.FileAttributes = dwAttributes;
	if (const auto ec = setInfo(sFileBasicInfo, bIsCache); ec)
		return ec;

	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::getTimeAttributes(
	FILETIME& ftCreationTime,
	FILETIME& ftLastAccessTime,
	FILETIME& ftLastWriteTime,
	const bool bIsCache) const noexcept
{
	const auto lock = lockGuard();

	/** получаем информацию о файле */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache); ec)
		return ec;

	ftCreationTime = info.ftCreationTime;
	ftLastAccessTime = info.ftLastAccessTime;
	ftLastWriteTime = info.ftLastWriteTime;
	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::getTimeAttributes(
	FILETIME& ftCreationTime,
	FILETIME& ftLastAccessTime,
	FILETIME& ftLastWriteTime,
	const bool bIsCache) noexcept
{
	const auto lock = lockGuard();

	/** получаем информацию о файле */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache); ec)
		return ec;

	ftCreationTime = info.ftCreationTime;
	ftLastAccessTime = info.ftLastAccessTime;
	ftLastWriteTime = info.ftLastWriteTime;
	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::setTimeAttributes(
	const FILETIME& ftCreationTime,
	const FILETIME& ftLastAccessTime,
	const FILETIME& ftLastWriteTime,
	const bool bIsCache) noexcept
{
	const auto lock = lockGuard();

	/** получаем информацию о файле */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache); ec)
		return ec;

	/** копируем предыдущее */
	FILE_BASIC_INFO sFileBasicInfo = { };
	memcpy(&sFileBasicInfo.CreationTime,
		&ftCreationTime,
		sizeof(ftCreationTime));
	memcpy(&sFileBasicInfo.LastAccessTime,
		&ftLastAccessTime,
		sizeof(ftLastAccessTime));
	memcpy(&sFileBasicInfo.LastWriteTime,
		&ftLastWriteTime,
		sizeof(ftLastWriteTime));

	/** устанавливаем атрибуты */
	sFileBasicInfo.FileAttributes = info.dwFileAttributes;
	if (const auto ec = setInfo(sFileBasicInfo, bIsCache); ec)
		return ec;

	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::getAllAttributes(
	DWORD& dwAttributes,
	FILETIME& ftCreationTime,
	FILETIME& ftLastAccessTime,
	FILETIME& ftLastWriteTime,
	const bool bIsCache) const noexcept
{
	const auto lock = lockGuard();

	/** получаем информацию о файле */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache); ec)
		return ec;

	dwAttributes = info.dwFileAttributes;
	ftCreationTime = info.ftCreationTime;
	ftLastAccessTime = info.ftLastAccessTime;
	ftLastWriteTime = info.ftLastWriteTime;
	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::getAllAttributes(
	DWORD& dwAttributes,
	FILETIME& ftCreationTime,
	FILETIME& ftLastAccessTime,
	FILETIME& ftLastWriteTime,
	const bool bIsCache) noexcept
{
	const auto lock = lockGuard();

	/** получаем информацию о файле */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache); ec)
		return ec;

	dwAttributes = info.dwFileAttributes;
	ftCreationTime = info.ftCreationTime;
	ftLastAccessTime = info.ftLastAccessTime;
	ftLastWriteTime = info.ftLastWriteTime;
	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::setAllAttributes(
	const DWORD dwAttributes,
	const FILETIME& ftCreationTime,
	const FILETIME& ftLastAccessTime,
	const FILETIME& ftLastWriteTime,
	const bool bIsCache) noexcept
{
	const auto lock = lockGuard();

	/** получаем информацию о файле */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache); ec)
		return ec;

	/** копируем предыдущее */
	FILE_BASIC_INFO sFileBasicInfo = { };
	memcpy(&sFileBasicInfo.CreationTime,
		&ftCreationTime,
		sizeof(ftCreationTime));
	memcpy(&sFileBasicInfo.LastAccessTime,
		&ftLastAccessTime,
		sizeof(ftLastAccessTime));
	memcpy(&sFileBasicInfo.LastWriteTime,
		&ftLastWriteTime,
		sizeof(ftLastWriteTime));

	/** устанавливаем атрибуты */
	sFileBasicInfo.FileAttributes = dwAttributes;
	if (const auto ec = setInfo(sFileBasicInfo, bIsCache); ec)
		return ec;

	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::getInfo(
	const std::filesystem::path& fileSystemPath,
	BY_HANDLE_FILE_INFORMATION& info)
{
	std::error_code ec;
	bool bIsDirectory = false;
	/** проверка наличия объекта */
	if (const auto bIsExist = std::filesystem::exists(fileSystemPath, ec);
		ec || !bIsExist)
	{
		/** ошибка работы функции */
		if (ec)
			return ec;

		/** объекта не существует */
		return std::error_code(ERROR_PATH_NOT_FOUND, std::system_category());
	}

	/** проверка наличия объекта */
	if (bIsDirectory = std::filesystem::is_directory(fileSystemPath, ec);
		ec)
	{
		/** ошибка работы функции */
		return ec;
	}

	/** создаем объект файловой системы */
	handle::CHandle hFile = CreateFile(
		fileSystemPath.c_str(),
		0, 0,
		NULL,
		OPEN_EXISTING, bIsDirectory ? FILE_FLAG_BACKUP_SEMANTICS : 0,
		NULL);
	if (!hFile.isValid())
		return std::error_code(GetLastError(), std::system_category());

	/** получаем информацию */
	if (!GetFileInformationByHandle(hFile, &info))
		return std::error_code(GetLastError(), std::system_category());

	/** успех получения информации */
	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::setInfo(
	const std::filesystem::path& fileSystemPath,
	const FILE_BASIC_INFO& info)
{
	std::error_code ec;
	bool bIsDirectory = false;
	/** проверка наличия объекта */
	if (const auto bIsExist = std::filesystem::exists(fileSystemPath, ec);
		ec || !bIsExist)
	{
		/** ошибка работы функции */
		if (ec)
			return ec;

		/** объекта не существует */
		return std::error_code(ERROR_PATH_NOT_FOUND, std::system_category());
	}

	/** проверка наличия объекта */
	if (bIsDirectory = std::filesystem::is_directory(fileSystemPath, ec);
		ec)
	{
		/** ошибка работы функции */
		return ec;
	}

	/** создаем объект файловой системы */
	handle::CHandle hFile = CreateFile(
		fileSystemPath.c_str(),
		0, 0,
		NULL,
		OPEN_EXISTING, bIsDirectory ? FILE_FLAG_BACKUP_SEMANTICS : 0,
		NULL);
	if (!hFile.isValid())
		return std::error_code(GetLastError(), std::system_category());

	/** устанавливаем информацию */
	if (!SetFileInformationByHandle(
		hFile,
		FileBasicInfo,
		(PVOID)&info,
		sizeof(info)))
		return std::error_code(GetLastError(), std::system_category());

	/** успех установки информации */
	return {};
}
//------------------------------------------------------------------------------
std::error_code CFileSystemObject::getCache(
	BY_HANDLE_FILE_INFORMATION& info) const noexcept 
{
	const auto lock = lockGuard();

	if (!_bIsCache)
		return std::error_code(ERROR_NOT_FOUND, std::system_category());

	info = _infoCahce;
	return {};
}
//------------------------------------------------------------------------------
void CFileSystemObject::updateCache(
	const BY_HANDLE_FILE_INFORMATION& info) noexcept
{
	const auto lock = lockGuard();

	/** сохранения кэша */
	_infoCahce = info;
	_bIsCache = true;
}
//------------------------------------------------------------------------------
void CFileSystemObject::resetCache() noexcept
{
	const auto lock = lockGuard();

	_bIsCache = false;
}
//------------------------------------------------------------------------------
void CFileSystemObject::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);
}
//------------------------------------------------------------------------------
CFileSystemObject::~CFileSystemObject()
{
	release(true);
}
//------------------------------------------------------------------------------