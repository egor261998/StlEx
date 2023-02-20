#pragma once

namespace stlex::pipe::client
{
	/** клиент для работы с NamedPipe */
	class STLEX CNPipeClient : 
		public IPipe
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор клиента NamedPipe.
		* @param npipePath - путь до NPipe.
		* @param pIocp - механизм ввода/вывода.
		*/
		CNPipeClient(
			const std::filesystem::path& npipePath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp = nullptr);
	//--------------------------------------------------------------------------
		/**
		* присоединится к серверу.
		* @param dwRepeatCount - количество попыток подключения, если канал занят.
		* @param dwMilliseconds - ожидания канала.
		* @return - код ошибки.
		*/
		std::error_code connectToServer(
			const DWORD dwRepeatCount = 0,
			const DWORD dwMilliseconds = 0) override;
	//--------------------------------------------------------------------------
		/**
		* ожидание клиента сервером.
		* @return - код ошибки.
		*/
		std::error_code waitClient() noexcept override;
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
		~CNPipeClient();
	//--------------------------------------------------------------------------
		CNPipeClient(const CNPipeClient&) = delete;
		CNPipeClient(CNPipeClient&&) = delete;
		CNPipeClient& operator=(const CNPipeClient&) = delete;
		CNPipeClient& operator=(CNPipeClient&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}