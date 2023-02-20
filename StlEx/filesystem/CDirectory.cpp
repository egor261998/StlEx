#include "../stdafx.h"

using CDirectory = stlex::filesystem::CDirectory;

//------------------------------------------------------------------------------
CDirectory::CDirectory(
	const std::filesystem::path& directoryPath,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) : 
	CDirectory(directoryPath, pIocp, *this)
{
	
}
//------------------------------------------------------------------------------
CDirectory::CDirectory(
	const std::filesystem::path& directoryPath,
	const std::shared_ptr<io::iocp::CIocp>& pIocp,
	CDirectory& parent) : 
	CFileSystemObject(directoryPath, EFileSystemObjectType::eDirectory, pIocp),
	_parent(parent)
{
	
}
//------------------------------------------------------------------------------
std::error_code CDirectory::createDirectory(
	const bool isSubOpenDirectory,
	const DWORD dwDesiredAccess,
	const DWORD dwShareMode,
	const DWORD dwCreationDisposition,
	const DWORD dwFlagsAndAttributes)
{
	if (!isInitialize())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	{
		const auto lock = lockGuard();

		/** сброс кэша */
		resetCache();

		/** на случай если папка уже открыта */
		if (getState() != EIoState::close)
			return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());

		changeState(EIoState::openning);
	}

	/** сохранение данных */
	_isSubOpenDirectory = isSubOpenDirectory;
	_dwDesiredAccess = dwDesiredAccess;
	_dwShareMode = dwShareMode;
	_dwFlagsAndAttributes = dwFlagsAndAttributes;

	try
	{
		const auto& directoryPath = getPath();

		/** счетчик проходов */
		size_t nCount = 0;
		while (nCount < MAXBYTE)
		{
			/** попытка открыть папку с ходу */
			handle::CHandle hHandle = CreateFile(
				directoryPath.c_str(),
				dwDesiredAccess,
				dwShareMode,
				NULL,
				OPEN_EXISTING,
				dwFlagsAndAttributes | FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				NULL);

			if (!hHandle.isValid())
			{
				if (const auto dwResult = GetLastError();
					dwResult != ERROR_FILE_NOT_FOUND &&
					dwResult != ERROR_PATH_NOT_FOUND)
				{
					/** сторонн¤¤ ошибка */
					changeState(EIoState::close);
					return std::error_code(dwResult, std::system_category());
				}

				switch (dwCreationDisposition)
				{
				case CREATE_NEW:
				case CREATE_ALWAYS:
				case OPEN_ALWAYS:
				{
					/** папки нет */
					std::error_code ec;
					std::filesystem::create_directories(directoryPath, ec);
					if (ec)
					{
						changeState(EIoState::close);
						return ec;
					}

					break;
				}
				default:
					break;
				}

				/** попытка открыть папку еще раз */
				nCount++;
				continue;
			}

			/** инициализаци¤ асинхронных операций */
			if (const auto ec = bindHandle(hHandle); ec)
			{
				changeState(EIoState::close);
				return ec;
			}

			if (auto ec = enumDirectory(); ec)
			{
				/** ошибка операций с объектом */
				close();
				return ec;
			}

			break;
		}
		
		return {};
	}
	catch (const std::exception& ex)
	{
		close();
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::list<std::filesystem::path> CDirectory::getSubFilesystemObject(
	const bool bFile,
	const bool bDirectory,
	const std::filesystem::path& proximatePath) const
{
	const auto lock = lockGuard();

	try
	{
		std::list<std::filesystem::path> listPath;

		for (const auto& [k, v] : _listFileSystemObject)
		{
			DWORD dwAttributes = {};
			if (const auto ec = v->getAttributes(
				dwAttributes, true); ec)
			{
				/** ошибка получени¤ атрибутов */
				throw std::runtime_error(
					"getAttributes failed with error: " + ec.message());
			}

			const bool bIsDirectory = dwAttributes & FILE_ATTRIBUTE_DIRECTORY;
			if (bFile && !bIsDirectory)
			{
				/** добавление к списку файла */
				listPath.push_back(v->getPath().lexically_proximate(proximatePath));
			}
			else if (bIsDirectory)
			{
				const auto& directory = dynamic_cast<CDirectory&>(*v);

				/** добавление вложенных данных */
				auto subListPath = directory.getSubFilesystemObject(
					bFile, bDirectory, proximatePath);
				std::move(subListPath.begin(), subListPath.end(), listPath.end());

				/** добавление к списку папки */
				if (bDirectory)
					listPath.push_back(v->getPath().lexically_proximate(proximatePath));
			}		
		}

		return listPath;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code CDirectory::deleteDirectory(
	const std::filesystem::path& directoryPath) noexcept
{
	const auto bResult = RemoveDirectory(directoryPath.c_str());

	return std::error_code(
		bResult ? ERROR_SUCCESS : GetLastError(),
		std::system_category());
}
//------------------------------------------------------------------------------
void CDirectory::close() noexcept
{
	const auto lock = lockGuard();

	/** сброс кэша */
	resetCache();

	closeHandle();

	_listFileSystemObject.clear();
	_nCountSubFile = 0;
	_nCountSubDirectory = 0;
}
//------------------------------------------------------------------------------
void CDirectory::release(
	const bool bIsWait) noexcept
{
	__super::release(false);
	close();	
	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//------------------------------------------------------------------------------
std::error_code CDirectory::enumDirectory()
{
	const auto lock = lockGuard();

	if (!isInitialize())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		WIN32_FIND_DATA ffd = {0};
		std::wstring directoryPath = getPath();
		std::wstring pathFind = directoryPath;
		pathFind.append(L"\\*");
		auto hFind = FindFirstFile(pathFind.c_str(), &ffd);
		
		if (handle::CHandle::isValid(hFind))
		{
			do
			{
				try
				{		
					std::wstring path = directoryPath;
					path.append(L"\\");
					path.append(&ffd.cFileName[0]);
					path = std::filesystem::path(path).wstring();

					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (_wcsicmp(&ffd.cFileName[0], L".") == 0 ||
							_wcsicmp(&ffd.cFileName[0], L"..") == 0)
						{
							/** эти элементы пропускаем,
								иначе отсканируем всю систему
								еще и упремс¤ в рекурсию */
							continue;
						}
						else if (ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
						{
							/** системна¤ папка, не добавл¤ем в список */
							continue;
						}
					}
					
					/** нашелс¤ объект */
					if (const auto ec = addFileSystemObject(ffd); ec)
					{
						FindClose(hFind);
						return ec;
					}
				}
				catch (const std::exception&)
				{
					FindClose(hFind);
					throw;
				}

			} while (FindNextFile(hFind, &ffd));

			FindClose(hFind);
		}

		return {};
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code CDirectory::addFileSystemObject(
	const WIN32_FIND_DATA& ffd)
{
	const auto lock = lockGuard();

	if (!isInitialize())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::wstring path = 
			getPath().
			wstring().
			append(L"\\").
			append(&ffd.cFileName[0]);

		std::shared_ptr<CFileSystemObject> pFileSystemObject;
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			auto pDirectory = std::make_shared<CDirectory>(
				path,
				_pIocp,
				*this);

			if (_isSubOpenDirectory)
			{
				/** установлен признак открыти¤ вложенных объектов */
				if (const auto ec = pDirectory->createDirectory(
					true,
					_dwDesiredAccess,
					_dwShareMode,
					OPEN_EXISTING,
					_dwFlagsAndAttributes); ec)
					return ec;
			}

			pFileSystemObject = std::move(pDirectory);
		}
		else
		{
			pFileSystemObject = std::make_shared<CFile>(
				path, _pIocp);
		}

		BY_HANDLE_FILE_INFORMATION info = {};
		if (const auto ec = pFileSystemObject->getCache(info); ec)
		{
			/** кэша нет */
			info.dwFileAttributes = ffd.dwFileAttributes;
			info.ftCreationTime = ffd.ftCreationTime;
			info.ftLastAccessTime = ffd.ftLastAccessTime;
			info.ftLastWriteTime = ffd.ftLastWriteTime;
			info.nFileSizeHigh = ffd.nFileSizeHigh;
			info.nFileSizeLow = ffd.nFileSizeLow;

			/** обновление кэша */
			pFileSystemObject->updateCache(info);
		}
			 
		_listFileSystemObject[path] = std::move(pFileSystemObject);
		changeSubCount(1, pFileSystemObject->_eFileSystemObjectType);

		return {};
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
void CDirectory::changeSubCount(
	const int countItem, 
	const EFileSystemObjectType eFileSystemObjectType) noexcept
{
	switch (eFileSystemObjectType)
	{
	case EFileSystemObjectType::eFile:
		_nCountSubFile += countItem;
		break;
	case EFileSystemObjectType::eDirectory:
		_nCountSubDirectory += countItem;
		break;
	default:
		break;
	}
	
	if (&_parent != this)
		_parent.changeSubCount(countItem, eFileSystemObjectType);
}
//------------------------------------------------------------------------------
CDirectory::~CDirectory()
{
	release(true);
}
//------------------------------------------------------------------------------