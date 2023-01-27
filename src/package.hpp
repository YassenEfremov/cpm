#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>


namespace cpm {

	/**
	 * @brief A class representing a cpm package
	 */
	class Package {

		public:

		/**
		 * @brief Constructor for cpm package
		 * 
		 * @param name the package name
		 */
		Package(const std::string &name);

		/**
		 * @brief Getter for name
		 * 
		 * @return The name of the package
		*/
		const std::string &get_name() const;


		private:

		std::string name;
	};
}


#endif	// PACKAGE_H

