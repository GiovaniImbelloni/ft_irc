#include "Utils.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

void sendMessage(int fd, const std::string& message) {
    if (send(fd, message.c_str(), message.size(), 0) == -1) {
        std::cerr << "Failed to send message to client (fd: " << fd << ") - " << strerror(errno) << std::endl;
    }
}

std::vector<std::string> split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        token = str.substr(start, end - start);
        tokens.push_back(token);
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }

    token = str.substr(start);
    tokens.push_back(token);

    return tokens;
}

bool isValidPassword(const std::string& password) {
    if (password.empty())
        return false;

    if (password.length() > 32)
        return false;

    for (size_t i = 0; i < password.length(); ++i) {
        if (std::isspace(password[i]))
            return false;
    }

    return true;
}