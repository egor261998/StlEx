#pragma once

namespace stlex::io::iocp
{
	/** асинхронная операция механизма ввода/вывода */
	class STLEX CIocp::CAsyncOperation final : 
		protected misc::CInterlockedItem
	{
		friend class CIocp;
		friend class misc::CInterlockedList<CAsyncOperation>;

	#pragma region Public_Inner
	//--------------------------------------------------------------------------
		/** буфер ввода/вывода */
		struct SBuffer
		{
			/** размер буфера */
			DWORD _dwSize;
			/** буфер который используется в операции */
			union
			{
				const BYTE* _pConst;
				BYTE* _p;
			};			
		};
	//--------------------------------------------------------------------------

	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор асинхронной операции.
		* @param pIocp - указатель на порт ввода/вывода.
		* @param pCompletionRoutineContext - контекст асинхронной операции.
		* @param fCompletionRoutine - функция завершения асинхронной операции.
		*/
		CAsyncOperation(
			CIocp* pIocp,
			const PVOID pCompletionRoutineContext = nullptr,
			const FAsyncCompletion fCompletionRoutine = nullptr);
	//--------------------------------------------------------------------------
		/**
		* успех старта счетчика.
		* @return - успех старта операций.
		*/
		bool isStartOperation() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* обновить счетчик асинхронных операций и попытка старта новой операции.
		* @param pCounter - счетчик операций.
		* @return - успех старта операций.
		*/
		bool updateCounter(
			CCounter* pCounter = nullptr) noexcept;
	//--------------------------------------------------------------------------
		/**
		* отмена асинхронной операции.
		*/
		void cancel() noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CAsyncOperation() = default;
	//--------------------------------------------------------------------------
		CAsyncOperation(const CAsyncOperation&) = delete;
		CAsyncOperation(CAsyncOperation&&) = delete;
		CAsyncOperation& operator=(const CAsyncOperation&) = delete;
		CAsyncOperation& operator=(CAsyncOperation&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Data
	public:
	//--------------------------------------------------------------------------
		/** указатель на порт ввода/вывода */
		CIocp* _pIocp;

		/** ключ завершения */
		ULONG_PTR _ulCompletionKey = 0;

		/** асинхронный контекст */
		OVERLAPPED _overlapped = { 0 };

		union
		{
			/** буфер для сокетов */
			//WSABUF _wsaBuffer{ 0 };

			/** буфер для всех */
			SBuffer _buffer;
		};	
		
		/** возвращенных байт из асинхронной операции */
		DWORD _dwReturnedBytes = 0;
		/** код ошибки */
		std::error_code _ec;

		/** контекст функции завершения */
		PVOID _pCompletionRoutineContext;
		/** функция завершения */
		FAsyncCompletion _fCompletionRoutine;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** нить вызова, если она из пула */
		CThreadPoolWorker* _pThreadPoolWorker = nullptr;

		/** счетчик асинхронных операций */
		bool _isStartOperation = false;
		CCounter* _pCounter = nullptr;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}