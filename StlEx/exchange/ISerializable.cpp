#include "../stdafx.h"

using ISerializable = stlex::exchange::ISerializable;

/**
* 26481 - логика указателей.
* 26493 - привидегния в стиле C.
*/
#pragma warning (push)
#pragma warning (disable: 26481 26493)

//------------------------------------------------------------------------------
void ISerializable::serialize(
	std::vector<BYTE>& buffer,
	const std::filesystem::path& data)
{
	/** сериализуем как строку */
	std::wstring str = data;
	serialize(buffer, str);
}
//------------------------------------------------------------------------------
size_t ISerializable::deserialize(
	const std::vector<BYTE>& buffer,
	const size_t offset,
	std::filesystem::path& data)
{
	/** десериализуем как строку */
	std::wstring str;

	const auto dwReturnedBytes = deserialize(buffer, offset, str);
	data = str;

	return dwReturnedBytes;
}
//------------------------------------------------------------------------------
void ISerializable::serialize(
	std::vector<BYTE>& buffer,
	const std::error_code& data)
{
	/** сериализуем как значение от ошибки */
	serialize(buffer, data.value());
}
//------------------------------------------------------------------------------
size_t ISerializable::deserialize(
	const std::vector<BYTE>& buffer,
	const size_t offset,
	std::error_code& data)
{
	/** десериализуем как значение от ошибки */
	auto value = data.value();
	const auto dwReturnedBytes = deserialize(buffer, offset, value);
	data = std::error_code(value, std::system_category());

	return dwReturnedBytes;
}
//------------------------------------------------------------------------------
#pragma warning (pop)