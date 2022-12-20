#ifndef INSTALL_H
#define INSTALL_H

#include <string>


namespace cpm {

	/**
	 * @brief Install the given package/s
	 * 
	 * @param package URL to a GitHub repository
	 */
	void install(const std::string &package);
}


#endif	// INSTALL_H
