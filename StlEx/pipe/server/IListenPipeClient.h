#pragma once

namespace stlex::pipe::server
{
	/** интерфейс клиента pipe для слушателя */
	class STLEX IListenPipe::IListenPipeClient : 
		public IPipe
	{
		friend class IListenPipe;
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/*
		* конструктор клиента интерфейса pipe.
		* @param listenPipe - родительный объект.
		*/
		IListenPipeClient(
			IListenPipe& listenPipe);
	//--------------------------------------------------------------------------
		/**
		* удалить клиента из списка.
		*/
		void deleteClient() noexcept;
	//--------------------------------------------------------------------------
		/**
		* закончить работу.
		* @param bIsWait - признак ожидания.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~IListenPipeClient();
	//--------------------------------------------------------------------------
		IListenPipeClient(const IListenPipeClient&) = delete;
		IListenPipeClient(IListenPipeClient&&) = delete;
		IListenPipeClient& operator=(const IListenPipeClient&) = delete;
		IListenPipeClient& operator=(IListenPipeClient&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Method
	private:
	//--------------------------------------------------------------------------
		/**
		* обработчик события подключения клиента к серверу.
		* @param asyncOperation - асинхронная операция.
		*/
		static void connectPipeHandler(
			const io::iocp::CAsyncOperation& asyncOperation) noexcept;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//--------------------------------------------------------------------------
		/** родительный объект */
		IListenPipe& _listenPipe;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}