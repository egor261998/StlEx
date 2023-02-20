#pragma once

namespace stlex::cs
{
	/** блокировка критической секции */
	class STLEX CCriticalSectionLockGuard final
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		* @param cs - критическая секция.
		*/
		CCriticalSectionLockGuard(
			CCriticalSection& cs) noexcept;
	//--------------------------------------------------------------------------
		/**
		* освободить объект.
		*/
		void free() noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CCriticalSectionLockGuard();
	//--------------------------------------------------------------------------
		CCriticalSectionLockGuard(const CCriticalSectionLockGuard&) = delete;
		CCriticalSectionLockGuard(CCriticalSectionLockGuard&&) = delete;
		CCriticalSectionLockGuard& operator=(const CCriticalSectionLockGuard&) = delete;
		CCriticalSectionLockGuard& operator=(CCriticalSectionLockGuard&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** используемая критическая секция */
		CCriticalSection& _cs;

		/** признак блокировки */
		bool _bIsLock = true;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}