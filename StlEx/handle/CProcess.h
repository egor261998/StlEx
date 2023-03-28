#pragma once

namespace stlex::handle
{
	/** объект процесса */
	class STLEX CProcess
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		*/
		CProcess() noexcept = default;
	//--------------------------------------------------------------------------
		/**
		* создать процесс.
		* @param processPath - путь до процесса.
		* @param commandLine - аргументы запуска процесса.
		* @param dwCreationFlags - флаги создания процесса.
		* @return - результат работы функции.
		*/
		std::error_code create(
			const std::filesystem::path& processPath,
			const std::wstring_view& commandLine,
			const DWORD dwCreationFlags = 0);
	//--------------------------------------------------------------------------
		/**
		* создать процесс с установкой текущей директорией.
		* @param processPath - путь до процесса.
		* @param commandLine - аргументы запуска процесса.
		* @param currentDirectoryPath - текущая директория процесса.
		* @param dwCreationFlags - флаги создания процесса.
		* @return - результат работы функции.
		*/
		std::error_code create(
			const std::filesystem::path& processPath,
			const std::wstring_view& commandLine,
			const std::filesystem::path& currentDirectoryPath,
			const DWORD dwCreationFlags);
	//--------------------------------------------------------------------------
		/**
		* дождаться завершения процесса.
		* @return - результат работы функции.
		*/
		std::error_code waitProcess() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить код завершения процесса.
		* @param ec - код завершения процесса.
		* @return - результат работы функции.
		*/
		std::error_code getExitCode(
			std::error_code& ec) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* уничтожить процесс.
		* @param ec - код завершения процесса.
		* @return - результат работы функции.
		*/
		std::error_code kill(
			const std::error_code& ec = std::error_code(ERROR_PROCESS_ABORTED, std::system_category())) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* проверить валидность процесса.
		* @return - результат проверки.
		*/
		bool isValid() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* закрыть описатели.
		*/
		void close() noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		virtual ~CProcess();
	//--------------------------------------------------------------------------
		CProcess(const CProcess&) noexcept = default;
		CProcess(CProcess&&) noexcept = default;
		virtual CProcess& operator=(const CProcess&) noexcept = default;
		virtual CProcess& operator=(CProcess&&) noexcept = default;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** описатель процесса */
		CHandle _hProcess;

		/** описатель нити процесса */
		CHandle _hThread;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}