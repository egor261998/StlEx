#pragma once

namespace stlex::logger 
{
	/** лог в консоль */
	class STLEX CLoggerToConsole : 
		public ILogger
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/*
		* конструктор.
		*/
		CLoggerToConsole() noexcept = default;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CLoggerToConsole() = default;
	//--------------------------------------------------------------------------
		CLoggerToConsole(const CLoggerToConsole&) = delete;
		CLoggerToConsole(CLoggerToConsole&&) = delete;
		CLoggerToConsole& operator=(const CLoggerToConsole&) = delete;
		CLoggerToConsole& operator=(CLoggerToConsole&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//--------------------------------------------------------------------------
		/**
		* логировать сообщение.
		* @param eMessageType - тип сообщения.
		* @param message - логируемое сообщение.
		* @return - результат записи.
		*/
		std::error_code logWrite(
			const EMessageType eMessageType,
			const std::wstring& message) noexcept override;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}