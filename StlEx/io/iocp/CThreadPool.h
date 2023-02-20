#pragma once

namespace stlex::io::iocp
{
	/** пул нитей для механизма ввода/вывода */
	class STLEX CIocp::CThreadPool final : 
		public misc::CCounter
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* инициализация пула нитей.
		* @param iocp - родительный объект.
		* @param minThreadCount - минимальное кол-во нитей в пуле.
		* @param maxThreadCount - максимальное кол-во нитей в пуле.
		*/
		CThreadPool(
			CIocp& iocp,
			const DWORD minThreadCount,
			const DWORD maxThreadCount);
	//--------------------------------------------------------------------------
		/**
		* высвободить память из под нитки.
		* @param threadPoolWorker - нить пула.
		*/
		void delWorker(
			const CThreadPoolWorker& threadPoolWorker) noexcept;
	//--------------------------------------------------------------------------
		/**
		* попытка пометить нитку для удаления.
		* @param threadPoolWorker - нить пула.
		* @return - успех операции.
		*/
		bool canFreeWorker(
			CThreadPoolWorker& threadPoolWorker) noexcept;
	//--------------------------------------------------------------------------
		/**
		* попытка добавить новую нитку.
		* @param bAddAnyway - добавить нить в любом случае.
		* @return - успех операции.
		*/
		bool canAddWorker(
			bool bAddAnyway = false);
	//--------------------------------------------------------------------------
		/**
		* снизить занятость пула.
		*/
		void decrementBusyWorker() noexcept;
	//--------------------------------------------------------------------------
		/**
		* закончить работу и дождаться всех асинхронных операций.
		* @param bIsWait - признак ожидания.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CThreadPool();
	//--------------------------------------------------------------------------
		CThreadPool(const CThreadPool&) = delete;
		CThreadPool(CThreadPool&&) = delete;
		CThreadPool& operator=(const CThreadPool&) = delete;
		CThreadPool& operator=(CThreadPool&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	public:
	//--------------------------------------------------------------------------
		/** родительный объект */
		CIocp& _iocp;

		/** минимальное кол-во нитей */
		DWORD _nMinThreadCount = 0;
		/** максимальное кол-во нитей */
		DWORD _nMaxThreadCount = 0;
		
		/** используемое кол-во нитей */
		DWORD _nBusyThreadCount = 0;
		/** высвобождаемое кол-во нитей */
		DWORD _nFreeThreadCount = 0;

		/** текущее кол-во нитей */
		std::unordered_map<
			const CThreadPoolWorker*, std::shared_ptr<CThreadPoolWorker>> _listWorker;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}