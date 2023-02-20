#pragma once

namespace stlex::misc
{
	/** элемент потокобезопасного списка */
	class STLEX CInterlockedItem : 
		public SLIST_ENTRY { };

	/** потокобезопасный односвязный список */
	template <class T>
	class STLEX CInterlockedList final
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор.
		*/
		CInterlockedList() noexcept
		{
			InitializeSListHead(&_listHead);
		}
	//--------------------------------------------------------------------------
		/**
		* получить количество элементов в списке.
		* возвращаемое значение подвержено мгновенному изменению.
		* @return - количество элементов.
		*/
		size_t size() const noexcept
		{
			return _nCount;
		}
	//--------------------------------------------------------------------------
		/**
		* очистить список.
		*/
		void clear() noexcept
		{
			while (true)
			{
				auto pInterlockedItem = popEx();

				if (pInterlockedItem == nullptr)
					return;
			}
		}
	//--------------------------------------------------------------------------
		/**
		* извлечь первый элемент списка.
		* @return - элемент списка.
		*/
		T* pop() noexcept
		{
		/**
		* 26481 - арифметика указателей.
		*/
		#pragma warning (disable: 26481)
			auto pItemEntry = InterlockedPopEntrySList(&_listHead);

			if (pItemEntry == nullptr)
				return nullptr;

			_nCount--;

			return CONTAINING_RECORD(pItemEntry, T, Next);
		#pragma warning (default: 26481)
		}
	//--------------------------------------------------------------------------
		/**
		* извлечь первый элемент списка как умный указатель.
		* @return - элемент списка как умный указатель.
		*/
		std::unique_ptr<T> popEx() noexcept
		{
			return std::unique_ptr<T>(pop());
		}
	//--------------------------------------------------------------------------
		/**
		* вставить элемент в начало списка.
		* @param pInterlockedItem - новый элемент списка.
		*/
		void push(
			T* pInterlockedItem) noexcept
		{
			InterlockedPushEntrySList(&_listHead,
				pInterlockedItem);

			_nCount++;
		}
	//--------------------------------------------------------------------------
		/**
		* вставить элемент в начало списка из умного указателя.
		* @param pInterlockedItem - новый элемент списка как умный указатель.
		*/
		void pushEx(
			std::unique_ptr<T>&& pInterlockedItem) noexcept
		{
			push(pInterlockedItem.release());
		}
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CInterlockedList()
		{
			clear();
		}
	//--------------------------------------------------------------------------
		CInterlockedList(const CInterlockedList&) = delete;
		CInterlockedList(CInterlockedList&&) = delete;
		CInterlockedList& operator=(const CInterlockedList&) = delete;
		CInterlockedList& operator=(CInterlockedList&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** голова списка */
		SLIST_HEADER _listHead;

		/** количество элементов в списке */
		std::atomic_size_t _nCount = 0;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}