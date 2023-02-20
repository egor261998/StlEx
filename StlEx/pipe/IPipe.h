#pragma once

namespace stlex::pipe
{
	/** интерфейс работы с pipe */
	class STLEX IPipe : 
		public io::IAsyncIo,
		public misc::CPathStorage
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор интерфейса pipe.
		* @param pipePath - путь до pipe.
		* @param pIocp - механизм ввода/вывода.
		*/
		IPipe(
			const std::filesystem::path& pipePath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp = nullptr);
	//--------------------------------------------------------------------------
		/**
		* виртуально присоединится к серверу.
		* @param dwRepeatCount - количество попыток подключения, если канал занят.
		* @param dwMilliseconds - ожидания канала.
		* @return - код ошибки.
		*/
		virtual std::error_code connectToServer(
			const DWORD dwRepeatCount = 0,
			const DWORD dwMilliseconds = 0) = 0;
	//--------------------------------------------------------------------------
		/**
		* виртуальное ожидание клиента сервером.
		* @return - код ошибки.
		*/
		virtual std::error_code waitClient() = 0;
	//--------------------------------------------------------------------------
		/**
		* виртуальное ожидание доступности серверного именованного канала.
		* @param dwMilliseconds - время ожидания уведомления.
		* @return - код ошибки.
		*/
		virtual std::error_code waitServerToConnect(
			const DWORD dwMilliseconds) noexcept = 0;
	//--------------------------------------------------------------------------
		/**
		* виртуально закрыть pipe.
		*/
		virtual void close() noexcept = 0;
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
		~IPipe();
	//--------------------------------------------------------------------------
		IPipe(const IPipe&) = delete;
		IPipe(IPipe&&) = delete;
		IPipe& operator=(const IPipe&) = delete;
		IPipe& operator=(IPipe&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}