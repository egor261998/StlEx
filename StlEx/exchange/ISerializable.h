#pragma once

/**
* 26415 - замечание по передачу умного указателя в метод.
* 26418 - предлагает вообще не использовать умный указатель.
* 26481 - логика указателей.
*/
#pragma warning (disable: 26415 26418 26481)
namespace stlex::exchange
{
	/** интерфейс сериализации */
	class STLEX ISerializable
	{
	#pragma region Private_Inner
	private:
	//--------------------------------------------------------------------------
		/** вспомогательный сериализатор */
		template<class T, bool isSerializable = std::is_base_of<ISerializable, T>::value>
		struct serializable;
	//--------------------------------------------------------------------------
		/** сериализатор для объектов которые унаследованы */
		template<class T>
		struct serializable<T, true> final
		{
		//----------------------------------------------------------------------
			/**
			* перевод данных в байтовый массив.
			* @param buffer - буфер для добавления.
			* @param data - переводимые данные.
			*/
			static void serialize(				
				std::vector<BYTE>& buffer,
				const ISerializable& data)
			{
				data.serialize(buffer);
			}
		//----------------------------------------------------------------------
			/**
			* перевод байтового массива в данные.
			* @param buffer - байтовый массив.
			* @param offset - смещение текущего перевода.
			* @param data - переводимые данные.
			* @return - количество переведенных байт.
			*/
			static size_t deserialize(
				const std::vector<BYTE>& buffer,
				const size_t offset,
				ISerializable& data)
			{
				return data.deserialize(buffer, offset);
			}
		//----------------------------------------------------------------------
		};
	//--------------------------------------------------------------------------
		/** сериализатор для базовых типов */
		template<class T>
		struct serializable<T, false> final
		{
		//----------------------------------------------------------------------
			/**
			* перевод данных в байтовый массив.
			* @param buffer - буфер для добавления.
			* @param data - переводимые данные.
			*/
			static void serialize(
				std::vector<BYTE>& buffer,
				const T& data)
			{
				const auto offset = buffer.size();
				buffer.resize(buffer.size() + sizeof(data));
				memcpy(buffer.data() + offset, &data, sizeof(data));
			}
		//----------------------------------------------------------------------
			/**
			* перевод байтового массива в данные.
			* @param buffer - байтовый массив.
			* @param offset - смещение текущего перевода.
			* @param data - переводимые данные.
			* @return - количество переведенных байт.
			*/
			static size_t deserialize(
				const std::vector<BYTE>& buffer,
				const size_t offset,
				T& data)
			{
				if ((buffer.size() - offset) < sizeof(data))
					return 0;

				memcpy(&data, &buffer.at(offset), sizeof(data));

				return sizeof(data);
			}
		//----------------------------------------------------------------------
		};
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		*/
		ISerializable() noexcept = default;
	//--------------------------------------------------------------------------
		/**
		* виртуальный перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		*/
		virtual void serialize(
			std::vector<BYTE>& buffer) const = 0;
	//--------------------------------------------------------------------------
		/**
		* виртуальный перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @return - количество переведенных байт.
		*/
		virtual size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset) = 0;
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		static void serialize(
			std::vector<BYTE>& buffer,
			const std::filesystem::path& data);
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			std::filesystem::path& data);
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		static void serialize(
			std::vector<BYTE>& buffer,
			const std::error_code& data);
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			std::error_code& data);
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		template <class T>
		static void serialize(
			std::vector<BYTE>& buffer,
			const std::basic_string<T, std::char_traits<T>, std::allocator<T>>& data)
		{
			const size_t count = data.size();
			serialize(buffer, count);
			if (count == 0)
				return;

			const size_t offset = buffer.size();
			buffer.resize(buffer.size() + count * sizeof(T));
			memcpy(buffer.data() + offset, data.c_str(), count * sizeof(T));
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		template <class T>
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			std::basic_string<T, std::char_traits<T>, std::allocator<T>>& data)
		{
			data.clear();

			/** получение количество элементов */
			size_t count = 0;
			const size_t returnedBytes = deserialize(buffer, offset, count);
			if (returnedBytes == 0)
				return 0;

			if (count == 0)
				return returnedBytes;

			const size_t offsetTemp = offset + returnedBytes;

			if ((buffer.size() - offsetTemp) < (count * sizeof(T)))
				return 0;

			data.resize(count);
			memcpy(data.data(), &buffer.at(offsetTemp), count * sizeof(T));

			return returnedBytes + count * sizeof(T);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		template <class T>
		static void serialize(
			std::vector<BYTE>& buffer,
			const std::unique_ptr<T>& data)
		{
			if (!data)
				return;

			serialize(buffer, *data);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		template <class T>
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			std::unique_ptr<T>& data)
		{
			if (!data)
			{
				data = std::make_unique<T>();
			}

			return deserialize(buffer, offset, *data);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		template <class T>
		static void serialize(
			std::vector<BYTE>& buffer,
			const std::shared_ptr<T>& data)
		{
			if (!data)
				return;

			serialize(buffer, *data);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		template <class T>
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			std::shared_ptr<T>& data)
		{
			if (!data)
			{
				data = std::make_shared<T>();
			}

			return deserialize(buffer, offset, *data);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		template <class T>
		static void serialize(
			std::vector<BYTE>& buffer,
			const std::vector<T>& data)
		{
			const size_t count = data.size();
			serialize(buffer, count);

			for (const auto& it : data)
				serialize(buffer, it);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		template <class T>
		static DWORD deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			std::vector<T>& data)
		{
			data.clear();

			/** получение количество элементов */
			size_t count = 0;
			size_t returnedBytes = deserialize(buffer, offset, count);
			if (returnedBytes == 0)
				return 0;

			if (count == 0)
				return returnedBytes;

			size_t offsetTemp = offset + returnedBytes;

			for (size_t i = 0; i < count; i++)
			{
				/** получаем объект */
				T dataTemp = {};
				const auto returnedBytesTemp = deserialize(
					buffer, offsetTemp, dataTemp);
				if (returnedBytesTemp == 0)
					return 0;

				offsetTemp += returnedBytesTemp;
				returnedBytes += returnedBytesTemp;

				/** перемещаем объект в лист */
				data.push_back(std::move(dataTemp));
			}

			return returnedBytes;
		}
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		template <class T>
		static void serialize(
			std::vector<BYTE>& buffer,
			const std::list<T>& data)
		{
			const size_t count = data.size();
			serialize(buffer, count);

			for (const auto& it : data)
				serialize(buffer, it);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		template <class T>
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			std::list<T>& data)
		{
			data.clear();

			/** получение количество элементов */
			size_t count = 0;
			size_t returnedBytes = deserialize(buffer, offset, count);
			if (returnedBytes == 0)
				return 0;

			if (count == 0)
				return returnedBytes;

			size_t offsetTemp = offset + returnedBytes;

			for (size_t i = 0; i < count; i++)
			{
				/** получаем объект */
				T dataTemp = {};
				const auto returnedBytesTemp = deserialize(
					buffer, offsetTemp, dataTemp);
				if (returnedBytesTemp == 0)
					return 0;

				offsetTemp += returnedBytesTemp;
				returnedBytes += returnedBytesTemp;

				/** перемещаем объект в лист */
				data.push_back(std::move(dataTemp));
			}

			return returnedBytes;
		}
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		template <class KT, class VT>
		static void serialize(
			std::vector<BYTE>& buffer,
			const std::pair<KT, VT>& data)
		{
			serialize(buffer, data.first, data.second);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		template <class KT, class VT>
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			std::pair<KT, VT>& data)
		{
			return deserialize(buffer, offset, data.first, data.second);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		template <class... Args>
		static void serialize(
			std::vector<BYTE>& buffer,
			const std::tuple<Args...>& data)
		{		
			serializeTuple(
				buffer, data, std::make_index_sequence<sizeof...(Args)>{});
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		template <class... Args>
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			std::tuple<Args...>& data)
		{		
			return deserializeTuple(
				buffer, offset, data, std::make_index_sequence<sizeof...(Args)>{});
		}
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		template <class KT, class VT>
		static void serialize(
			std::vector<BYTE>& buffer,
			const std::unordered_map<KT, VT>& data)
		{
			const auto offset = buffer.size();
			const size_t count = data.size();

			/** количество элементов */
			serialize(buffer, count);

			for (const auto& it : data)
				serialize(buffer, it);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		template <class KT, class VT>
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			std::unordered_map<KT, VT>& data)
		{
			data.clear();

			/** получение количество элементов */
			size_t count = 0;
			size_t returnedBytes = deserialize(buffer, offset, count);
			if (returnedBytes == 0)
				return 0;

			if (count == 0)
				return returnedBytes;

			size_t offsetTemp = offset + returnedBytes;

			for (size_t i = 0; i < count; i++)
			{
				/** получаем объект */
				std::pair<KT, VT> dataTemp = {};
				const auto returnedBytesTemp = deserialize(
					buffer, offsetTemp, dataTemp);
				if (returnedBytesTemp == 0)
					return 0;

				offsetTemp += returnedBytesTemp;
				returnedBytes += returnedBytesTemp;

				/** перемещаем объект в таблицу */
				data.insert(std::move(dataTemp));
			}

			return returnedBytes;
		}
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		*/
		template <class T>
		static void serialize(
			std::vector<BYTE>& buffer,
			const T& data)
		{
			serializable<T>::serialize(buffer, data);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		* @param args - переводимые данные.
		*/
		template <class... Args, class T>
		static void serialize(
			std::vector<BYTE>& buffer,
			const T& data,
			const Args&... args)
		{
			ISerializable::serialize(buffer, data);
			ISerializable::serialize(buffer, args...);		
		}
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param args - переводимые данные.
		*/
		template <class... Args>
		static std::vector<BYTE> serializeReturn(
			const Args&... args)
		{
			std::vector<BYTE> buffer;
			serialize(buffer, args...);
			return buffer;
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @return - количество переведенных байт.
		*/
		template <class T>
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			T& data)
		{
			return serializable<T>::deserialize(buffer, offset, data);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @param args - переводимые данные.
		* @return - количество переведенных байт.
		*/
		template <class... Args, class T>
		static size_t deserialize(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			T& data,
			Args&... args)
		{
			const auto sizeData = ISerializable::deserialize(
				buffer, offset, data);

			/** ошибка, необходимо выйти */
			if (sizeData == 0)
				return 0;

			const auto sizeArg = ISerializable::deserialize(
				buffer, offset + sizeData, args...);

			/** ошибка, необходимо выйти */
			if (sizeArg == 0)
				return 0;

			return sizeArg + sizeData;
		}
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		virtual ~ISerializable() = default;
	//--------------------------------------------------------------------------
		ISerializable(const ISerializable&) = delete;
		ISerializable(ISerializable&&) = delete;
		virtual ISerializable& operator=(const ISerializable&) = default;
		virtual ISerializable& operator=(ISerializable&&) = default;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Method
	private:
	//--------------------------------------------------------------------------
		/**
		* перевод данных в байтовый массив.
		* @param buffer - буфер для добавления.
		* @param data - переводимые данные.
		* @param is - список параметров.
		*/
		template <class Tuple, size_t... Indices>
		static void serializeTuple(
			std::vector<BYTE>& buffer,
			const Tuple& data,
			std::index_sequence<Indices...>)
		{
			serialize(buffer, std::get<Indices>(data)...);
		}
	//--------------------------------------------------------------------------
		/**
		* перевод байтового массива в данные.
		* @param buffer - байтовый массив.
		* @param offset - смещение текущего перевода.
		* @param data - переводимые данные.
		* @param is - список параметров.
		* @return - количество переведенных байт.
		*/
		template <class Tuple, size_t... Indices>
		static size_t deserializeTuple(
			const std::vector<BYTE>& buffer,
			const size_t offset,
			Tuple& data,
			std::index_sequence<Indices...>)
		{
			return deserialize(buffer, offset, std::get<Indices>(data)...);
		}
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}
#pragma warning (default: 26415 26418 26481)