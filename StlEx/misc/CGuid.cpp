#include "../stdafx.h"

using CGuid = stlex::misc::CGuid;

//------------------------------------------------------------------------------
std::wstring CGuid::toString(
	const EFormatType eFormatType) const
{
	WCHAR wszGuid[MAX_PATH] = { };

	const auto nSize = StringFromGUID2(
		_guid, &wszGuid[0], ARRAYSIZE(wszGuid) - 1);
	assert(nSize != 0);

	std::wstring guid(&wszGuid[0]);
	switch (eFormatType)
	{
	case EFormatType::eNaked:
	{
		while (true)
		{
			if (const auto index = guid.find_first_of(L"{}");
				index != std::wstring::npos)
			{
				guid.erase(index, 1);
				continue;
			}

			break;
		}

		break;
	}
	default:
		break;
	}

	return guid;
}
//------------------------------------------------------------------------------
bool CGuid::createGuid() noexcept
{
	return CoCreateGuid(&_guid) == S_OK;
}
//------------------------------------------------------------------------------
CGuid::operator GUID() const noexcept
{
	return _guid;
}
//------------------------------------------------------------------------------