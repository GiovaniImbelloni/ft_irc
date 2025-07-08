#include "Server.hpp"
#include "Utils.hpp"

void Server::privmsg_cmd(Client &client, int clientFd, std::vector<std::string> params) {
	if (params.empty()) {
		sendMessage(clientFd, "411 " + client.getNickName() + " :No recipient given (PRIVMSG)\r\n");
		return;
	}
	if (params.size() < 2) {
		sendMessage(clientFd, "412 " + client.getNickName() + " :No text to send\r\n");
		return;
	}

	std::string target = params[0];
	std::string message = params[1];
	for (size_t i = 2; i < params.size(); ++i) {
		message += " " + params[i];
	}

	if (target[0] == '#') {
		if (!isChannelExist(target)) {
			sendMessage(clientFd, "403 " + client.getNickName() + " " + target + " :No such channel\r\n");
			return;
		}

		Channel& channel = channels_[target];
		if (!channel.isMember(client)) {
			sendMessage(clientFd, "404 " + client.getNickName() + " " + target + " :Cannot send to channel\r\n");
			return;
		}

		std::string privMsg = ":" + client.getNickName() + " PRIVMSG " + target + " :" + message + "\r\n";
		sendToChannelExcept(channel, client, privMsg);
	} else {
		Client* targetClient = findClientByNick(target);
		if (!targetClient) {
			sendMessage(clientFd, "401 " + client.getNickName() + " " + target + " :No such nick/channel\r\n");
			return;
		}

		std::string privMsg = ":" + client.getNickName() + " PRIVMSG " + target + " :" + message + "\r\n";
		sendMessage(targetClient->getClientFd(), privMsg);
	}
}