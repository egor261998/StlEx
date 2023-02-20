#pragma once

namespace stlex::misc
{
	/** счетчик операций */
	class STLEX CCounter
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор.
		*/
		CCounter() noexcept(false) = default;
	//--------------------------------------------------------------------------
		/**
		* блокировка объекта.
		* @return - объект блокировки.
		*/
		cs::CCriticalSectionLockGuard lockGuard() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* инициализирован ли счетчик операций.
		* @return - признак инициализации.
		*/
		bool isInitialize() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* старт новых операции.
		* @param nCount - количество операций.
		* @return - успех старта операций.
		*/
		bool startOperation(
			const size_t nCount = 1) noexcept;
	//--------------------------------------------------------------------------
		/**
		* окончание операций.
		* @param nCount - количество операций.
		* @return - возможность завершени¤.
		*/
		bool endOperation(
			const size_t nCount = 1) noexcept;
	//--------------------------------------------------------------------------
		/**
		* проверка количества операций.
		* @param nCount - смещение ссылок.
		* @return - TRUE если операций в обработке больше чем указано в параметре.
		*/
		bool checkOperation(
			const size_t nCount = 0) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* ожидание операций.
		* @param nCount - смещение ссылок.
		*/
		void waitOperation(
			const size_t nCount = 0) const;
	//--------------------------------------------------------------------------
		/**
		* закончить работу со счетчиком операций.
		* @param bIsWait - признак ожидани¤.
		*/
		virtual void release(
			const bool bIsWait) noexcept;
	//--------------------------------------------------------------------------
		/**
		* удаление объекта после того как отработает последн¤¤ операци¤.
		* @return - признак того что объект удалилс¤.
		*/
		bool deleteAfterEndOperation() noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		virtual ~CCounter();
	//--------------------------------------------------------------------------
		CCounter(const CCounter&) = delete;
		CCounter(CCounter&&) = delete;
		CCounter& operator=(const CCounter&) = delete;
		CCounter& operator=(CCounter&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Method
	private:
	//--------------------------------------------------------------------------		
		/**
		* обработка операции.
		* @param bResultFree - признак завершени¤.
		* @param isDelete - признак нужно ли удал¤ть объект.
		*/
		void processingOperation(
			bool& bResultFree,
			bool& isDelete) noexcept;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------	
		/** произвольное ожидание операции */
		mutable std::map<size_t, std::list<handle::CEvent>> _counterWait;

		/** синхронизаци¤ счетчика */
		mutable cs::CCriticalSection _csCounter;

		/** количество выполн¤емых асинхронных операций */
		size_t _nCounterCount = 0;

		/** событие завершени¤ последней операции */
		handle::CEvent _eventCounterFree;

		/** состо¤ние */
		bool _isCounterInitialize = true;

		/** признак удалени¤ объекта после завершени¤ последней операции */
		bool _isDeleteAfterEndOperation = false;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}