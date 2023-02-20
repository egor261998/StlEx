#include "../stdafx.h"

using CProcess = stlex::handle::CProcess;

//------------------------------------------------------------------------------
std::error_code CProcess::create(
	const std::filesystem::path& processPath,
	const std::wstring& commandLine,
	const DWORD dwCreationFlags)
{
	return create(
		processPath, 
		commandLine, 
		std::filesystem::current_path(),
		dwCreationFlags);
}
//------------------------------------------------------------------------------
std::error_code CProcess::create(
	const std::filesystem::path& processPath,
	const std::wstring& commandLine,
	const std::filesystem::path& currentDirectoryPath,
	const DWORD dwCreationFlags)
{
	/** закрываем описатели */
	close();

	STARTUPINFO si = {};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {};
	if (const auto ec = CreateProcess(
		processPath.c_str(),
		(LPWSTR)commandLine.c_str(),
		nullptr,
		nullptr,
		false,
		dwCreationFlags,
		nullptr,
		currentDirectoryPath.c_str(),
		&si,
		&pi) ?
		std::error_code() :
		std::error_code(GetLastError(), std::system_category()); ec)
	{
		/** процесс не запустился */
		return ec;
	}

	try
	{
		_hThread = pi.hThread;
		_hProcess = pi.hProcess;
	}
	catch (const std::exception&)
	{
		const auto dwError = GetLastError();
		TerminateProcess(pi.hProcess, dwError);
		if (_hProcess.isValid())
			_hProcess.close();
		else
			CloseHandle(pi.hProcess);

		if (_hThread.isValid())
			_hThread.close();
		else
			CloseHandle(pi.hThread);
		return std::error_code(dwError, std::system_category());
	}

	return {};
}
//------------------------------------------------------------------------------
std::error_code CProcess::waitProcess() const noexcept
{
	/** ожидание процесса */
	return _hProcess.waitSignal();
}
//------------------------------------------------------------------------------
std::error_code CProcess::getExitCode(
	std::error_code& ec) const noexcept
{
	DWORD dwExitCode = ERROR_SUCCESS;
	if (!GetExitCodeProcess(_hProcess, &dwExitCode))
		return std::error_code(GetLastError(), std::system_category());

	ec = std::error_code(dwExitCode, std::system_category());
	return {};
}
//------------------------------------------------------------------------------
std::error_code CProcess::kill(
	const std::error_code& ec) const noexcept
{
	const auto ecTerminate = TerminateProcess(_hProcess, ec.value()) ?
		std::error_code() : std::error_code(GetLastError(), std::system_category());

	return ecTerminate;
}
//------------------------------------------------------------------------------
bool CProcess::isValid() const noexcept
{
	return _hThread.isValid() && _hProcess.isValid();
}
//------------------------------------------------------------------------------
void CProcess::close() noexcept
{
	_hProcess.close();
	_hThread.close();
}
//------------------------------------------------------------------------------
CProcess::~CProcess()
{
	close();
}
//------------------------------------------------------------------------------