#pragma once

namespace stlex::pipe::server
{
	/** интерфейс прослушивающего сервера pipe */
	class STLEX IListenPipe : 
		virtual public misc::CCounter,
		public misc::CPathStorage
	{
	#pragma region Public_Inner
	public:
	//--------------------------------------------------------------------------
		/** интерфейс клиента pipe */
		class IListenPipeClient;

		/** клиент */
		class IListenNPipeClient;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор прослушивателя.
		* @param pipePath - путь до pipe.
		* @param pIocp - механизм ввода/вывода.
		*/
		IListenPipe(
			const std::filesystem::path& pipePath,
			const std::shared_ptr<io::iocp::CIocp>& pIocp = nullptr);
	//--------------------------------------------------------------------------
		/**
		* начало приема клиент.
		*/
		void acceptClient() noexcept;
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
		~IListenPipe();
	//--------------------------------------------------------------------------
		IListenPipe(const IListenPipe&) = delete;
		IListenPipe(IListenPipe&&) = delete;
		IListenPipe& operator=(const IListenPipe&) = delete;
		IListenPipe& operator=(IListenPipe&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//--------------------------------------------------------------------------
		/**
		* виртуальный обработчик события ошибки подключения клиента.
		* @param ec - код ошибки.
		* @return - признак продолжения попытки подключения.
		*/
		virtual bool connectErrorHandler(
			const std::error_code& ec) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* виртуальное создание клиента.
		* @return - созданный клиент.
		*/
		virtual std::unique_ptr<IListenPipeClient> createClient() = 0;
	//--------------------------------------------------------------------------
		/**
		* удалить клиента из списка.
		* @param pPipe - указатель на клиента.
		*/
		void deleteClient(
			IListenPipeClient* const pPipe) noexcept;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Data
	public:
	//--------------------------------------------------------------------------
		/** механизм ввода/вывода */
		std::shared_ptr<io::iocp::CIocp> _pIocp;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//--------------------------------------------------------------------------
		/** подключенные клиенты */
		std::unordered_map<IListenPipeClient*, std::shared_ptr<IListenPipeClient>> _clients;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}