#pragma once

namespace stlex::exchange
{
	/** буфер обмена данными */
	class STLEX CBufferExchange final
	{
	#pragma region Public_Inner
	public:
	//--------------------------------------------------------------------------
		/** тип буфера */
		enum class EType
		{
			eNone,
			eWrite,
			eRead
		};
	//--------------------------------------------------------------------------
		/** состояние буфера обмена данных */
		enum class EState
		{
			eNone,
			eHeadPening,
			eDataPending,
			eReady
		};
	//--------------------------------------------------------------------------
		/** шапка буфера */
		struct SHead
		{
			/** тип сообщения */
			DWORD dwMessageType = 0;

			/** размер данных буфера */
			DWORD dwSizeData = 0;
		};
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		*/
		CBufferExchange() noexcept(false) = default;
	//--------------------------------------------------------------------------
		/**
		* уложить данные в уже уложенную шапку.
		* @param buffer - данные.
		*/
		void putData(
			const std::vector<BYTE>& buffer);
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CBufferExchange() = default;
	//--------------------------------------------------------------------------
		CBufferExchange(const CBufferExchange&) = default;
		CBufferExchange(CBufferExchange&&) = default;
		CBufferExchange& operator=(const CBufferExchange&) = default;
		CBufferExchange& operator=(CBufferExchange&&) = default;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Data
	public:
	//--------------------------------------------------------------------------
		/** контекст на данные */
		PVOID _pContext = nullptr;

		/** шапка данных*/
		SHead _sHead;

		/** данные буфера обмена данными */
		std::vector<BYTE> _pBuffer;

		/** состояние буфера обмена данных */
		EState _eState = EState::eNone;

		/** тип буфера */
		EType _eType = EType::eNone;

		/** событие завершения операции */
		handle::CEvent _ev;	

		/** ошибка после завершения операции */
		std::error_code _ec;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}