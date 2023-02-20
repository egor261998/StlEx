#include "../stdafx.h"

using IPipe = stlex::pipe::IPipe;

//------------------------------------------------------------------------------
IPipe::IPipe(
	const std::filesystem::path& pipePath,
	const std::shared_ptr<io::iocp::CIocp>& pIocp) :
	IAsyncIo(pIocp),
	CPathStorage(pipePath)
{
	
}
//------------------------------------------------------------------------------
void IPipe::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);
}
//------------------------------------------------------------------------------
IPipe::~IPipe()
{
	release(true);
}
//------------------------------------------------------------------------------