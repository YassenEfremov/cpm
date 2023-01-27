#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


namespace cpm {

	/**
	 * @brief A class representing an object repository
	 * 
	 * @tparam T the type of objects to store
	 */
	template<typename T>
	class Repository {

		public:

		/**
		 * @brief Constructor for repository
		 * 
		 * @param filename the name of the file to store the repository
		 */
		Repository(const fs::path &filename) : filename(filename) {}
		virtual ~Repository() = default;

		/**
		 * @brief Add the specified objects/s to the repository
		 * 
		 * @param object the object to add
		 * 
		 * @return The number of records modified in the repository
		 */
		virtual int add(const T &object) = 0;

		/**
		 * @brief Remove the specified object/s from the repository
		 * 
		 * @param object the object to remove
		 * 
		 * @return The number of records modified in the repository
		 */
		virtual int remove(const T &object) = 0;

		/**
		 * @brief List all of the objects in the repository
		 * 
		 * @return A list of all the objects in the repository
		 */
		virtual std::vector<T> list() = 0;


		protected:

		fs::path filename;
	};
}


#endif	// REPOSITORY_H

