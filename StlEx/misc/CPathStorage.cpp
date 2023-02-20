#include "../stdafx.h"

using CPathStorage = stlex::misc::CPathStorage;

//------------------------------------------------------------------------------
CPathStorage::CPathStorage(
	const std::filesystem::path& path) :
	_path(path.lexically_normal())
{

}
//------------------------------------------------------------------------------
const std::filesystem::path& CPathStorage::getPath() const noexcept
{
	return _path;
}
//------------------------------------------------------------------------------