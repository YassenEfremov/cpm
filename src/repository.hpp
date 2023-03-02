#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <filesystem>
#include <unordered_set>

namespace fs = std::filesystem;


namespace cpm {

/**
 * @brief A class representing an object repository
 * 
 * @tparam T the type of objects to store
 */
template<typename T, typename P>
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
	 * @brief Getter for filename
	 * 
	 * @return The name of the repository file
	 */
	const fs::path &get_filename() const { return this->filename; }

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
	virtual std::unordered_set<T, P> list() const = 0;

	/**
	 * @brief Check if the specified object is in the repository
	 * 
	 * @param object the object
	 * 
	 * @return true if the object is found, false otherwise
	 */
	virtual bool contains(const T &object) const = 0;


	protected:

	fs::path filename;
};

} // namespace cpm


#endif	// REPOSITORY_H
