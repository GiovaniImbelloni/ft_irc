#include "Server.hpp"
#include "Utils.hpp"

void	Server::pass_cmd(Client &client, int clientFd, std::vector<std::string> params) {
	if (client.getIsRegistratedWithPass()) {
		sendMessage(clientFd, "462 :You may not reregister\r\n");
		return;
	}

	if (params.empty()) {
		sendMessage(clientFd, "461 PASS :Not enough parameters\r\n");
		return;
	}

	std::cout << "Received password: " << params.front() << std::endl;
	std::cout << "Params size: " << params.size() << std::endl;

	if (params[0] != password_) {
		sendMessage(clientFd, "464 :Password incorrect\r\n");
		return;
	}

	client.setIsRegisteredWithPass(true);
	sendMessage(clientFd, ":Password accepted\r\n");

	tryAuthenticate(client, clientFd);
}