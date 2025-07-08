#include "Server.hpp"
#include "Utils.hpp"

void Server::invite_cmd(Client &client, int clientFd, std::vector<std::string> params) {
	if (params.size() < 2) {
		sendMessage(clientFd, "461 INVITE :Not enough parameters\r\n");
		return;
	}

	std::string targetNick = params[0];
	std::string channelName = params[1];
	if (!isChannelExist(channelName)) {
		sendMessage(clientFd, "403 " + client.getNickName() + " " + channelName + " :No such channel\r\n");
		return;
	}

	Channel& channel = channels_[channelName];
	if (!channel.isMember(client)) {
		sendMessage(clientFd, "442 " + client.getNickName() + " " + channelName + " :You're not on that channel\r\n");
		return;
	}

	if (channel.getInviteMode() && !channel.isOperator(client)) {
		sendMessage(clientFd, "482 " + client.getNickName() + " " + channelName + " :You're not channel operator\r\n");
		return;
	}

	Client* targetClient = findClientByNick(targetNick);
	if (!targetClient) {
		sendMessage(clientFd, "401 " + client.getNickName() + " " + targetNick + " :No such nick/channel\r\n");
		return;
	}

	if (channel.isMember(*targetClient)) {
		sendMessage(clientFd, "443 " + client.getNickName() + " " + targetNick + " " + channelName + " :is already on channel\r\n");
		return;
	}

	channel.insertInvited(*targetClient);

	std::string inviteMsg = ":" + client.getNickName() + " INVITE " + targetNick + " " + channelName + "\r\n";
	sendMessage(targetClient->getClientFd(), inviteMsg);

	sendMessage(clientFd, "341 " + client.getNickName() + " " + targetNick + " " + channelName + "\r\n");
}