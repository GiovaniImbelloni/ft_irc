#include "Server.hpp"
#include "Utils.hpp"

void Server::names_cmd(Client &client, int clientFd, std::vector<std::string> params) {
	if (params.empty()) {
		for (std::map<std::string, Channel>::iterator it = channels_.begin(); it != channels_.end(); ++it) {
			Channel& channel = it->second;
			if (channel.isMember(client)) {
				showMembersList(client, channel);
			}
		}
	} else {
		for (size_t i = 0; i < params.size(); ++i) {
			std::string channelName = params[i];
			if (isChannelExist(channelName)) {
				Channel& channel = channels_[channelName];
				if (channel.isMember(client)) {
					showMembersList(client, channel);
				} else {
					showMembersList(client, channel);
				}
			} else {
				sendMessage(clientFd, "366 " + client.getNickName() + " " + channelName + " :End of /NAMES list.\r\n");
			}
		}
	}
}