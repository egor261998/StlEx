#pragma once

namespace stlex::filesystem
{
	/** работа с объектом файловой системы */
	class STLEX CFileSystemObject : 
		public io::IAsyncIo,
		public misc::CPathStorage
	{
		friend class CDirectory;
	#pragma region Public_Inner
	public:
	//--------------------------------------------------------------------------
		/** тип объекта файловой системы */
		enum class EFileSystemObjectType
		{
			eFile, 
			eDirectory
		};
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* получить информацию об объекте файловой системы.
		* @param info - информация.
		* @param bIsCache - попробовать получить из кэша.
		* @return - код ошибки.
		*/
		std::error_code getInfo(
			BY_HANDLE_FILE_INFORMATION& info,
			const bool bIsCache = false) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить информацию об объекте файловой системы и сохранить в кэш.
		* @param info - информация.
		* @param bIsCache - попробовать получить из кэша.
		* @return - код ошибки.
		*/
		std::error_code getInfo(
			BY_HANDLE_FILE_INFORMATION& info,
			const bool bIsCache = false) noexcept;
	//--------------------------------------------------------------------------
		/**
		* установить информацию об объекте файловой системы.
		* @param info - информация.
		* @param bIsCache - обновление кэша.
		* @return - код ошибки.
		*/
		std::error_code setInfo(
			const FILE_BASIC_INFO& info,
			const bool bIsCache = false) noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить атрибуты.
		* @param dwAttributes - атрибуты.
		* @param bIsCache - попробовать получить из кэша.
		* @return - код ошибки.
		*/
		std::error_code getAttributes(
			DWORD& dwAttributes,
			const bool bIsCache = false) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить атрибуты и сохранить в кэш.
		* @param dwAttributes - атрибуты.
		* @param bIsCache - попробовать получить из кэша.
		* @return - код ошибки.
		*/
		std::error_code getAttributes(
			DWORD& dwAttributes,
			const bool bIsCache = false) noexcept;
	//--------------------------------------------------------------------------
		/**
		* установить атрибуты.
		* @param dwAttributes - атрибуты.
		* @param bIsCache - обновление кэша.
		* @return - код ошибки.
		*/
		std::error_code setAttributes(
			const DWORD dwAttributes,
			const bool bIsCache = false) noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить атрибуты времени.
		* @param ftCreationTime - время создания.
		* @param ftLastAccessTime - время последнего доступа.
		* @param ftLastWriteTime - время последней записи.
		* @param bIsCache - попробовать получить из кэша.
		* @return - код ошибки.
		*/
		std::error_code getTimeAttributes(
			FILETIME& ftCreationTime,
			FILETIME& ftLastAccessTime,
			FILETIME& ftLastWriteTime,
			const bool bIsCache = false) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить атрибуты времени и сохранить в кэш.
		* @param ftCreationTime - время создания.
		* @param ftLastAccessTime - время последнего доступа.
		* @param ftLastWriteTime - время последней записи.
		* @param bIsCache - попробовать получить из кэша.
		* @return - код ошибки.
		*/
		std::error_code getTimeAttributes(
			FILETIME& ftCreationTime,
			FILETIME& ftLastAccessTime,
			FILETIME& ftLastWriteTime,
			const bool bIsCache = false) noexcept;
	//--------------------------------------------------------------------------
		/**
		* установить атрибуты времени.
		* @param ftCreationTime - время создания.
		* @param ftLastAccessTime - время последнего доступа.
		* @param ftLastWriteTime - время последней записи.
		* @param bIsCache - обновление кэша.
		* @return - код ошибки.
		*/
		std::error_code setTimeAttributes(
			const FILETIME& ftCreationTime,
			const FILETIME& ftLastAccessTime,
			const FILETIME& ftLastWriteTime,
			const bool bIsCache = false) noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить все атрибуты.
		* @param dwAttributes - атрибуты.
		* @param ftCreationTime - время создания.
		* @param ftLastAccessTime - время последнего доступа.
		* @param ftLastWriteTime - время последней записи.
		* @param bIsCache - попробовать получить из кэша.
		* @return - код ошибки.
		*/
		std::error_code getAllAttributes(
			DWORD& dwAttributes,
			FILETIME& ftCreationTime,
			FILETIME& ftLastAccessTime,
			FILETIME& ftLastWriteTime,
			const bool bIsCache = false) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить все атрибуты и сохранить в кэш.
		* @param dwAttributes - атрибуты.
		* @param ftCreationTime - время создания.
		* @param ftLastAccessTime - время последнего доступа.
		* @param ftLastWriteTime - время последней записи.
		* @param bIsCache - попробовать получить из кэша.
		* @return - код ошибки.
		*/
		std::error_code getAllAttributes(
			DWORD& dwAttributes,
			FILETIME& ftCreationTime,
			FILETIME& ftLastAccessTime,
			FILETIME& ftLastWriteTime,
			const bool bIsCache = false) noexcept;
	//--------------------------------------------------------------------------
		/**
		* установить все атрибуты.
		* @param dwAttributes - атрибуты.
		* @param ftCreationTime - время создания.
		* @param ftLastAccessTime - время последнего доступа.
		* @param ftLastWriteTime - время последней записи.
		* @param bIsCache - обновление кэша.
		* @return - код ошибки.
		*/
		std::error_code setAllAttributes(
			const DWORD dwAttributes,
			const FILETIME& ftCreationTime,
			const FILETIME& ftLastAccessTime,
			const FILETIME& ftLastWriteTime,
			const bool bIsCache = false) noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить информацию об объекте файловой системы.
		* @param fileSystemPath - путь до объекта файловой системы.
		* @param info - информация.
		* @return - код ошибки.
		*/
		static std::error_code getInfo(
			const std::filesystem::path& fileSystemPath,
			BY_HANDLE_FILE_INFORMATION& info);
	//--------------------------------------------------------------------------
		/**
		* установить информацию об объекте файловой системы.
		* @param fileSystemPath - путь до объекта файловой системы.
		* @param info - информация.
		* @return - код ошибки.
		*/
		static std::error_code setInfo(
			const std::filesystem::path& fileSystemPath,
			const FILE_BASIC_INFO& info);
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
		~CFileSystemObject();
	//--------------------------------------------------------------------------
		CFileSystemObject(const CFileSystemObject&) = delete;
		CFileSystemObject(CFileSystemObject&&) = delete;
		CFileSystemObject& operator=(const CFileSystemObject&) = delete;
		CFileSystemObject& operator=(CFileSystemObject&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//--------------------------------------------------------------------------
		/**
		* конструктор объекта файловой системы.
		* @param fileSystemPath - путь до объекта.
		* @param eFileSystemObjectType - тип объекта.
		* @param pIocp - механизм ввода/вывода.
		*/
		CFileSystemObject(
			const std::filesystem::path& fileSystemPath,
			const EFileSystemObjectType eFileSystemObjectType,
			const std::shared_ptr<io::iocp::CIocp>& pIocp = nullptr);
	//--------------------------------------------------------------------------
		/**
		* обновление кэша.
		* @param info - информация.
		*/
		void updateCache(
			const BY_HANDLE_FILE_INFORMATION& info) noexcept;
	//--------------------------------------------------------------------------
		/**
		* получение кэша.
		* @param info - информация.
		* @return - код ошибки.
		*/
		std::error_code getCache(
			BY_HANDLE_FILE_INFORMATION& info) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* сбросить кэш. предыдущее значение кэша более не будет действительным.
		*/
		void resetCache() noexcept;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** кэшированные данные */
		BY_HANDLE_FILE_INFORMATION _infoCahce = {};

		/** признак ранее сохраненных данных */
		bool _bIsCache = false;

		/** тип объекта */
		const EFileSystemObjectType _eFileSystemObjectType;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}