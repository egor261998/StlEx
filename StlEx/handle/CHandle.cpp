#include "../stdafx.h"

using CHandle = stlex::handle::CHandle;

//------------------------------------------------------------------------------
CHandle::CHandle(
	const HANDLE hObject)
{
	/** выполняем команду через перегрузку оператора присвоения */
	*this = hObject;
}
//------------------------------------------------------------------------------
bool CHandle::isValid() const noexcept
{
	return isValid(getHandle());
}
//------------------------------------------------------------------------------
bool CHandle::isValid(
	const HANDLE hObject) noexcept
{
	return (hObject != INVALID_HANDLE_VALUE && hObject != nullptr);
}
//------------------------------------------------------------------------------
HANDLE CHandle::getHandle() const noexcept
{
	return _pHandle.get();
}
//------------------------------------------------------------------------------
std::error_code CHandle::wait(
	const DWORD dwMilliseconds,
	const bool bAlertable) const noexcept
{
	return std::error_code(
		WaitForSingleObjectEx(getHandle(), dwMilliseconds, bAlertable),
		std::system_category());
}
//------------------------------------------------------------------------------
std::error_code CHandle::waitSignal() const noexcept
{
	while (true)
	{
		switch (const auto ecWait = wait(INFINITE, true); ecWait.value())
		{
		case WAIT_IO_COMPLETION:
		case WAIT_TIMEOUT:
			/** заходим на новое ожидание */
			break;
		default:
			return ecWait;
		}
	}
}
//------------------------------------------------------------------------------
void CHandle::close() noexcept
{
	_pHandle.reset();
}
//------------------------------------------------------------------------------
CHandle& CHandle::operator=(
	const HANDLE hObject)
{
	/** закрываем предыдущий описатель */
	close();

	if (isValid(hObject))
	{
		/** описатель валидный */
		_pHandle = std::shared_ptr<void>(hObject, &CloseHandle);
		return *this;
	}

	return *this;
}
//------------------------------------------------------------------------------
CHandle::operator HANDLE() const noexcept
{
	return getHandle();
}
//------------------------------------------------------------------------------
CHandle::~CHandle()
{
	close();
}
//------------------------------------------------------------------------------