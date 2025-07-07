#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <stdlib.h>
#include <iostream>

# include <iostream>
# include <string>
# include <vector>

void sendMessage(int fd, const std::string& message);
std::vector<std::string> split(const std::string& str, const std::string& delimiter);
bool isValidPassword(const std::string& password);

#endif
