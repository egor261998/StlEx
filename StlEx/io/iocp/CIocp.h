#pragma once

namespace stlex::io::iocp
{
	/** реализация механизма Iocp */
	class STLEX CIocp final :
		public misc::CCounter
	{
	#pragma region Public_Inner
	public:
	//--------------------------------------------------------------------------
		/** асинхронная операция */
		class CAsyncOperation;
	//--------------------------------------------------------------------------
		/**
		* прототип функции завершения операции.
		* @param asyncOperation - асинхронная операция.
		*/
		typedef void (*FAsyncCompletion)(
			const CAsyncOperation& asyncOperation) noexcept;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Inner
	private:
	//--------------------------------------------------------------------------
		class CThreadPool;
		class CThreadPoolWorker;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* инициализация Iocp.
		* @param pLogger - объект логирования.
		*/
		CIocp(
			const std::shared_ptr<logger::ILogger>& pLogger) noexcept(false);
	//--------------------------------------------------------------------------
		/**
		* инициализация Iocp.
		* @param minThreadCount - минимальное кол-во нитей в пуле.
		* @param maxThreadCount - максимальное кол-во нитей в пуле.
		* @param pLogger - объект логирования.
		*/
		CIocp(
			const DWORD minThreadCount = 1,
			const DWORD maxThreadCount = INFINITE,
			const std::shared_ptr<logger::ILogger>& pLogger = nullptr) noexcept(false);
	//--------------------------------------------------------------------------
		/**
		* установить объект логирования.
		* @param pLogger - объект логирования.
		* @return - результат работы функции.
		*/
		std::error_code setLogger(
			const std::shared_ptr<logger::ILogger>& pLogger) noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить объект логирования.
		* @return - объект логирования.
		*/
		std::shared_ptr<logger::ILogger> getLogger() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* привязать дескриптор к порту ввода/вывода.
		* @param hHandle - привязываемый дескриптор.
		* @param ulCompletionKey - ключ завершения.
		* @return - результат работы функции.
		*/
		std::error_code bind(
			const HANDLE hHandle,
			const ULONG_PTR ulCompletionKey = 0) noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить асинхронную операцию.
		* если в метод был передан счетчик асинхронных операций,
		* то произойдет попытка старта новой асинхронной операции. 
		* по завершению работы функции обратного вызова, ссылка будет сброшена.
		* 
		* @param pCompletionRoutineContext - контекст асинхронной операции.
		* @param fCompletionRoutine - функция завершения асинхронной операции.
		* @param pCounter - счетчик асинхронных операций.
		* @return - асинхронная операция.
		*/
		CAsyncOperation& getAsyncOperation(
			const PVOID pCompletionRoutineContext,
			const FAsyncCompletion fCompletionRoutine,
			CCounter* pCounter = nullptr);
	//--------------------------------------------------------------------------
		/**
		* вызвать транзитный вызов через порт ввода/вывода.
		* если в метод был передан счетчик асинхронных операций,
		* то произойдет попытка старта новой асинхронной операции.
		* по завершению работы функции обратного вызова, ссылка будет сброшена.
		* 
		* @param pCompletionRoutineContext - контекст асинхронной операции.
		* @param fCompletionRoutine - функция завершения асинхронной операции.
		* @param ulCompletionKey - ключ заверения.
		* @param pCounter - счетчик асинхронных операций.
		* @return - результат работы функции.
		*/
		std::error_code transit(
			const PVOID pCompletionRoutineContext,
			const FAsyncCompletion fCompletionRoutine,
			const ULONG_PTR ulCompletionKey = 0,
			CCounter* pCounter = nullptr);
	//--------------------------------------------------------------------------
		/*
		* лог сообщения.
		* @param eMessageType - тип сообщения.
		* @param message - сообщение.
		* @param ec - код ошибки.
		* @param logInfo - информация о логе.
		* @return - результат записи.
		*/
		template<class T>
		std::error_code log(
			const logger::ILogger::EMessageType eMessageType,
			const T& message,
			const std::error_code& ec = {},
			const logger::CLogInfo& logInfo = logger::CLogInfo()) noexcept
		{
			const auto lock = lockGuard();

			return _pLogger->log(eMessageType, message, ec, logInfo);
		}
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
		~CIocp();
	//--------------------------------------------------------------------------
		CIocp(const CIocp&) = delete;
		CIocp(CIocp&&) = delete;
		CIocp& operator=(const CIocp&) = delete;
		CIocp& operator=(CIocp&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Method
	private:
	//--------------------------------------------------------------------------
		/**
		* обработчик асинхронных операций текущего порта ввода/вывода.
		* @param workerThread - объект нити обработчика.
		*/
		void workerThread(
			CThreadPoolWorker& workerThread) noexcept;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** время ожидания нагрузки на нить в (мс),
			после этого времени она сама удалится */
		static constexpr DWORD _nWorkerTimeIde = 10000;

		/** зарезервированное максимальное кол-во свободных асинхронных операций,
			присутствующих в системе */
		static constexpr DWORD _nMaxReservAsyncOperation = 100;

		/** порт ввода/вывода */
		handle::CHandle _hIocp;

		/** пул нитей */
		std::unique_ptr<CThreadPool> _pThreadPool;

		/** TLS для текущего механизма ввода/вывода */
		misc::CThreadTls<CThreadPoolWorker> _tls;

		/** список асинхронных операций */
		misc::CInterlockedList<CAsyncOperation> _listFreeAsyncOperation;

		/** объект логирования */
		std::shared_ptr<logger::ILogger> _pLogger;
	//--------------------------------------------------------------------------
	#pragma endregion
	};

	using CAsyncOperation = CIocp::CAsyncOperation;
}