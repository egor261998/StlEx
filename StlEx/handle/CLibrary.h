#pragma once

namespace stlex::handle
{
	/** объект динамической библиотеки */
	class STLEX CLibrary final : 
		public CHandle
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		*/
		CLibrary() noexcept = default;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CLibrary();
	//--------------------------------------------------------------------------
		CLibrary(const CLibrary&) noexcept = default;
		CLibrary(CLibrary&&) noexcept = default;
		CLibrary& operator=(const CLibrary&) = default;
		CLibrary& operator=(CLibrary&&) noexcept = default;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//--------------------------------------------------------------------------
		/**
		* загрузка динамической библиотеки.
		* @param path - путь до библиотеки.
		* @return - результат работы.
		*/
		std::error_code load(
			const std::filesystem::path& path);
	//--------------------------------------------------------------------------
		/**
		* получить адрес по имени.
		* @param name - имя.
		* @param address - адрес.
		* @return - результат работы.
		*/
		template<typename _Ty>
		std::error_code getProcAddress(
			const std::string_view name,
			_Ty& address) const noexcept
		{
		/**
		* 26493 - приведение типов.
		*/
		#pragma warning (disable: 26493)
			return getProcAddress(name, *(PVOID*)&address);
		#pragma warning (default: 26493)
		}		
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Method
	private:
	//--------------------------------------------------------------------------
		/**
		* получить адрес по имени.
		* @param name - имя.
		* @param address - адрес.
		* @return - результат работы.
		*/
		std::error_code getProcAddress(
			const std::string_view name,
			PVOID& address) const noexcept;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}