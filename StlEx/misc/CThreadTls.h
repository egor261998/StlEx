#pragma once

namespace stlex::misc
{
	/** TLS */
	template <class T>
	class STLEX CThreadTls final
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		*/
		CThreadTls() noexcept(false)
		{		
			/** выделяем новый индекс */
			_dwTlsIndex = TlsAlloc();
			if (_dwTlsIndex == TLS_OUT_OF_INDEXES)
			{
				throw std::runtime_error("TlsAlloc failed with error: " +
					std::to_string(GetLastError()));
			}
		}
	//--------------------------------------------------------------------------
		/**
		* установить значение.
		* @param p - значение.
		* @return - код ошибки.
		*/
		std::error_code setValue(T* p) const noexcept
		{
			const auto bResult = TlsSetValue(_dwTlsIndex, p);

			return std::error_code(
				bResult ? ERROR_SUCCESS : GetLastError(),
				std::system_category());
		}
	//--------------------------------------------------------------------------
		/**
		* получить значение с перезаписью SetLastError.
		* @return - значение.
		*/
		T* getValue() const noexcept
		{
			return static_cast<T*>(TlsGetValue(_dwTlsIndex));
		}
	//--------------------------------------------------------------------------
		/**
		* получить значение без перезаписи SetLastError.
		* @param ec - код ошибки после работы функции.
		* @return - значение.
		*/
		T* getValue(
			std::error_code& ec) const noexcept
		{
			const auto dwErrorCode = GetLastError();
			const auto value = getValue();
			ec = std::error_code(GetLastError(), std::system_category());
			SetLastError(dwErrorCode);
			return value;
		}
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CThreadTls()
		{
			if (_dwTlsIndex == TLS_OUT_OF_INDEXES)
			{
				/** индекса нет, высвобождать нечего */
				return;
			}

			TlsFree(_dwTlsIndex);		
		}
	//--------------------------------------------------------------------------
		CThreadTls(const CThreadTls& tls) noexcept = default;
		CThreadTls(CThreadTls&& tls) noexcept = default;
		CThreadTls& operator=(const CThreadTls& tls) noexcept = default;
		CThreadTls& operator=(CThreadTls && tls) noexcept = default;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** индекс */
		DWORD _dwTlsIndex = TLS_OUT_OF_INDEXES;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}