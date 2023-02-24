#ifndef SEMVER_H
#define SEMVER_H

#include <string>


namespace cpm {

	/**
	 * @brief A class representing a generic semantic version
	 */
	class SemVer {

		public:

		/**
		 * @brief Default constructor for generic semantic verion
		 */
		SemVer();

		/**
		 * @brief Constructor for generic semantic verion
		 * 
		 * @param major the major version
		 * @param minor the minor version
		 * @param patch the patch version
		 * @param suffix a suffix (alpha, beta, rc-1, etc.)
		 */
		SemVer(int major, int minor, int patch, const std::string &suffix);

		/**
		 * @brief Constructor for generic semantic verion
		 * 
		 * @param version_str a semantic version as a string
		 */
		SemVer(const std::string &version_str);

		/**
		 * @brief Getter for major version
		 * 
		 * @return The major version
		 */
		int get_major() const;

		/**
		 * @brief Getter for minor version
		 * 
		 * @return The minor version
		 */
		int get_minor() const;
		
		/**
		 * @brief Getter for patch version
		 * 
		 * @return The patch version
		 */
		int get_patch() const;
		
		/**
		 * @brief Getter for suffix
		 * 
		 * @return The suffix
		 */
		std::string get_suffix() const;

		/**
		 * @brief Get the semantic version as a string
		 * 
		 * @return The semantic version as a string
		 */
		std::string string() const;

		/**
		 * @brief Check if the version is different from the default one
		 * 
		 * @return true if the version is not the default one, false otherwise
		*/
		bool is_specified();

		friend bool operator<(const SemVer &lhs, const SemVer &rhs);
		friend bool operator>(const SemVer &lhs, const SemVer &rhs);
		friend bool operator<=(const SemVer &lhs, const SemVer &rhs);
		friend bool operator>=(const SemVer &lhs, const SemVer &rhs);

		friend bool operator==(const SemVer &lhs, const SemVer &rhs);
		friend bool operator!=(const SemVer &lhs, const SemVer &rhs);


		private:

		int major;
		int minor;
		int patch;
		std::string suffix;
	};
}


#endif	// SEMVER_H
