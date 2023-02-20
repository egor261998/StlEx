#pragma once

namespace stlex::logger 
{
	/** лог в файл */
	class STLEX CLoggerToFile : 
		public ILogger
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/*
		* конструктор.
		* @param logPath - путь до файла.
		*/
		CLoggerToFile(
			const std::filesystem::path& logPath);
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CLoggerToFile() = default;
	//--------------------------------------------------------------------------
		CLoggerToFile(const CLoggerToFile&) = delete;
		CLoggerToFile(CLoggerToFile&&) = delete;
		CLoggerToFile& operator=(const CLoggerToFile&) = delete;
		CLoggerToFile& operator=(CLoggerToFile&&) = delete;	
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

	#pragma region Private_Data
	//--------------------------------------------------------------------------
	private:
		/** путь до файла */
		const std::filesystem::path _logPath;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}