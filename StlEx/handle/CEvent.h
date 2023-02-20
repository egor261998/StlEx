#pragma once

namespace stlex::handle
{
	/** реализация событий в классе */
	class STLEX CEvent final : 
		public CHandle
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор по умолчанию.
		* @param bIsCreateEvent - признак создания события.
		*/
		CEvent(
			const bool bIsCreateEvent = true) noexcept(false);
	//--------------------------------------------------------------------------
		/**
		* конструктор с присвоением имени события.
		* @param name - имя объекта.
		* @param bIsCreateEvent - признак создания события.
		*/
		CEvent(
			const std::wstring_view name,
			const bool bIsCreateEvent = false);
	//--------------------------------------------------------------------------
		/**
		* конструктор копирования.
		* @param handle - объект копирования.
		*/
		CEvent(
			const CHandle& handle) noexcept;
	//--------------------------------------------------------------------------
		/**
		* конструктор перемещения.
		* @param handle - объект перемещения.
		*/
		CEvent(
			CHandle&& handle) noexcept;
	//--------------------------------------------------------------------------
		/**
		* дать уведомление.
		* @return - код ошибки.
		*/
		std::error_code notify() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* отменить уведомление.
		* @return - код ошибки.
		*/
		std::error_code cancel() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* оператор копирования объекта.
		* @param handle - копируемый объект.
		* @return - текущий объект.
		*/
		CEvent& operator=(
			const CHandle& handle) noexcept override;
	//--------------------------------------------------------------------------
		/**
		* оператор перемещения объекта.
		* @param handle - перемещаемый объект.
		* @return - текущий объект.
		*/
		CEvent& operator=(
			CHandle&& handle) noexcept override;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		~CEvent();
	//--------------------------------------------------------------------------
		CEvent(const CEvent&) noexcept = default;
		CEvent(CEvent&&) noexcept = default;
		CEvent& operator=(const CEvent&) noexcept = default;
		CEvent& operator=(CEvent&&) noexcept = default;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}