#ifndef UTIL_H
#define UTIL_H

#include <string>


namespace cpm::util {

	/**
	 * @brief Decode a base64 encoded string
	 * 
	 * @param intput a base64 encoded string
	 * 
	 * @return The decoded string
	 */
    std::string base64_decode(const std::string &input);
}


#endif	// UTIL_H
