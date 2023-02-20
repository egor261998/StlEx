#include "../stdafx.h"

using CLogInfo = stlex::logger::CLogInfo;
using ILogger = stlex::logger::ILogger;

//------------------------------------------------------------------------------
std::error_code ILogger::log(
	const EMessageType eMessageType,
	const std::exception& ex,
	const std::error_code& ec,
	const CLogInfo& logInfo) noexcept
{
	try
	{
		return log(eMessageType, ex.what(), ec, logInfo);
	}
	catch (const std::exception&){}	

	return std::error_code(ERROR_WRITE_FAULT, std::system_category());
}
//------------------------------------------------------------------------------
std::error_code ILogger::log(
	const EMessageType eMessageType,
	const std::string_view message,
	const std::error_code& ec,
	const CLogInfo& logInfo) noexcept
{
	try
	{
		const auto ret = std::wstring(message.begin(), message.end());

		return log(eMessageType, ret, ec, logInfo);
	}
	catch (const std::exception&) {}

	return std::error_code(ERROR_WRITE_FAULT, std::system_category());
}
//------------------------------------------------------------------------------
std::error_code ILogger::log(
	const EMessageType eMessageType,
	const std::wstring_view message,
	const std::error_code& ec,
	const CLogInfo& logInfo) noexcept
{
	try
	{
		/** сборка сообщения */
		const auto ret = assembleMessage(
			eMessageType, message, ec, logInfo);

		/** логирование */
		return logWrite(eMessageType, ret);
	}
	catch (const std::exception&){}
	
	return std::error_code(ERROR_WRITE_FAULT, std::system_category());
}
//------------------------------------------------------------------------------
std::wstring ILogger::assembleMessage(
	const EMessageType eMessageType,
	const std::wstring_view message,
	const std::error_code& ec,
	const CLogInfo& logInfo)
{
	SYSTEMTIME sysTime = {};
	GetSystemTime(&sysTime);
	std::wstring ret = L"\n\nTime: " + misc::formatTime(sysTime);

	ret.append(L"\nType: ");
	switch (eMessageType)
	{
	case EMessageType::trace:
		ret.append(L"trace");
		break;
	case EMessageType::warning:
		ret.append(L"warning");
		break;
	case EMessageType::critical:
		ret.append(L"critical");
		break;
	default:
		ret.append(L"Unknown");
		break;
	}

	/** преобразование строк */
	std::string strFunction(logInfo._szFunction);
	std::wstring wStrFunction(strFunction.begin(), strFunction.end());
	std::string strFile(logInfo._szFile);
	std::wstring wStrFile(strFile.begin(), strFile.end());

	/** сборка сообщения */
	ret.append(L"\nMessage: ").append(message);
	ret.append(L"\nError code: " + std::to_wstring(ec.value()));
	ret.append(L"\nMessage Function: " + wStrFunction);
	ret.append(L"\nLog line: " + std::to_wstring(logInfo._liLine));
	ret.append(L"\nLog file: " + wStrFile);

	return ret;
}
//------------------------------------------------------------------------------