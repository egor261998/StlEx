#pragma once

/**
* 26493 - приведения в стиле C.
*/
#pragma warning (disable: 26493)
namespace stlex::exchange
{
	/** интерфейс обмена данными */
	template <class _TChannel>
	class STLEX IExchangeData : 
		public _TChannel
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор интерфейса обмена данными.
		* @param args - аргументы для наследника.
		*/
		template <class... Args>
		IExchangeData(
			Args&&... args) noexcept(false) :
			_TChannel(std::forward<Args>(args)...)
		{
			
		}
	//--------------------------------------------------------------------------
		/**
		* закончить работу и дождаться всех асинхронных операций.
		* @param bIsWait - признак ожидания.
		*/
		void release(
			const bool bIsWait) noexcept override
		{
			__super::release(bIsWait);
		}
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~IExchangeData()
		{
			release(true);
		}
	//--------------------------------------------------------------------------
		IExchangeData(const IExchangeData&) = delete;
		IExchangeData(IExchangeData&&) = delete;
		IExchangeData& operator=(const IExchangeData&) = delete;
		IExchangeData& operator=(IExchangeData&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion	

	#pragma region Protected_Method
	protected:
	//--------------------------------------------------------------------------
		/**
		* синхронная запись.
		* @param dwMessageType - тип сообщения.
		* @param bufferWrite - буфер для записи.
		* @param bufferSize - размер буфера для записи.
		* @return - результат записи.
		*/
		std::error_code startWrite(
			const DWORD dwMessageType,
			const BYTE* bufferWrite,
			const DWORD dwBufferSize)
		{
			misc::CCounterScoped counter(*this);
			if (!counter.isStartOperation())
				return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

			try
			{
				DWORD dwReturnedBytes = 0;

				/** заполняем шапку */
				CBufferExchange::SHead sHead;
				sHead.dwMessageType = dwMessageType;
				sHead.dwSizeData = dwBufferSize;

				/** пишем шапку */
				if (const auto ec = __super::startWrite(
					(BYTE*)&sHead,
					sizeof(sHead),
					dwReturnedBytes); ec)
					return ec;
				if (dwReturnedBytes != sizeof(sHead))
					return std::error_code(ERROR_INVALID_DATA, std::system_category());

				if (sHead.dwSizeData == 0)
				{
					/** данных нет */
					return {};
				}

				/** пишем данные */
				if (const auto ec = __super::startWrite(
					bufferWrite,
					sHead.dwSizeData,
					dwReturnedBytes); ec)
					return ec;
				if (dwReturnedBytes != sHead.dwSizeData)
					return std::error_code(ERROR_INVALID_DATA, std::system_category());

				/** успешное завершение */
				return {};
			}
			catch (const std::exception& ex)
			{
				__super::_pIocp->log(logger::ILogger::EMessageType::critical, ex);
				throw;
			}
		}
	//--------------------------------------------------------------------------
		/**
		* синхронная запись.
		* @param dwMessageType - тип сообщения.
		* @param bufferWrite - буфер для записи.
		* @return - результат записи.
		*/
		std::error_code startWrite(
			const DWORD dwMessageType,
			const std::vector<byte>& bufferWrite)
		{
			misc::CCounterScoped counter(*this);
			if (!counter.isStartOperation())
				return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

			try
			{
				/** размер данных слишком большой */
				if (bufferWrite.size() > INFINITE)
					return std::error_code(ERROR_INVALID_DATA, std::system_category());

				return startWrite(
					dwMessageType,
					bufferWrite.data(),
					(DWORD)bufferWrite.size());
			}
			catch (const std::exception& ex)
			{
				__super::_pIocp->log(logger::ILogger::EMessageType::critical, ex);
				throw;
			}
		}
	//--------------------------------------------------------------------------
		/**
		* синхронная запись.
		* @param dwMessageType - тип сообщения.
		* @param data - данные.
		* @return - результат записи.
		*/
		template <class... Args>
		std::error_code startWrite(
			const DWORD dwMessageType,
			const Args&... data)
		{
			misc::CCounterScoped counter(*this);
			if (!counter.isStartOperation())
				return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

			try
			{
				/** сериализуем данные */
				const auto bufferWrite = ISerializable::serializeReturn(
					data...);

				return startWrite(
					dwMessageType,
					bufferWrite);
			}
			catch (const std::exception& ex)
			{
				__super::_pIocp->log(logger::ILogger::EMessageType::critical, ex);
				throw;
			}
		}
	//--------------------------------------------------------------------------
		/**
		* асинхронная запись.
		* @param dwMessageType - тип сообщения.
		* @param data - данные для записи.
		* @param pContext - контекст данных.
		* @return - результат записи.
		*/
		template<class T>
		std::pair<std::error_code, handle::CEvent> startAsyncWrite(
			const DWORD dwMessageType,
			const T& data,
			PVOID pContext = nullptr)
		{
			const auto lock = __super::lockGuard();

			handle::CEvent ev(false);

			misc::CCounterScoped counter(*this);
			if (!counter.isStartOperation())
				return std::pair(
					std::error_code(ERROR_OPERATION_ABORTED, std::system_category()),
					ev);

			/** сериализуем данные */
			const auto bufferWrite = ISerializable::serializeReturn(data);

			/** размер данных слишком большой */
			if (bufferWrite.size() > INFINITE)
				return std::pair(
					std::error_code(ERROR_INVALID_DATA, std::system_category()),
					ev);
		
			/** сборка буфера */
			CBufferExchange bufferExchange;
			bufferExchange.putData(bufferWrite);
			bufferExchange._sHead.dwMessageType = dwMessageType;	
			bufferExchange._pContext = pContext;
			bufferExchange._eType = CBufferExchange::EType::eWrite;
			ev = bufferExchange._ev;

			/** добавляем в очередь */
			_queueAsyncWrite.push_back(std::move(bufferExchange));
			if (_queueAsyncWrite.size() != 1)
			{
				/** элемент в очереди не первый */
				counter.release();
				return std::pair({}, ev);
			}
				
			try
			{
				/** обрабатываем первый элемент очереди на запись */
				bool bIsReady = false;
				if (const auto ec = queueAsyncProcessing(
					_queueAsyncWrite.front(), bIsReady); ec)
				{
					_queueAsyncWrite.pop_front();
					return std::pair(ec, ev);
				}

				assert(!bIsReady);

				/** в обработке */
				counter.release();
				return std::pair({}, ev);
			}
			catch (const std::exception& ex)
			{
				_queueAsyncWrite.pop_front();
				__super::_pIocp->log(logger::ILogger::EMessageType::critical, ex);
				throw;
			}	
		}
	//--------------------------------------------------------------------------
		/**
		* синхронное чтение.
		* @param dwMessageType - тип сообщения.
		* @param bufferRead - буфер для чтения.
		* @return - результат чтения.
		*/
		std::error_code startRead(
			const DWORD dwMessageType,
			std::vector<byte>& bufferRead)
		{
			misc::CCounterScoped counter(*this);
			if (!counter.isStartOperation())
				return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

			try
			{
				DWORD dwReturnedBytes = 0;
				
				CBufferExchange::SHead sHead;

				/** читаем шапку */
				if (const auto ec = __super::startRead(
					(BYTE*)&sHead,
					sizeof(sHead),
					dwReturnedBytes); ec)
					return ec;
				if (dwReturnedBytes != sizeof(sHead))
					return std::error_code(ERROR_INVALID_DATA, std::system_category());

				if (sHead.dwSizeData == 0)
				{
					/** данных нет */
					return {};
				}

				/** перевыделение размера */
				bufferRead.resize(sHead.dwSizeData);

				/** читаем данные */
				if (const auto ec = __super::startRead(
					bufferRead.data(),
					sHead.dwSizeData,
					dwReturnedBytes); ec)
					return ec;
				if (dwReturnedBytes != sHead.dwSizeData || 
					sHead.dwMessageType != dwMessageType)
					return std::error_code(ERROR_INVALID_DATA, std::system_category());

				/** успешное завершение */
				return {};
			}
			catch (const std::exception& ex)
			{
				__super::_pIocp->log(logger::ILogger::EMessageType::critical, ex);
				throw;
			}
		}
	//--------------------------------------------------------------------------
		/**
		* синхронное чтение.
		* @param dwMessageType - тип сообщения.
		* @param data - данные для заполнения.
		* @return - результат чтения.
		*/
		template<class... Args>
		std::error_code startRead(
			const DWORD dwMessageType,
			Args&... data)
		{
			misc::CCounterScoped counter(*this);
			if (!counter.isStartOperation())
				return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

			try
			{
				/** получение данных из канала */
				std::vector<byte> bufferRead;
				if (const auto ec = startRead(
					dwMessageType, bufferRead); ec)
					return ec;

				/** заполнение данных */
				if (!bufferRead.empty() &&
					ISerializable::deserialize(
						bufferRead, 0, data...) == 0)
					return std::error_code(ERROR_INVALID_DATA, std::system_category());		

				/** успешное завершение */
				return {};
			}
			catch (const std::exception& ex)
			{
				__super::_pIocp->log(logger::ILogger::EMessageType::critical, ex);
				throw;
			}
		}
	//--------------------------------------------------------------------------
		/**
		* асинхронное чтение.
		* @param pContext - контекст данных.
		* @return - результат записи.
		*/
		std::pair<std::error_code, handle::CEvent> startAsyncRead(
			PVOID pContext = nullptr)
		{
			const auto lock = __super::lockGuard();

			handle::CEvent ev(false);

			misc::CCounterScoped counter(*this);
			if (!counter.isStartOperation())
				return std::pair(
					std::error_code(ERROR_OPERATION_ABORTED, std::system_category()),
					ev);
		
			/** сборка буфера */
			CBufferExchange bufferExchange;
			bufferExchange._pContext = pContext;
			bufferExchange._eType = CBufferExchange::EType::eRead;
			ev = bufferExchange._ev;

			/** добавляем в очередь */
			_queueAsyncRead.push_back(std::move(bufferExchange));
			if (_queueAsyncRead.size() != 1)
			{
				/** элемент в очереди не первый */
				counter.release();
				return std::pair({}, ev);
			}
				
			try
			{
				/** обрабатываем первый элемент очереди на чтение */
				bool bIsReady = false;
				if (const auto ec = queueAsyncProcessing(
					_queueAsyncRead.front(), bIsReady); ec)
				{
					_queueAsyncRead.pop_front();
					return std::pair(ec, ev);
				}

				assert(!bIsReady);

				/** в обработке */
				counter.release();
				return std::pair({}, ev);
			}
			catch (const std::exception& ex)
			{
				_queueAsyncRead.pop_front();
				__super::_pIocp->log(logger::ILogger::EMessageType::critical, ex);
				throw;
			}	
		}
	//--------------------------------------------------------------------------
		/**
		* сбросить все элементы в очереди на запись.
		* элемент который уже находится в канале, сброшен не будет.
		* @return - количество отмененных операций, событие операции в обработке.
		*/
		std::pair<size_t, handle::CEvent> cancelWriteQueue() noexcept
		{
			return cancelQueue(_queueAsyncWrite);
		}
	//--------------------------------------------------------------------------
		/**
		* сбросить все элементы в очереди на чтение.
		* элемент который уже находится в канале, сброшен не будет.
		* @return - количество отмененных операций, событие операции в обработке.
		*/
		std::pair<size_t, handle::CEvent> cancelReadQueue() noexcept
		{
			return cancelQueue(_queueAsyncRead);
		}
	//--------------------------------------------------------------------------
		/**
		* виртуальный обработчик события завершения асинхронной записи.
		* @param dwMessageType - тип сообщения.
		* @param bufferWrite - буфер с данными.
		* @param ec - код ошибки завершения.
		* @param pContext - контекст данных.
		*/
		virtual void asyncWriteCompletionHandler(
			const DWORD dwMessageType,
			const std::vector<byte>& bufferWrite,
			const std::error_code& ec,
			PVOID pContext) noexcept
		{
			UNREFERENCED_PARAMETER(dwMessageType);
			UNREFERENCED_PARAMETER(bufferWrite);
			UNREFERENCED_PARAMETER(ec);
			UNREFERENCED_PARAMETER(pContext);
		}
	//--------------------------------------------------------------------------
		/**
		* виртуальный обработчик события завершения асинхронного чтения.
		* @param dwMessageType - тип сообщения.
		* @param bufferRead - буфер с данными.
		* @param ec - код ошибки завершения.
		* @param pContext - контекст данных.
		*/
		virtual void asyncReadCompletionHandler(
			const DWORD dwMessageType,
			const std::vector<byte>& bufferRead,
			const std::error_code& ec,
			PVOID pContext) noexcept
		{
			UNREFERENCED_PARAMETER(dwMessageType);
			UNREFERENCED_PARAMETER(bufferRead);
			UNREFERENCED_PARAMETER(ec);
			UNREFERENCED_PARAMETER(pContext);
		}
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Method
	private:
	//--------------------------------------------------------------------------
		/**
		* сбросить все элементы в очереди.
		* элемент который уже находится в канале, сброшен не будет.
		* @param qeueAsync - очередь.
		* @return - количество отмененных операций, событие операции в обработке.
		*/
		std::pair<size_t, handle::CEvent> cancelQueue(
			std::list<CBufferExchange>& qeueAsync) noexcept
		{
			std::list<CBufferExchange> cancelList;
			handle::CEvent hEvent(false);
			{
				const auto lock = __super::lockGuard();

				if (!qeueAsync.empty())
				{
					/** получаем первый итератор */
					auto it = qeueAsync.begin();
					if (it->_eState == CBufferExchange::EState::eNone)
					{
						/** элемент не используется, нужно сбросить весь лист */
						cancelList = std::move(qeueAsync);
					}
					else
					{
						try
						{
							/** элемент используется, 
								нужно сбросить весь лист кроме первого */
							hEvent = it->_ev;
							cancelList.splice(
								cancelList.begin(),
								qeueAsync,
								++it,
								qeueAsync.end());
						}
						catch (const std::exception& ex)
						{
							__super::_pIocp->log(
								logger::ILogger::EMessageType::warning,
								ex);
						}					
					}
				}
			}

			/** сбрасываем блоки с кодом ошибки */
			for (auto& it : cancelList)
			{
				it._ec = std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
				asyncCompletionHandlerInner(it);
			}

			return { cancelList.size(), hEvent };
		}
	//--------------------------------------------------------------------------
		/**
		* получить из очереди буфер обмена и удалить его из очереди.
		* @param qeueAsync - очередь.
		* @return - буфер обмена данных.
		*/
		CBufferExchange getBufferExchangeWithPop(
			std::list<CBufferExchange>& qeueAsync) noexcept
		{
			const auto lock = __super::lockGuard();

			auto bufferExchange = std::move(qeueAsync.front());
			qeueAsync.pop_front();

			return bufferExchange;
		}
	//--------------------------------------------------------------------------
		/**
		* признак продолжения обработки очереди.
		* @param qeueAsync - очередь.
		* @return - признак продолжения.
		*/
		bool isProcessingQueue(
			const std::list<CBufferExchange>& qeueAsynс) noexcept
		{
			const auto lock = __super::lockGuard();
			if (qeueAsynс.empty())
			{
				/** очередь пустая */
				return false;
			}

			if (qeueAsynс.front()._eState != CBufferExchange::EState::eNone)
			{
				/** операция уже идет */
				return false;
			}

			/** можно обрабатывать следующий элемент */
			return true;
		}
	//--------------------------------------------------------------------------
		/**
		* обработчик события завершения асинхронной записи.
		* @param bufferWrite - буфер данных.
		* @param dwReturnedBytes - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		void asyncWriteCompletionHandler(
			const BYTE* bufferWrite,
			const DWORD dwReturnedBytes,
			const std::error_code& ec) noexcept final
		{
			UNREFERENCED_PARAMETER(bufferWrite);

			asyncCompletionHandler(_queueAsyncWrite, dwReturnedBytes, ec);
		}
	//--------------------------------------------------------------------------
		/**
		* обработка буфера данных обмена.
		* @param bufferExchange - буфер обмена данными.
		* @param bIsReady - признак полностью готового буфера обмена данных.
		* @param dwReturnedBytes - количество возвращенных байт.
		* @return - результат обработки.
		*/
		std::error_code queueAsyncProcessing(
			CBufferExchange& bufferExchange,
			bool& bIsReady,
			const DWORD dwReturnedBytes = 0)
		{
			bIsReady = false;
			std::error_code ec;

			const auto lock = __super::lockGuard();

			if (!__super::isInitialize())
			{
				ec = std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
				bufferExchange._ec = ec;
				bufferExchange._eState = CBufferExchange::EState::eReady;
				bIsReady = true;
				return ec;
			}

			try
			{
				switch (bufferExchange._eState)
				{
				case CBufferExchange::EState::eNone:
				{
					switch (bufferExchange._eType)
					{
					case CBufferExchange::EType::eRead:
						/** читаем шапку */
						ec = __super::startAsyncRead(
							(BYTE*)&bufferExchange._sHead,
							sizeof(bufferExchange._sHead));
						break;
					case CBufferExchange::EType::eWrite:
						/** записываем шапку */
						ec = __super::startAsyncWrite(
							(const BYTE*)&bufferExchange._sHead,
							sizeof(bufferExchange._sHead));
						break;
					default:
						ec = std::error_code(ERROR_INVALID_DATA, std::system_category());
						__super::_pIocp->log(
							logger::ILogger::EMessageType::warning,
							L"bufferExchange._eType is invalid",
							ec);
						break;
					}
					
					if (ec)
					{
						bufferExchange._ec = ec;
						bufferExchange._eState = CBufferExchange::EState::eReady;
						bIsReady = true;
						return ec;
					}

					/** операция записи в обработке */
					bufferExchange._eState = CBufferExchange::EState::eHeadPening;
					break;
				}
				case CBufferExchange::EState::eHeadPening:
				{	
					if (dwReturnedBytes != sizeof(bufferExchange._sHead))
					{
						ec = std::error_code(
							ERROR_INVALID_DATA, std::system_category());
						bufferExchange._ec = ec;
						bufferExchange._eState = CBufferExchange::EState::eReady;
						bIsReady = true;
						return ec;
					}

					if (bufferExchange._sHead.dwSizeData == 0)
					{
						/** данных нет */

						/** буфер данных обмена полностью обработан */
						bufferExchange._eState = CBufferExchange::EState::eReady;
						bIsReady = true;
						return {};
					}

					switch (bufferExchange._eType)
					{
					case CBufferExchange::EType::eRead:
						bufferExchange._pBuffer.resize(bufferExchange._sHead.dwSizeData);
						/** читаем данные */
						ec = __super::startAsyncRead(
							bufferExchange._pBuffer.data(),
							bufferExchange._sHead.dwSizeData);
						break;
					case CBufferExchange::EType::eWrite:
						/** записываем данные */
						ec = __super::startAsyncWrite(
							bufferExchange._pBuffer.data(),
							bufferExchange._sHead.dwSizeData);
						break;
					default:
						ec = std::error_code(ERROR_INVALID_DATA, std::system_category());
						__super::_pIocp->log(
							logger::ILogger::EMessageType::warning,
							L"bufferExchange._eType is invalid",
							ec);
						break;
					}

					if (ec)
					{
						bufferExchange._ec = ec;
						bufferExchange._eState = CBufferExchange::EState::eReady;
						bIsReady = true;
						return ec;
					}

					/** операция записи в обработке */
					bufferExchange._eState = CBufferExchange::EState::eDataPending;
					break;
				}
				case CBufferExchange::EState::eDataPending:
				{
					if (dwReturnedBytes != bufferExchange._sHead.dwSizeData)
					{
						ec = std::error_code(
							ERROR_INVALID_DATA, std::system_category());
						bufferExchange._ec = ec;
						bufferExchange._eState = CBufferExchange::EState::eReady;
						bIsReady = true;
						return ec;
					}

					/** буфер данных обмена полностью обработан */
					bufferExchange._eState = CBufferExchange::EState::eReady;
					bIsReady = true;
					break;
				}
				default:
				{
					ec = std::error_code(
						ERROR_INVALID_DATA, std::system_category());
					__super::_pIocp->log(
						logger::ILogger::EMessageType::warning,
						L"Invalid state",
						ec);
					bufferExchange._ec = ec;
					bufferExchange._eState = CBufferExchange::EState::eReady;
					bIsReady = true;
					return ec;
				}
				}

				return {};
			}
			catch (const std::exception& ex)
			{
				__super::_pIocp->log(logger::ILogger::EMessageType::critical, ex);

				bufferExchange._ec = std::error_code(
					ERROR_EXTENDED_ERROR, std::system_category());
				bufferExchange._eState = CBufferExchange::EState::eReady;
				bIsReady = true;

				throw;
			}
		}
	//--------------------------------------------------------------------------
		/**
		* обработчик события завершения асинхронного чтения.
		* @param bufferRead - буфер данных.
		* @param dwReturnedBytes - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		void asyncReadCompletionHandler(
			BYTE* bufferRead,
			const DWORD dwReturnedBytes,
			const std::error_code& ec) noexcept final
		{
			UNREFERENCED_PARAMETER(bufferRead);

			asyncCompletionHandler(_queueAsyncRead, dwReturnedBytes, ec);
		}
	//--------------------------------------------------------------------------
		/**
		* обработчик события завершения асинхронного буфера.
		* @param qeueAsync - очередь.
		* @param dwReturnedBytes - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		void asyncCompletionHandler(
			std::list<CBufferExchange>& qeueAsync,
			const DWORD dwReturnedBytes,
			const std::error_code& ec) noexcept
		{
			auto lock = __super::lockGuard();

			if (ec)
			{
				/** ошибка завершения чтения */
				auto bufferExchange = getBufferExchangeWithPop(
					qeueAsync);

				lock.free();

				/** завершаем работу с буфером обмена данными */
				bufferExchange._ec = ec;
				bufferExchange._eState = CBufferExchange::EState::eReady;
				asyncCompletionHandlerInner(bufferExchange);

				/** обработка нового буфера обмена данных */
				newAsyncBufferExchange(qeueAsync);
				return;
			}

			try
			{
				/** обрабатываем буфер обмена данными */
				bool bIsReady = false;
				if (const auto ecProcessing = queueAsyncProcessing(
					qeueAsync.front(),
					bIsReady,
					dwReturnedBytes); ecProcessing)
				{
					/** ошибка завершения обработки */
					auto bufferExchange = getBufferExchangeWithPop(
						qeueAsync);

					lock.free();

					asyncCompletionHandlerInner(bufferExchange);

					/** обработка нового буфера обмена данных */
					newAsyncBufferExchange(qeueAsync);
					return;
				}

				/** необходимо узнать закончена ли обработка */
				if (bIsReady)
				{
					auto bufferExchange = getBufferExchangeWithPop(
						qeueAsync);

					lock.free();

					asyncCompletionHandlerInner(bufferExchange);

					/** обработка нового буфера обмена данных */
					newAsyncBufferExchange(qeueAsync);
					return;
				}
			}
			catch (const std::exception& ex)
			{
				__super::_pIocp->log(logger::ILogger::EMessageType::warning, ex);

				/** заканчиваем с текущем буфером обмена данными */
				auto bufferExchange = getBufferExchangeWithPop(
					qeueAsync);

				lock.free();

				asyncCompletionHandlerInner(bufferExchange);

				/** обработка нового буфера обмена данных */
				newAsyncBufferExchange(qeueAsync);
				return;
			}
		}
	//--------------------------------------------------------------------------
		/**
		* обработка нового буфера данных для чтения.
		* @param qeueAsync - очередь.
		*/
		void newAsyncBufferExchange(
			std::list<CBufferExchange>& qeueAsync) noexcept
		{
			while (true)
			{
				auto lock = __super::lockGuard();

				/** проверка очереди на возможность обработки */
				if (!isProcessingQueue(qeueAsync))
					return;

				try
				{
					bool bIsReady = false;

					if (const auto ecProcessing = queueAsyncProcessing(
						qeueAsync.front(),
						bIsReady); ecProcessing)
					{
						/** получаем элемент из очереди */
						auto bufferExchange = getBufferExchangeWithPop(
							qeueAsync);

						lock.free();

						/** завершение работы с элементом */
						asyncCompletionHandlerInner(bufferExchange);

						/** переход к новому элементу */
						continue;
					}

					assert(!bIsReady);
					return;
				}
				catch (const std::exception& ex)
				{
					__super::_pIocp->log(logger::ILogger::EMessageType::warning, ex);

					/** получаем элемент из очереди */
					auto bufferExchange = getBufferExchangeWithPop(
						qeueAsync);

					lock.free();

					/** завершение работы с элементом */
					asyncCompletionHandlerInner(bufferExchange);

					/** переход к новому элементу */
					continue;
				}
			}		
		}
	//--------------------------------------------------------------------------
		/**
		* обработчик события завершения асинхронной операции.
		* @param bufferExchange - буфер обмена данных.
		*/
		void asyncCompletionHandlerInner(
			const CBufferExchange& bufferExchange) noexcept
		{
			switch (bufferExchange._eType)
			{
			case CBufferExchange::EType::eRead:
				asyncReadCompletionHandler(
					bufferExchange._sHead.dwMessageType,
					bufferExchange._pBuffer,
					bufferExchange._ec,
					bufferExchange._pContext);
				break;
			case CBufferExchange::EType::eWrite:
				asyncWriteCompletionHandler(
					bufferExchange._sHead.dwMessageType,
					bufferExchange._pBuffer,
					bufferExchange._ec,
					bufferExchange._pContext);
				break;
			default:
				__super::_pIocp->log(
					logger::ILogger::EMessageType::warning,
					L"bufferExchange._eType is invalid",
					std::error_code(ERROR_INVALID_DATA, std::system_category()));
				break;
			}
			
			if (const auto ecNotify = bufferExchange._ev.notify(); ecNotify)
				__super::_pIocp->log(
					logger::ILogger::EMessageType::warning,
					L"notify failed",
					ecNotify);
			__super::endOperation();
		}
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** очередь на запись */
		std::list<CBufferExchange> _queueAsyncWrite;
		/** очередь на чтение */
		std::list<CBufferExchange> _queueAsyncRead;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}
#pragma warning (default: 26493)