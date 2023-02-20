#pragma once

namespace stlex::filesystem
{
	/** работа с директорией */
	class STLEX CDirectory : 
		public CFileSystemObject
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор папки.
		* @param directoryPath - абсолютное путь до папки.
		* @param pIocp - механизм ввода/вывода.
		*/
		CDirectory(
			const std::filesystem::path& directoryPath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp = nullptr);
	//--------------------------------------------------------------------------
		/**
		* конструктор папки в случае подпапки.
		* @param directoryPath - абсолютное путь до папки.
		* @param pIocp - механизм ввода/вывода.
		* @param parent - родительный класс.
		*/
		CDirectory(
			const std::filesystem::path& directoryPath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp,
			CDirectory& parent);
	//--------------------------------------------------------------------------
		/**
		* открыть/создать папку.
		* @param isSubOpenDirectory - открыть все вложенные папки.
		* @param dwDesiredAccess - запрошенный доступ к папки.
		* ( GENERIC_READ | GENERIC_WRITE).
		* @param dwShareMode - параметр общего доступа к папки.
		* (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE).
		* @param dwCreationDisposition - действие над папкой.
		* (CREATE_NEW | CREATE_ALWAYS | OPEN_EXISTING | OPEN_ALWAYS | TRUNCATE_EXISTING).
		* @param dwFlagsAndAttributes - атрибуты и флаги.
		* FILE_FLAG_BACKUP_SEMANTICS и FILE_FLAG_OVERLAPPED всегда присутствует.
		* @return - код ошибки.
		*/
		std::error_code createDirectory(
			const bool isSubOpenDirectory = false,
			const DWORD dwDesiredAccess = 0,
			const DWORD dwShareMode = 0,
			const DWORD dwCreationDisposition = 0,
			const DWORD dwFlagsAndAttributes = 0);
	//--------------------------------------------------------------------------
		/**
		* получить список путей.
		* @param bFile - включить в список файлы.
		* @param bDirectory - включить с список папки.
		* @param proximatePath - обрезать начальный путь по.
		* @return - список файлов.
		*/
		std::list<std::filesystem::path> getSubFilesystemObject(
			const bool bSubFile,
			const bool bSubDirectory,
			const std::filesystem::path& proximatePath = "") const;
	//--------------------------------------------------------------------------
		/**
		* закрыть папку.
		*/
		void close() noexcept;
	//--------------------------------------------------------------------------
		/**
		* удалить папку.
		* @param directoryPath - путь до папки.
		* @return - код ошибки.
		*/
		static std::error_code deleteDirectory(
			const std::filesystem::path& directoryPath) noexcept;
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
		~CDirectory();
	//--------------------------------------------------------------------------
		CDirectory(const CDirectory&) = delete;
		CDirectory(CDirectory&&) = delete;
		CDirectory& operator=(const CDirectory&) = delete;
		CDirectory& operator=(CDirectory&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Method
	private:
	//--------------------------------------------------------------------------
		/**
		* найти все элементы.
		* @return - код ошибки.
		*/
		std::error_code enumDirectory();
	//--------------------------------------------------------------------------
		/**
		* добавление объекта в список.
		* @param ffd - информация об объекте.
		* @return - результат добавления.
		*/
		std::error_code addFileSystemObject(
			const WIN32_FIND_DATA& ffd);
	//--------------------------------------------------------------------------
		/**
		* изменение количества вложенных элементов.
		* @param countItem - количество элементов.
		* @param eFileSystemObjectType - тип объекта для элементов.
		*/
		void changeSubCount(
			const int countItem,
			const EFileSystemObjectType eFileSystemObjectType) noexcept;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** родительный объект */
		CDirectory& _parent;

		/** список вложенных элементов */
		std::unordered_map<
			std::wstring, 
			std::shared_ptr<CFileSystemObject>> _listFileSystemObject;

		/** признак открытия вложенных папок */
		bool _isSubOpenDirectory = false;

		/** запрошенный доступ к папки */
		DWORD _dwDesiredAccess = 0;
		/** параметр общего доступа к папки. */
		DWORD _dwShareMode = 0;
		/** dwFlagsAndAttributes */
		DWORD _dwFlagsAndAttributes = 0;

		/** количество входящих элементов */
		std::atomic_uint64_t _nCountSubFile;
		std::atomic_uint64_t _nCountSubDirectory;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}