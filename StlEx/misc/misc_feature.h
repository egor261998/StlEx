#pragma once

namespace stlex::misc
{
//------------------------------------------------------------------------------
	/** размеры буферов */
	enum EBufferSize : DWORD
	{
		/** Б */
		e1B = 1 << 0,
		e2B = e1B << 1,
		e4B = e2B << 1,
		e8B = e4B << 1,
		e16B = e8B << 1,
		e32B = e16B << 1,
		e64B = e32B << 1,
		e128B = e64B << 1,
		e256B = e128B << 1,
		e512B = e256B << 1,
		e1024B = e512B << 1,

		/** КБ */
		e1K = e1024B,
		e2K = e1K << 1,
		e4K = e2K << 1,
		e8K = e4K << 1,
		e16K = e8K << 1,
		e32K = e16K << 1,
		e64K = e32K << 1,
		e128K = e64K << 1,
		e256K = e128K << 1,
		e512K = e256K << 1,
		e1024K = e512K << 1,

		/** МБ */
		e1M = e1024K,
		e2M = e1M << 1,
		e4M = e2M << 1,
		e8M = e4M << 1,
		e16M = e8M << 1,
		e32M = e16M << 1,
		e64M = e32M << 1,
		e128M = e64M << 1,
		e256M = e128M << 1,
		e512M = e256M << 1,
		e1024M = e512M << 1,

		/** ГБ */
		e1G = e1024M,
	};
//------------------------------------------------------------------------------
	/** типы путей */
	enum class EPathType
	{
		driver,
		file_or_directory,
		npipe,
		gpipe
	};
//------------------------------------------------------------------------------
	/**
	* определить тип пути.
	* @param path - путь.
	* @return - тип пути.
	*/
	EPathType STLEX pathType(
		const std::filesystem::path& path) noexcept;
//------------------------------------------------------------------------------
	/**
	* форматирование строки в список строк.
	* @param str - строка.
	* @return - список строк.
	*/
	std::list<std::wstring> STLEX toListStr(
		const std::wstring_view str);
//------------------------------------------------------------------------------
	/**
	* форматирование размера.
	* @param nValue - значение в байтах.
	* @param bIsSpeed - признак скорости в секундах.
	* @param bIsFullInformation - признак вывода полной информации о размере 
		в байтах в уточняющих скобках.
	* @param shift - смещение отображаемых знаков.
	* @return - форматированная строка.
	*/
	std::wstring STLEX formatByte(
		const UINT64 nValue,
		const bool bIsSpeed = false,
		const bool bIsFullInformation = false,
		const UINT shift = 10000);
//------------------------------------------------------------------------------
	/**
	* форматирование времени.
	* @param systemTime - системное время.
	* @param bIsToLocal - признак перевода времени в локальный часовой пояс.
	* @return - форматированная строка.
	*/
	std::wstring STLEX formatTime(
		const SYSTEMTIME systemTime,
		const bool bIsToLocal = true);
//------------------------------------------------------------------------------
	/**
	* форматирование времени.
	* @param fileTime - файловое время.
	* @param bIsToLocal - признак перевода времени в локальный часовой пояс.
	* @return - форматированная строка.
	*/
	std::wstring STLEX formatTime(
		const FILETIME fileTime,
		const bool bIsToLocal = true);
//------------------------------------------------------------------------------
	/**
	* преобразование кодировки для строки.
	* @param str - строка.
	* @return - строка другой кодировки.
	*/
	std::wstring STLEX convertStr(
		const std::string_view str);
//------------------------------------------------------------------------------
	/**
	* преобразование кодировки для строки.
	* @param wstr - строка.
	* @return - строка другой кодировки.
	*/
	std::string STLEX convertStr(
		const std::wstring_view wstr);
//------------------------------------------------------------------------------
}