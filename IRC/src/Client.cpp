#include "Client.hpp"

Client::Client() : clientFd_(-1), nickName_(""), userName_(""), clientAddress_(""), realName_(""), \
					isRegisteredWithPass_(false), isAuthenticated_(false), modeInvisible_(false) {}

Client::~Client() {}


int Client::getClientFd() const {
	return clientFd_;
}

int	Client::getClientPort() const {
	return clientPort_;
}

std::string Client::getNickName() const {
	return nickName_;
}

std::string Client::getUserName() const {
	return userName_;
}

std::string Client::getClientAddress() const {
	return clientAddress_;
}

std::string Client::getRealName() const {
	return realName_;
}

bool	Client::getIsRegistratedWithPass() const {
	return isRegisteredWithPass_;
}

bool Client::getIsAuthenticated() const {
	return isAuthenticated_;
}

bool Client::getModeInvisible() const {
	return modeInvisible_;
}

void Client::setNickName(const std::string& nickName) {
	nickName_ = nickName;
}

void Client::setClientPort(int port) {
    clientPort_ = port;
}

void Client::setIsAuthenticated(bool authenticated) {
	isAuthenticated_ = authenticated;
}

void Client::setUserName(const std::string& userName) {
	userName_ = userName;
}

void Client::setClientAddress(const std::string& clientAddress) {
	clientAddress_ = clientAddress;
}

void Client::setRealName(const std::string& realName) {
	realName_ = realName;
}

void Client::setIsRegisteredWithPass(bool registeredWithPass) {
	isRegisteredWithPass_ = registeredWithPass;
}

void Client::setClientFd(int clientFd) {
	clientFd_ = clientFd;
}


void Client::setModeInvisible(bool modeInvisible) {
	modeInvisible_ = modeInvisible;
}


