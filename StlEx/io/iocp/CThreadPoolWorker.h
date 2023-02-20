#pragma once

namespace stlex::io::iocp
{
	/** нить пула нитей для механизма ввода/вывода */
	class STLEX CIocp::CThreadPoolWorker final : 
		public misc::CCounter
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/*
		* конструктор нити.
		* @param threadPool - пул нитей.
		*/
		CThreadPoolWorker(
			CThreadPool& threadPool);
	//--------------------------------------------------------------------------
		/**
		* запустить новую нить.
		* @param threadPool - пул нитей.
		*/
		static void start(
			CThreadPool& threadPool);
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
		~CThreadPoolWorker();
	//--------------------------------------------------------------------------
		CThreadPoolWorker(const CThreadPoolWorker&) = delete;
		CThreadPoolWorker(CThreadPoolWorker&&) = delete;
		CThreadPoolWorker& operator=(const CThreadPoolWorker&) = delete;
		CThreadPoolWorker& operator=(CThreadPoolWorker&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Data
	public:
	//--------------------------------------------------------------------------
		/** пул нитей */
		CThreadPool& _threadPool;

		/** нить */
		std::thread _thread;

		/** признак использования нити */
		bool _bIsBusy = false;
		/** признак высвобождения нити */
		bool _bIsFree = false;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}