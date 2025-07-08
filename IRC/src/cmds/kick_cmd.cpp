#include "Server.hpp"
#include "Utils.hpp"

void Server::kick_cmd(Client &client, int clientFd, std::vector<std::string> params) {
	if (params.size() < 2) {
		sendMessage(clientFd, "461 KICK :Not enough parameters\r\n");
		return;
	}

	std::string channelName = params[0];
	std::string targetNick = params[1];
	std::string reason = "No reason given";
	if (params.size() >= 3) {
		reason = params[2];
		for (size_t i = 3; i < params.size(); ++i) {
			reason += " " + params[i];
		}
	}

	if (!isChannelExist(channelName)) {
		sendMessage(clientFd, "403 " + client.getNickName() + " " + channelName + " :No such channel\r\n");
		return;
	}

	Channel& channel = channels_[channelName];
	if (!channel.isMember(client)) {
		sendMessage(clientFd, "442 " + client.getNickName() + " " + channelName + " :You're not on that channel\r\n");
		return;
	}

	if (!channel.isOperator(client)) {
		sendMessage(clientFd, "482 " + client.getNickName() + " " + channelName + " :You're not channel operator\r\n");
		return;
	}

	Client* targetClient = findClientByNick(targetNick);
	if (!targetClient) {
		sendMessage(clientFd, "401 " + client.getNickName() + " " + targetNick + " :No such nick\r\n");
		return;
	}

	if (!channel.isMember(*targetClient)) {
		sendMessage(clientFd, "441 " + client.getNickName() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n");
		return;
	}

	std::string kickMsg = ":" + client.getNickName() + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";

	sendToChannel(channelName, kickMsg);

	channel.removeMember(*targetClient);
	channel.removeOperator(*targetClient);
	channel.removeInvited(*targetClient);

	if (channel.isEmpty()) {
		deleteChannel(channelName);
	}
}