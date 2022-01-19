#include "common.h"

#include "openssl/rand.h"
#include "openssl/err.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>

void UrlDecode(std::string& dst, const char *src) {
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

std::string GetToken(std::string& s, std::string_view delimeter) {
	int pos = s.find(delimeter) + delimeter.length();
	int last = s.find("&", pos);
	last = (last != std::string::npos) ? last : s.length();
	
	return std::string(s.cbegin() + pos, s.cbegin() + last);
}

std::string ReturnFile(const char* path) {
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

int ValidateIsbn(std::string& isbn) {
	std::vector<int> digits;
	for (auto &i : isbn) {
		if (i >= 48 && i <= 57)
			digits.push_back(int(i) - '0');
	}

	/* 13 digit isbn */
	if (digits.size() == 13) {
		long sum = 0;
		int i = 1;
		for (auto &d : digits) {
			if (i % 2 == 0)
				sum += d * 3;
			else
				sum += d;
			i++;
		}
		if (sum % 10 == 0)
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
	log::error("wrong isbn");
	return EINVAL;
}

std::string GenFakeIsbn() {
	/* generate 14 digit random number */
	srand(time(NULL));
	long long r1 = rand();
	long long r2 = rand();
	long long max = pow(10, 14);
	return std::to_string((r1 * r2) % max);
}

std::string GenerateRandomString() {
	unsigned char buffer[128];

	int rc = RAND_bytes(buffer, sizeof(buffer));
	unsigned long err = ERR_get_error();

	if(rc != 1) {
		/* RAND_bytes failed */
		/* `err` is valid    */
	} else {
		return std::string(reinterpret_cast<char*>(buffer));
	}
}

std::string GetHostIp() {
	// char host[256];
	// gethostname(host, 256);

	// struct hostent *host_entry;
	// host_entry = gethostbyname(host);

	int fd;
	struct ifreq ifr;
	
    // replace with your interface name
    // or ask user to input
    
	char iface[] = "eth0";
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = AF_INET;

	//Copy the interface name in the ifreq structure
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	//display result
	printf("%s - %s\n" , iface , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );

	std::string result = inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
	return result;
}