#pragma once

namespace stlex::misc
{
	/** блокировка счетчика операций */
	class STLEX CCounterScoped final
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор.
		* @param counter - счетчик операций.
		* @param nCount - количество операций.
		*/
		CCounterScoped(
			CCounter& counter,
			const size_t nCount = 1) noexcept;
	//--------------------------------------------------------------------------
		/**
		* успех старта счетчика.
		* @return - успех старта операций.
		*/
		bool isStartOperation() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* освободить.
		* @param bIsEndOperation - признак снятия ссылок.
		*/
		void release(
			const bool bIsEndOperation = false) noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CCounterScoped();
	//--------------------------------------------------------------------------
		CCounterScoped(const CCounterScoped&) = delete;
		CCounterScoped(CCounterScoped&&) = delete;
		CCounterScoped& operator=(const CCounterScoped&) = delete;
		CCounterScoped& operator=(CCounterScoped&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** используемый счетчик */
		CCounter& _counter;

		/** количество успешных операций счетчика */
		size_t _nCountStart = 0;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}