#ifndef PACKAGE_H
#define PACKAGE_H

#include <filesystem>
#include <string>

namespace fs = std::filesystem;


namespace cpm {

	/**
	 * @brief A class representing a cpm package
	 */
	class Package {

		fs::path url;
		std::string name;


		public:

		Package(const fs::path &url);

		/**
		 * @brief Getter for name
		 * 
		 * @return The name of the package
		*/
		const fs::path &get_url() const;
		const std::string &get_name() const;
	};
}


#endif	// PACKAGE_H

