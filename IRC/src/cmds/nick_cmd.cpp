#include "Server.hpp"
#include "Utils.hpp"

void Server::nick_cmd(Client& client, int clientFd, std::vector<std::string> params) {
	if (params.empty()) {
		sendMessage(clientFd, "431 :No nickname given\r\n");
		return;
	}

	std::string newNick = params[0];
	std::string oldNick = client.getNickName();

	if (isNicknameInUse(newNick)) {
		sendMessage(clientFd, "433 " + newNick + " :Nickname is already in use\r\n");
		return;
	}

	client.setNickName(newNick);

	if (!oldNick.empty() && oldNick != newNick) {
		std::string msg = ":" + oldNick + " NICK :" + newNick + "\r\n";
		sendMessage(clientFd, msg);

		std::map<std::string, Channel>::iterator it;
		for (it = channels_.begin(); it != channels_.end(); ++it) {
			Channel& channel = it->second;
			if (channel.isMember(client)) {
				sendToChannelExcept(channel, client, msg);
			}
		}
	}
	else if (oldNick.empty()) {
		std::string msg = ":" + newNick + " NICK :" + newNick + "\r\n";
		sendMessage(clientFd, msg);
	}
	tryAuthenticate(client, clientFd);
}

