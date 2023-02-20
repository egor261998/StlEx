#pragma once

namespace stlex::misc
{
	/** хранилище пути */
	class STLEX CPathStorage
	{
	#pragma region Public_Method
	public:
	//--------------------------------------------------------------------------
		/**
		* конструктор.
		* @param pipe - путь.
		*/
		CPathStorage(
			const std::filesystem::path& path);
	//--------------------------------------------------------------------------
		/**
		* получить путь до pipe.
		* @return - путь до pipe.
		*/
		const std::filesystem::path& getPath() const noexcept;
	//--------------------------------------------------------------------------
		/**
		* деструктор.
		*/
		virtual ~CPathStorage() = default;
	//--------------------------------------------------------------------------
		CPathStorage(const CPathStorage&) = delete;
		CPathStorage(CPathStorage&&) = delete;
		CPathStorage& operator=(const CPathStorage&) = delete;
		CPathStorage& operator=(CPathStorage&&) = delete;
	//--------------------------------------------------------------------------
	#pragma endregion

	#pragma region Private_Data
	private:
	//--------------------------------------------------------------------------
		/** путь */
		const std::filesystem::path _path;
	//--------------------------------------------------------------------------
	#pragma endregion
	};
}