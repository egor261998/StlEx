#pragma once

namespace stlex::logger
{
	/** информация о логе */
	class STLEX CLogInfo final
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор информации о логе.
		* @param szFunction - имя функции из которой был вызван лог.
		* @param liLine - строка вызова лога.
		* @param szFile - имя файла из которого был вызван лог
		*/
		CLogInfo(
			const char* szFunction = __builtin_FUNCTION(),
			const int iLine = __builtin_LINE(),
			const char* szFile = __builtin_FILE()) noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CLogInfo() = default;
	//--------------------------------------------------------------------------
		CLogInfo(const CLogInfo&) noexcept = default;
		CLogInfo(CLogInfo&&) noexcept = default;
		CLogInfo& operator=(const CLogInfo&) noexcept = default;
		CLogInfo& operator=(CLogInfo&&) noexcept = default;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Data
	public:
	//--------------------------------------------------------------------------
		/** имя функции из которой был вызван лог */
		const char* _szFunction;

		/** имя файла из которого был вызван лог */
		const char* _szFile;

		/** строка вызова лога */
		const int _liLine;	
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}