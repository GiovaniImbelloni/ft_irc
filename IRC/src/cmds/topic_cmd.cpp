#include "Server.hpp"
#include "Utils.hpp"

void Server::topic_cmd(Client &client, int clientFd, std::vector<std::string> params) {
	if (params.empty()) {
		sendMessage(clientFd, "461 TOPIC :Not enough parameters\r\n");
		return;
	}

	std::string channelName = params[0];
	if (!isChannelExist(channelName)) {
		sendMessage(clientFd, "403 " + client.getNickName() + " " + channelName + " :No such channel\r\n");
		return;
	}

	Channel& channel = channels_[channelName];
	if (!channel.isMember(client)) {
		sendMessage(clientFd, "442 " + client.getNickName() + " " + channelName + " :You're not on that channel\r\n");
		return;
	}

	if (params.size() == 1) {
		if (channel.getTopic().empty()) {
			sendMessage(clientFd, "331 " + client.getNickName() + " " + channelName + " :No topic is set\r\n");
		} else {
			sendMessage(clientFd, "332 " + client.getNickName() + " " + channelName + " :" + channel.getTopic() + "\r\n");
		}
		return;
	}

	if (channel.getTopicMode() && !channel.isOperator(client)) {
		sendMessage(clientFd, "482 " + client.getNickName() + " " + channelName + " :You're not channel operator\r\n");
		return;
	}

	std::string newTopic = params[1];
	for (size_t i = 2; i < params.size(); ++i) {
		newTopic += " " + params[i];
	}

	channel.setTopic(newTopic);

	std::string topicMsg = ":" + client.getNickName() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
	sendToChannel(channelName, topicMsg);
}