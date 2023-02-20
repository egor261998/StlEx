#pragma once

namespace stlex::pipe::server
{
	/** интерфейс клиента NamedPipe для слушателя */
	class STLEX IListenPipe::IListenNPipeClient : 
		public IListenPipeClient
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор клиента NamedPipe.
		* @param listenPipe - родительный объект.
		*/
		IListenNPipeClient(
			IListenPipe& listenPipe);
	//--------------------------------------------------------------------------
		/**
		* присоединится к серверу.
		* @param dwRepeatCount - количество попыток подключения, если канал занят.
		* @param dwMilliseconds - ожидания канала.
		* @return - код ошибки.
		*/
		std::error_code connectToServer(
			const DWORD dwRepeatCount = 0,
			const DWORD dwMilliseconds = 0) noexcept override;
	//--------------------------------------------------------------------------
		/**
		* ожидание клиента сервером.
		* @return - код ошибки.
		*/
		std::error_code waitClient() override;
	//--------------------------------------------------------------------------
		/**
		* ожидание доступности серверного именованного канала.
		* @param dwMilliseconds - время ожидания уведомления.
		* @return - код ошибки.
		*/
		std::error_code waitServerToConnect(
			const DWORD dwMilliseconds) noexcept override;
	//--------------------------------------------------------------------------
		/**
		* закрыть NPipe.
		*/
		void close() noexcept override;
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
		~IListenNPipeClient();
	//--------------------------------------------------------------------------
		IListenNPipeClient(const IListenNPipeClient&) = delete;
		IListenNPipeClient(IListenNPipeClient&&) = delete;
		IListenNPipeClient& operator=(const IListenNPipeClient&) = delete;
		IListenNPipeClient& operator=(IListenNPipeClient&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}