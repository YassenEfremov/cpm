#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>


namespace cpm::util {

/**
 * @brief Decode a base64 encoded string
 * 
 * @param intput a base64 encoded string
 * 
 * @return The decoded string
 */
std::string base64_decode(const std::string &input);

/**
 * @brief Split the specified string into tokens by the given delimeter
 * 
 * @param str the string to be split
 * @param delim the delimiting string
 * 
 * @return A list of tokens
 */
std::vector<std::string> split_string(const std::string &str,
									  const std::string &delim);

} // namespace cpm::util


#endif	// UTIL_H
