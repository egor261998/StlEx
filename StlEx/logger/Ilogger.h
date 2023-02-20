#pragma once

namespace stlex::logger
{
	/** интерфейс логирования */
	class STLEX ILogger
	{
		
	#pragma region Public_Inner
	public:
	//--------------------------------------------------------------------------
		/** типы сообщения */
		enum class EMessageType : UINT8
		{
			trace,
			warning,
			critical
		};
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		*/
		ILogger() noexcept = default;
	//--------------------------------------------------------------------------
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param ex - исключение.
		* @param ec - код ошибки.
		* @param logInfo - информация о логе.
		* @return - результат записи.
		*/
		std::error_code log(
			const EMessageType eMessageType,
			const std::exception& ex,
			const std::error_code& ec = {},
			const CLogInfo& logInfo = {}) noexcept;
	//--------------------------------------------------------------------------
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param message - сообщение.
		* @param ec - код ошибки.
		* @param logInfo - информация о логе.
		* @return - результат записи.
		*/
		std::error_code log(
			const EMessageType eMessageType,
			const std::string_view message,
			const std::error_code& ec = {},
			const CLogInfo& logInfo = {}) noexcept;
	//--------------------------------------------------------------------------
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param message - сообщение.
		* @param ec - код ошибки.
		* @param logInfo - информация о логе.
		* @return - результат записи.
		*/
		std::error_code log(
			const EMessageType eMessageType,
			const std::wstring_view message,
			const std::error_code& ec = {},
			const CLogInfo& logInfo = {}) noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		virtual ~ILogger() = default;
	//--------------------------------------------------------------------------
		ILogger(const ILogger&) = delete;
		ILogger(ILogger&&) = delete;
		ILogger& operator=(const ILogger&) = delete;
		ILogger& operator=(ILogger&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//--------------------------------------------------------------------------
		/*
		* сборка сообщения.
		* @param eMessageType - тип сообщения.
		* @param message - сообщение.
		* @param ec - код ошибки.
		* @param logInfo - информация о логе.
		* @return - сообщение для записи в лог.
		*/
		virtual std::wstring assembleMessage(
			const EMessageType eMessageType,
			const std::wstring_view message,
			const std::error_code& ec,
			const CLogInfo& logInfo);
	//--------------------------------------------------------------------------
		/**
		* запись сообщения в лог.
		* @param eMessageType - тип сообщения.
		* @param message - сообщение для записи в лог.
		* @return - результат записи.
		*/
		virtual std::error_code logWrite(
			const EMessageType eMessageType,
			const std::wstring& message) = 0;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}