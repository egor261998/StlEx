#pragma once

namespace stlex::filesystem
{
	/** работа с файлом */
	class STLEX CFile : 
		public CFileSystemObject
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор файла.
		* @param filePath - путь до файла.
		* @param pIocp - механизм ввода/вывода.
		*/
		CFile(
			const std::filesystem::path& filePath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp = nullptr);
	//--------------------------------------------------------------------------
		/**
		* создать или открыть файл.
		* @param dwDesiredAccess - запрошенный доступ к файлу.
		* ( GENERIC_READ | GENERIC_WRITE).
		* @param dwShareMode - параметр общего доступа к файлу.
		* (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE).
		* @param dwCreationDisposition - действие над файлом.
		* (CREATE_NEW | CREATE_ALWAYS | OPEN_EXISTING | OPEN_ALWAYS | TRUNCATE_EXISTING).
		* @param dwFlagsAndAttributes - атрибуты и флаги.
		* FILE_FLAG_OVERLAPPED всегда присутствует.
		* @return - код ошибки.
		*/
		std::error_code createFile(
			const DWORD dwDesiredAccess = 0,
			const DWORD dwShareMode = 0,
			const DWORD dwCreationDisposition = 0,
			const DWORD dwFlagsAndAttributes = 0);
	//--------------------------------------------------------------------------
		/**
		* получить размер файла.
		* @param uSize - размер файла.
		* @param bIsCache - попробовать получить из кэша.
		* @return - код ошибки.
		*/
		std::error_code getFileSize(
			UINT64& uSize,
			const bool bIsCache = false) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить размер файла и сохранить в кэш.
		* @param info - информация.
		* @param bIsCache - попробовать получить из кэша.
		* @return - код ошибки.
		*/
		std::error_code getFileSize(
			UINT64& uSize,
			const bool bIsCache = false) noexcept;
	//--------------------------------------------------------------------------
		/**
		* удалить файл.
		* @return - код ошибки.
		*/
		std::error_code deleteFile() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* очистить файл.
		* @return - код ошибки.
		*/
		std::error_code clearFile() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* закрыть файл.
		*/
		void close() noexcept;
	//--------------------------------------------------------------------------
		/**
		* удалить файл.
		* @param filePath - путь до файла.
		* @return - код ошибки.
		*/
		static std::error_code deleteFile(
			const std::filesystem::path& filePath) noexcept;
	//--------------------------------------------------------------------------
		/**
		* закончить работу.
		* @param bIsWait - признак ожидания.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CFile();
	//--------------------------------------------------------------------------
		CFile(const CFile&) = delete;
		CFile(CFile&&) = delete;
		CFile& operator=(const CFile&) = delete;
		CFile& operator=(CFile&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}