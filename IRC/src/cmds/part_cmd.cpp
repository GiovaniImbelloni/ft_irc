#include "Server.hpp"
#include "Utils.hpp"

void Server::part_cmd(Client& client, int clientFd, std::vector<std::string> params) {
	if (params.empty()) {
		sendMessage(clientFd, "461 PART :Not enough parameters\r\n");
		return;
	}

	std::string channelNames = params[0];
	std::string partReason = "Leaving";
	
	if (params.size() >= 2) {
		partReason = params[1];
		for (size_t i = 2; i < params.size(); ++i) {
			partReason += " " + params[i];
		}
	}

	std::vector<std::string> channels = split(channelNames, ",");
	for (size_t i = 0; i < channels.size(); ++i) {
		std::string channelName = channels[i];
		
		if (!isChannelExist(channelName)) {
			sendMessage(clientFd, "403 " + client.getNickName() + " " + channelName + " :No such channel\r\n");
			continue;
		}

		Channel& channel = channels_[channelName];

		if (!channel.isMember(client)) {
			sendMessage(clientFd, "442 " + client.getNickName() + " " + channelName + " :You're not on that channel\r\n");
			continue;
		}

		std::string partMsg = ":" + client.getNickName() + " PART " + channelName + " :" + partReason + "\r\n";
		sendToChannel(channelName, partMsg);

		channel.removeMember(client);
		channel.removeOperator(client);
		channel.removeInvited(client);

		if (channel.isEmpty()) {
			deleteChannel(channelName);
		}
	}
}