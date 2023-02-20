#pragma once

namespace stlex::handle
{
	/** реализация описателя в классе */
	class STLEX CHandle
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		*/
		CHandle() noexcept = default;
	//--------------------------------------------------------------------------
		/**
		* конструктор из описателя.
		* @param hObject - новый описатель.
		*/
		CHandle(
			const HANDLE hObject);
	//--------------------------------------------------------------------------
		/**
		* проверить валидность.
		* @return - результат проверки.
		*/
		bool isValid() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* проверить валидность.
		* @param hObject - описатель.
		* @return - результат проверки.
		*/
		static bool isValid(
			const HANDLE hObject) noexcept;
	//--------------------------------------------------------------------------
		/**
		* получить описатель.
		* @return - описатель.
		*/
		HANDLE getHandle() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* ожидание уведомлений для нескольких событий.
		* @param bWaitAll - ожидать всех.
		* @param dwMilliseconds - время ожидания уведомления.
		* @param args - события.
		* @return - код ошибки.
		*/
		template<class... Args>
		static std::error_code wait(
			const bool bWaitAll,
			const DWORD dwMilliseconds,
			Args&&... args)
		{
			const auto numbers = sizeof...(args);

			std::vector<HANDLE> vecHandles;	
			vecHandles.reserve(numbers);
			(vecHandles.push_back(args), ...);

			return std::error_code(
				WaitForMultipleObjects(
					numbers,
					vecHandles.data(),
					bWaitAll,
					dwMilliseconds),
				std::system_category());
		}
	//--------------------------------------------------------------------------
		/**
		* ожидание уведомления.
		* @param dwMilliseconds - время ожидания уведомления.
		* @param bAlertable - признак ожидания APC.
		* @return - код ошибки.
		*/
		std::error_code wait(
			const DWORD dwMilliseconds = INFINITE,
			const bool bAlertable = false) const noexcept;
	//--------------------------------------------------------------------------
		/**
		* бесконечное ожидание сигнала уведомления.
		* @return - код ошибки.
		*/
		std::error_code waitSignal() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* закрыть описатель.
		*/
		void close() noexcept;
	//--------------------------------------------------------------------------
		/**
		* оператор присвоения описателя.
		* @param hObject - новый описатель.
		* @return - текущий объект.
		*/
		virtual CHandle& operator=(
			const HANDLE hObject);
	//--------------------------------------------------------------------------
		/**
		* оператор получения описателя.
		* @return - описатель.
		*/
		operator HANDLE() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		virtual ~CHandle();
	//--------------------------------------------------------------------------
		CHandle(const CHandle&) noexcept = default;
		CHandle(CHandle&&) noexcept = default;
		virtual CHandle& operator=(const CHandle&) noexcept = default;
		virtual CHandle& operator=(CHandle&&) noexcept = default;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//--------------------------------------------------------------------------
		/** описатель */
		std::shared_ptr<void> _pHandle;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}