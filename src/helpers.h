#include <fstream>
#include <sstream>
#include <chrono>

#include <openssl/ssl.h>
#include <openssl/crypto.h>


void urldecode(std::string& dst, const char *src) {
	int hex1, hex2;
	while (*src) {
		if ((*src == '%') && ((hex1 = src[1]) && (hex2 = src[2])) && (isxdigit(hex1) && isxdigit(hex2))) {
			hex1 -= '0';
			if (hex1 > 9)
				hex1 -= 7;
			hex2 -= '0';
			if (hex2 > 9)
				hex2 -= 7;

			dst.push_back(16*hex1+hex2);
			src+=3;
		} else {
			dst.push_back(*src++);
		}
	}
	if (dst.back() == '\r')
		dst.pop_back();
}

std::string_view get_token(std::string& s, std::string_view delimeter) {
	int pos = s.find(delimeter) + delimeter.length();
	int last = s.find("&", pos);
	last = (last != std::string::npos) ? last : s.length();
	
	return std::string_view(s.cbegin() + pos, s.cbegin() + last);
}

std::string return_file(const char* path) {
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}