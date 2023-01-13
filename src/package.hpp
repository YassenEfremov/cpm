#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>


namespace cpm {

	/**
	 * @brief A class representing a cpm package
	 */
	class Package {

		std::string name;


		public:

		Package(const std::string &name);

		/**
		 * @brief Getter for name
		 * 
		 * @return The name of the package
		*/
		const std::string &get_name() const;
	};
}


#endif	// PACKAGE_H

