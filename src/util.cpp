#include "util.hpp"

#include <string>
#include <vector>


namespace cpm::util {

    std::string base64_decode(const std::string &in) {

        std::string out;

        std::vector<int> T(256,-1);
        for (int i=0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

        int val=0, valb=-8;
        for (unsigned char c : in) {
            if (c == '\n') continue;
            if (T[c] == -1) break;
            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0) {
                out.push_back(char((val>>valb)&0xFF));
                valb -= 8;
            }
        }
        return out;
    }

 	std::vector<std::string> split_string(const std::string &str,
								          const std::string &delim) {

		std::vector<std::string> tokens;

		std::string leftover = str;
		
		while (true) {
			int delim_pos = leftover.find(delim);
			if (delim_pos == std::string::npos) {
                if (leftover.empty() && tokens.size() > 0) break;
    			tokens.push_back(leftover);
				break;
			}
			std::string next_token = leftover.substr(0, delim_pos);
			leftover = leftover.substr(delim_pos + delim.length());
            if (next_token.empty()) continue;
		    tokens.push_back(next_token);
		}

		return tokens;
	}
}
