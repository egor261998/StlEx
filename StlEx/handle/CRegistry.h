#pragma once

namespace stlex::handle
{
	/** объект для работы с ключем реестром */
	class STLEX CRegistry final :
		public CHandle
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		*/
		CRegistry() noexcept = default;
	//--------------------------------------------------------------------------
		/**
		* конструктор ключа.
		* @param hKey - базовый ключ.
		* @param nameSubKey - имя создаваемого ключа.
		* @param amDesiredAccess - флаги доступа.
		* (KEY_READ | KEY_WRITE | KEY_EXECUTE | KEY_ALL_ACCESS)
		* @param dwOptions - флаги вызова.
		*/
		CRegistry(
			const HKEY hKey,
			const std::wstring& nameSubKey,
			const REGSAM amDesiredAccess = KEY_ALL_ACCESS,
			const DWORD dwOptions = 0);
	//--------------------------------------------------------------------------
		/**
		* создать ключ.
		* @return - результат работы функции.
		*/
		std::error_code createKey();
	//--------------------------------------------------------------------------
		/**
		* открыть ключ.
		* @return - результат работы функции.
		*/
		std::error_code openKey();
	//--------------------------------------------------------------------------
		/**
		* обновление информации.
		* @param bIsUpdateSubKey - обновление подключей.
		* @return - результат работы функции.
		*/
		std::error_code updateInfo(
			const bool bIsUpdateSubKey);
	//--------------------------------------------------------------------------
		/**
		* получить значение по имени.
		* @param nameValue - имя значения.
		* @param dwValue - значение.
		* @return - результат работы функции.
		*/
		std::error_code queryValue(
			const std::wstring& nameValue,
			DWORD& dwValue) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* установить значение по имени.
		* @param nameValue - имя значения.
		* @param dwValue - значение.
		* @return - результат работы функции.
		*/
		std::error_code setValue(
			const std::wstring& nameValue,
			const DWORD dwValue) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить значение по имени.
		* @param nameValue - имя значения.
		* @param stringValue - значение.
		* @return - результат работы функции.
		*/
		std::error_code queryValue(
			const std::wstring& nameValue,
			std::wstring& stringValue) const;
	//--------------------------------------------------------------------------
		/**
		* установить значение по имени.
		* @param nameValue - имя значения.
		* @param stringValue - значение.
		* @return - результат работы функции.
		*/
		std::error_code setValue(
			const std::wstring& nameValue,
			const std::wstring& stringValue) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* удаление значения по имени.
		* @param nameValue - имя значения.
		* @return - результат работы функции.
		*/
		std::error_code deleteValue(
			const std::wstring& nameValue) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* удаление подключа.
		* @param nameSubKey - имя удаляемого ключа.
		* @param amDesiredAccess - флаги доступа.
		* (KEY_READ | KEY_WRITE | KEY_EXECUTE | KEY_ALL_ACCESS)
		* @return - результат работы функции.
		*/
		std::error_code deleteSubKey(
			const std::wstring& nameSubKey,
			const REGSAM amDesiredAccess) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CRegistry();
	//--------------------------------------------------------------------------
		CRegistry(const CRegistry&) = default;
		CRegistry(CRegistry&&) noexcept = default;
		CRegistry& operator=(const CRegistry&) = default;
		CRegistry& operator=(CRegistry&&) noexcept = default;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Method
	private:
	//--------------------------------------------------------------------------
		/**
		* получить информацию по ключу.
		* @param dwSubKeys - количество подключей.
		* @param dwMaxSubKeyLength - максимальная длина имени подключа.
		* @param dwValues - количество значений ключа.
		* @param dwMaxValueNameLength - максимальная длина имени значения ключа.
		* @param lastWriteTime - последнее время доступа.
		* @return - результат работы функции.
		*/
		std::error_code queryInfo(
			DWORD& dwSubKeys,
			DWORD& dwMaxSubKeyLength,
			DWORD& dwValues,
			DWORD& dwMaxValueNameLength,
			FILETIME& lastWriteTime) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* перечисление подключей по индексу.
		* @param dwSubKeyIndex - индекс подключа.
		* @param nameSubKey - имя подключа.
		* @param lastWriteTime - последнее время доступа.
		* @return - результат работы функции.
		*/
		std::error_code enumSubKey(
			const DWORD dwSubKeyIndex,
			std::wstring& nameSubKey,
			FILETIME& lastWriteTime) const;
	//--------------------------------------------------------------------------
		/**
		* перечисление значений по индексу.
		* @param dwSubKeyIndex - индекс значения.
		* @param nameValue - имя значения.
		* @param dwValueType - тип данных значения.
		* @return - результат работы функции.
		*/
		std::error_code enumValue(
			const DWORD dwSubKeyIndex,
			std::wstring& nameValue,
			DWORD& dwValueType) const;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Data
	public:
	//--------------------------------------------------------------------------
		/** базовый ключ */
		HKEY _hKey;

		/** доступ ключа */
		REGSAM _amDesiredAccess = {};

		/** опции ключа */
		DWORD _dwOptions = 0;

		/** имя создаваемого ключа */
		std::wstring _nameSubKey;

		/** список под ключей */
		std::list<CRegistry> _keyList;

		/** список значений, пара состоит из имени значения и его типа */
		std::list<std::pair<std::wstring, DWORD>> _valueList;

		/** время последнего доступа */
		FILETIME _lastWriteTime = {};
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}