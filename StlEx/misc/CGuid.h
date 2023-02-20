#pragma once

namespace stlex::misc
{
	/** объект для работы с GUID */
	class STLEX CGuid final
	{
	#pragma region Public_Inner
	//--------------------------------------------------------------------------
		/** тип формата */
		enum class EFormatType
		{
			/** имеет вид XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX */
			eNaked,

			/** имеет вид {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} */
			eRegistry,	
		};
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		*/
		CGuid() noexcept = default;
	//--------------------------------------------------------------------------
		/**
		* создания GUID.
		* @return - результат работы функции.
		*/
		bool createGuid() noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить как строку.
		* @param eFormatType - тип формата.
		* @return - строка.
		*/
		std::wstring toString(
			const EFormatType eFormatType = EFormatType::eNaked) const;
	//--------------------------------------------------------------------------
		/**
		* оператор получения.
		* @return - GUID.
		*/
		operator GUID() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CGuid() = default;
	//--------------------------------------------------------------------------
		CGuid(const CGuid&) noexcept = default;
		CGuid(CGuid&&) noexcept = default;
		CGuid& operator=(const CGuid&) noexcept = default;
		CGuid& operator=(CGuid&&) noexcept = default;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** GUID */
		GUID _guid = {};
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}