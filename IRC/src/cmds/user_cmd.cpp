#include "Server.hpp"
#include "Utils.hpp"

void Server::user_cmd(Client &client, int clientFd, std::vector<std::string> params) {
	if (!client.getUserName().empty()) {
		sendMessage(clientFd, "462 :You may not reregister\r\n");
		return;
	}

	if (params.empty()) {
		sendMessage(clientFd, "461 USER :Not enough parameters\r\n");
		return;
	}

	std::string username = params[0];
	client.setUserName(username);

	std::string realname;

	if (params.size() >= 4) {
		if (!params[3].empty() && params[3][0] == ':') {
			realname = params[3].substr(1);
			for (size_t i = 4; i < params.size(); ++i) {
				realname += " " + params[i];
			}
		} else {
			realname = params[3];
		}
	} else {
		realname = "";
	}

	client.setRealName(realname);

	tryAuthenticate(client, clientFd);
}
