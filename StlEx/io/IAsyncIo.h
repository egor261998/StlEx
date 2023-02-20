#pragma once

namespace stlex::io
{
	/** интерфейс асинхронного ввода/вывода */
	class STLEX IAsyncIo : 
		virtual public misc::CCounter
	{
	#pragma region Protected_Inner
	protected:
	//--------------------------------------------------------------------------
		/** состояние асинхронного ввода/вывода */
		enum class EIoState
		{
			openning,
			open,
			closing,
			close
		};
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Inner
	private:
	//--------------------------------------------------------------------------
		/** завершение синхронного запроса */
		struct SSyncIoComplete
		{
			/** событие завершения запроса */
			handle::CEvent& _ev;

			/** возращенный размер */
			DWORD& _dwReturnedBytes;

			/** код завершения */
			std::error_code& _ec;
		};
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор.
		* @param pIocp - механизм ввода/вывода.
		*/
		IAsyncIo(
			const std::shared_ptr<iocp::CIocp>& pIocp = nullptr) noexcept(false);
	//--------------------------------------------------------------------------
		/**
		* открыт ли объект.
		* @return - успех операции.
		*/
		bool isOpen() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* старт асинхронного чтения.
		* @param bufferRead - буфер для чтения.
		* @param bufferSize - размер буфера для чтения.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		virtual std::error_code startAsyncRead(
			BYTE* bufferRead,
			const DWORD dwBufferSize,
			const UINT64 offset = 0);
	//--------------------------------------------------------------------------
		/**
		* старт чтения.
		* @param bufferRead - буфер для чтения.
		* @param bufferSize - размер буфера для чтения.
		* @param dwReturnedBytes - количество прочитанных байт.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		virtual std::error_code startRead(
			BYTE* bufferRead,
			const DWORD dwBufferSize,
			DWORD& dwReturnedBytes,
			const UINT64 offset = 0);
	//--------------------------------------------------------------------------
		/**
		* старт асинхронной записи.
		* @param bufferWrite - буфер для записи.
		* @param bufferSize - размер буфера для записи.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		virtual std::error_code startAsyncWrite(
			const BYTE* bufferWrite,
			const DWORD dwBufferSize,
			const UINT64 offset = 0);
	//--------------------------------------------------------------------------
		/**
		* старт записи.
		* @param bufferWrite - буфер для записи.
		* @param bufferSize - размер буфера для записи.
		* @param dwReturnedBytes - количество записанных байт.
		* @param offset - офсет относительно начала.
		* @return - код ошибки.
		*/
		virtual std::error_code startWrite(
			const BYTE* bufferWrite,
			const DWORD dwBufferSize,
			DWORD& dwReturnedBytes,
			const UINT64 offset = 0);
	//--------------------------------------------------------------------------
		/**
		* дождаться завершения операций ввода/вывода.
		* @return - код ошибки.
		*/
		virtual std::error_code flush() const noexcept;
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
		~IAsyncIo();
	//--------------------------------------------------------------------------
		IAsyncIo(const IAsyncIo&) = delete;
		IAsyncIo(IAsyncIo&&) = delete;
		IAsyncIo& operator=(const IAsyncIo&) = delete;
		IAsyncIo& operator=(IAsyncIo&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//--------------------------------------------------------------------------
		/**
		* установить описатель.
		* @param handle - описатель.
		*/
		void setHandle(
			const handle::CHandle& handle) noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить описатель.
		* @return - описатель.
		*/
		HANDLE getHandle() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* инициализация асинхронной операции ввода/вывода.
		* @param handle - описатель.
		* @return - результат работы.
		*/
		std::error_code bindHandle(
			const handle::CHandle& handle) noexcept;
	//--------------------------------------------------------------------------
		/**
		* проверка наличия привязанного описателя.
		* @return - результат проверки.
		*/
		bool isBindHandle() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* изменить состояние.
		* @param eIoState - новое состояние.
		*/
		void changeState(
			const EIoState eIoState) noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить состояние.
		* @return -  состояние.
		*/
		EIoState getState() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* изменить описатель.
		* @param handle - описатель.
		*/
		void changeHandle(
			const handle::CHandle& handle);
	//--------------------------------------------------------------------------
		/**
		* закрыть описатель.
		*/
		void closeHandle() noexcept;
	//--------------------------------------------------------------------------
		/**
		* виртуальный обработчик события открытия.
		*/
		virtual void openHandler() noexcept;
	//--------------------------------------------------------------------------
		/**
		* виртуальный обработчик события завершения асинхронного чтения.
		* @param bufferRead - буфер данных.
		* @param dwReturnedBytes - количество прочитанных байт.
		* @param ec - код ошибки завершения.
		*/
		virtual void asyncReadCompletionHandler(
			BYTE* bufferRead,
			const DWORD dwReturnedBytes,
			const std::error_code& ec) noexcept;
	//--------------------------------------------------------------------------
		/**
		* виртуальный обработчик события завершения асинхронной записи.
		* @param bufferWrite - буфер данных.
		* @param dwReturnedBytes - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		virtual void asyncWriteCompletionHandler(
			const BYTE* bufferWrite,
			const DWORD dwReturnedBytes,
			const std::error_code& ec) noexcept;
	//--------------------------------------------------------------------------
		/**
		* виртуальный обработчик события закрытия.
		*/
		virtual void closeHandler() noexcept;
	//--------------------------------------------------------------------------
	#pragma endregion

	//--------------------------------------------------------------------------
	#pragma region Private_Method
	private:	
	//--------------------------------------------------------------------------
		/**
		* проверка стадии закрытия.
		* @return - TRUE если можно вызывать виртуальный обработчик закрытия.
		*/
		bool closeCheckPending() noexcept;
	//--------------------------------------------------------------------------
		/**
		* проверка стадии закрытия при асинхронной операции.
		* @param counterIoPending - счетчик операций.
		*/
		void closeCheckPending(
			misc::CCounterScoped& counterIoPending) noexcept;
	//--------------------------------------------------------------------------
		/**
		* обработчик события завершения асинхронного чтения.
		* @param asyncOperation - асинхронная операция.
		*/
		static void asyncReadIocpHandler(
			const iocp::CAsyncOperation& asyncOperation) noexcept;
	//--------------------------------------------------------------------------
		/**
		* обработчик события завершения асинхронной записи.
		* @param asyncOperation - асинхронная операция.
		*/
		static void asyncWriteIocpHandler(
			const iocp::CAsyncOperation& asyncOperation) noexcept;
	//--------------------------------------------------------------------------
		/**
		* обработчик события завершения асинхронной операции.
		* @param asyncOperation - асинхронная операция.
		*/
		static void asyncIocpHandler(
			const iocp::CAsyncOperation& asyncOperation) noexcept;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Data
	public:
	//--------------------------------------------------------------------------
		/** статистика обработанных байт */
		std::atomic_uint64_t _nCountReadByte = 0;
		std::atomic_uint64_t _nCountWriteByte = 0;
	
		/* порт ввода/вывода */
		std::shared_ptr<iocp::CIocp> _pIocp;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/* описатель */
		handle::CHandle _hAsyncHandle;

		/** счетчик операций ввода/вывода */
		misc::CCounter _counterIoPending;

		/** состояние асинхронного ввода/вывода */
		EIoState _eIoState = EIoState::close;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}