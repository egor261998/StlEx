#include "../stdafx.h"

using CLibrary = stlex::handle::CLibrary;

//------------------------------------------------------------------------------
std::error_code CLibrary::load(
	const std::filesystem::path& path)
{
	/** загружаем библиотеку */
	const auto hObject = LoadLibrary(path.c_str());

	try
	{
		/** закрываем предыдущий описатель */
		close();

		if (isValid(hObject))
		{
			/** описатель валидный */
			_pHandle = std::shared_ptr<void>(hObject, &FreeLibrary);
			return {};
		}

		return std::error_code(GetLastError(), std::system_category());
	}
	catch (const std::exception&)
	{
		FreeLibrary(hObject);
		throw;
	}
}
//------------------------------------------------------------------------------
std::error_code CLibrary::getProcAddress(
	const std::string_view name,
	PVOID& address) const noexcept
{
	address = GetProcAddress(static_cast<HMODULE>(getHandle()), name.data());
	if (address == nullptr)
	{
		return std::error_code(GetLastError(), std::system_category());
	}

	return {};
}
//------------------------------------------------------------------------------
CLibrary::~CLibrary()
{
	close();
}
//------------------------------------------------------------------------------