#pragma once

namespace stlex::cs
{
	/** реализация критических секций в классе */
	class STLEX CCriticalSection final :
		private CRITICAL_SECTION
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор критической секции.
		*/
		CCriticalSection() noexcept;
	//--------------------------------------------------------------------------
		/**
		* блокировка критической секции.
		*/
		 void lock() noexcept;
	//--------------------------------------------------------------------------
		/**
		* попытка блокировки критической секции.
		* @return - успех блокировки.
		*/
		bool tryLock() noexcept;
	//--------------------------------------------------------------------------
		/**
		* разблокировка критической секции.
		*/
		void unLock() noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор критической секции.
		*/
		~CCriticalSection();
	//--------------------------------------------------------------------------
		CCriticalSection(const CCriticalSection&) = delete;
		CCriticalSection(CCriticalSection&&) = delete;
		CCriticalSection& operator=(const CCriticalSection&) = delete;
		CCriticalSection& operator=(CCriticalSection&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}