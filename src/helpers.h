#include <fstream>
#include <sstream>
#include <chrono>
#include <stdlib.h>

#include <openssl/ssl.h>
#include <openssl/crypto.h>

/* Result codes */
#define OK           0
#define NOT_VALID   -1

namespace helpers
{
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

	std::string get_token(std::string& s, std::string_view delimeter) {
		int pos = s.find(delimeter) + delimeter.length();
		int last = s.find("&", pos);
		last = (last != std::string::npos) ? last : s.length();
		
		return std::string(s.cbegin() + pos, s.cbegin() + last);
	}

	std::string return_file(const char* path) {
		std::ifstream t(path);
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	}

	int validate_isbn(std::string& isbn) {
		std::vector<int> digits;
		for (auto &i : isbn) {
			if (i >= 48 && i <= 57)
				digits.push_back(int(i));
		}
		
		/* 13 digit isbn */
		if (digits.size() == 13) {
			long sum = 0;
			for (auto &d : digits) {
				if (d % 2 == 0)
					sum += d * 3;
				else
					sum += d;
			}
			if (sum / 10 == 0)
				return OK;
			else return NOT_VALID;
		}
		/* 10 digit isbn */
		else if (digits.size() == 10) {
			long sum = 0;
			int i = 1;
			for (auto &d : digits) {
				sum += d + i++;
			}
			if (sum / 11 == 0)
				return OK;
			else return NOT_VALID;
		}
	}

	std::string gen_fake_isbn() {
		/* generate 14 digit random number */
		srand(time(NULL));
		return std::to_string(rand() % 10^14);
	}
} // namespace h
