#include "../stdafx.h"

using CFile = stlex::filesystem::CFile;

//------------------------------------------------------------------------------
CFile::CFile(
	const std::filesystem::path& filePath,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) :
	CFileSystemObject(filePath, EFileSystemObjectType::eFile, pIocp)
{
	
}
//------------------------------------------------------------------------------
std::error_code CFile::createFile(
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

		/** на случай если файл уже открыт */
		if (getState() != EIoState::close)
			return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());

		changeState(EIoState::openning);
	}

	try
	{
		const auto& filePath = getPath();

		/** счетчик проходов */
		size_t nCount = 0;
		while (nCount < MAXBYTE)
		{
			/** попытка создать файл с ходу */
			handle::CHandle hHandle = CreateFile(
				filePath.c_str(),
				dwDesiredAccess,
				dwShareMode,
				NULL,
				dwCreationDisposition,
				dwFlagsAndAttributes | FILE_FLAG_OVERLAPPED,
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
					/** файла или папки нет */
					std::error_code ec;
					const auto parentPath = filePath.parent_path();
					std::filesystem::create_directories(parentPath, ec);
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

				/** попытка создать файл еще раз */
				nCount++;
				continue;
			}

			/** инициализаци¤ асинхронных операций */
			if (const auto ec = bindHandle(hHandle); ec)
			{
				changeState(EIoState::close);
				return ec;
			}

			break;
		}

		return {};
	}
	catch (const std::exception& ex)
	{
		changeState(EIoState::close);
		_pIocp->log(logger::ILogger::EMessageType::critical, ex);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code CFile::getFileSize(
	UINT64& uSize,
	const bool bIsCache) const noexcept
{
	/** получаем информацию о файле */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache))
		return ec;

	LARGE_INTEGER li = {};
	li.LowPart = info.nFileSizeLow;
	li.HighPart = info.nFileSizeHigh;
	uSize = li.QuadPart;
	return {};
}
//------------------------------------------------------------------------------
std::error_code CFile::getFileSize(
	UINT64& uSize,
	const bool bIsCache) noexcept
{
	/** получаем информацию о файле */
	BY_HANDLE_FILE_INFORMATION info = {};
	if (const auto ec = getInfo(info, bIsCache))
		return ec;

	LARGE_INTEGER li = {};
	li.LowPart = info.nFileSizeLow;
	li.HighPart = info.nFileSizeHigh;
	uSize = li.QuadPart;
	return {};
}
//------------------------------------------------------------------------------
std::error_code CFile::deleteFile() const noexcept
{
	/** устанавливаем флаг удалени¤ дл¤ файла */
	FILE_DISPOSITION_INFO FileDispInfo = { true };
	if (!SetFileInformationByHandle(
		getHandle(),
		FileDispositionInfo,
		&FileDispInfo,
		sizeof(FileDispInfo)))
		return std::error_code(GetLastError(), std::system_category());

	return {};
}
//------------------------------------------------------------------------------
std::error_code CFile::clearFile() const noexcept
{
	DWORD dwResult = ERROR_SUCCESS;
	const auto hHandle = getHandle();

	if (SetFilePointer(hHandle, 0, NULL, FILE_BEGIN) != 0)
	{
		dwResult = GetLastError();
	}
	else
	{
		if (!SetEndOfFile(hHandle))
			dwResult = GetLastError();
	}

	return std::error_code(dwResult, std::system_category());
}
//------------------------------------------------------------------------------
void CFile::close() noexcept
{
	/** сброс кэша */
	resetCache();

	closeHandle();
}
//------------------------------------------------------------------------------
std::error_code CFile::deleteFile(
	const std::filesystem::path& filePath) noexcept
{
	const auto bResult = DeleteFile(filePath.c_str());

	return std::error_code(
		bResult ? ERROR_SUCCESS : GetLastError(), 
		std::system_category());
}
//------------------------------------------------------------------------------
void CFile::release(
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
CFile::~CFile()
{
	release(true);
}
//------------------------------------------------------------------------------