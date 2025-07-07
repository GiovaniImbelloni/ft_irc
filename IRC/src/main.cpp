#include "../include/Server.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cctype>

struct Config {
	int port;
	std::string password;
};

bool isNumeric(const std::string& str) {
	for (size_t i = 0; i < str.length(); ++i) {
		if (!std::isdigit(static_cast<unsigned char>(str[i])))
			return false;
	}
	return true;
}

bool hasWhitespace(const std::string& str) {
	for (size_t i = 0; i < str.length(); ++i) {
		if (std::isspace(static_cast<unsigned char>(str[i])))
			return true;
	}
	return false;
}

bool parseArguments(int argc, char** argv, Config& config) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return false;
	}

	std::string portStr(argv[1]);
	std::string password(argv[2]);

	if (!isNumeric(portStr)) {
		std::cerr << "Error: Port must be numeric." << std::endl;
		return false;
	}

	std::istringstream iss(portStr);
	int port;
	if (!(iss >> port) || port < 1 || port > 65535) {
		std::cerr << "Error: Port must be between 1 and 65535." << std::endl;
		return false;
	}

	if (password.empty() || password.length() > 20 || hasWhitespace(password)) {
		std::cerr << "Error: Password must be 1–20 characters and contain no spaces." << std::endl;
		return false;
	}

	config.port = port;
	config.password = password;
	return true;
}

int main(int argc, char** argv) {
	Config config;
	if (!parseArguments(argc, argv, config)) {
		return 1;
	}

	try {
		Server server(config.port, config.password);
		server.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
