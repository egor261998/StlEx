#include "../stdafx.h"

using stlex::handle::CEvent;

//------------------------------------------------------------------------------
CEvent::CEvent(
	const bool bIsCreateEvent) noexcept(false) :
	CEvent(std::wstring_view(), bIsCreateEvent)
{
	
}
//------------------------------------------------------------------------------
CEvent::CEvent(
	const std::wstring_view name,
	const bool bIsCreateEvent)
{
	if (!bIsCreateEvent)
		return;

	/** создаем событие */
	const auto hObject = CreateEvent(NULL, FALSE, FALSE, name.data());

	try
	{
		CHandle::operator=(hObject);
	}
	catch (const std::exception&)
	{
		CloseHandle(hObject);
		throw;
	}

	if (!isValid())
	{
		throw std::runtime_error(
			"CreateEvent failed with error: " + std::to_string(GetLastError()));
	}
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CEvent::CEvent(
	const CHandle& handle) noexcept :
	CHandle(handle)
{
	
}
//------------------------------------------------------------------------------
CEvent::CEvent(
	CHandle&& handle) noexcept :
	CHandle(std::move(handle))
{

}
//------------------------------------------------------------------------------
std::error_code CEvent::notify() const noexcept
{
	return std::error_code(
		SetEvent(getHandle()) ? ERROR_SUCCESS : GetLastError(),
		std::system_category());
}
//------------------------------------------------------------------------------
std::error_code CEvent::cancel() const noexcept
{
	return std::error_code(
		ResetEvent(getHandle()) ? ERROR_SUCCESS : GetLastError(),
		std::system_category());
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CEvent& CEvent::operator=(
	const CHandle& handle) noexcept
{
	CHandle::operator=(handle);
	return *this;
}
//------------------------------------------------------------------------------
CEvent& CEvent::operator=(
	CHandle&& handle) noexcept
{
	CHandle::operator=(std::move(handle));
	return *this;
}
//------------------------------------------------------------------------------
CEvent::~CEvent()
{
	close();
}
//------------------------------------------------------------------------------