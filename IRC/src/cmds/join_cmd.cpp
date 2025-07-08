#include "Server.hpp"
#include "Utils.hpp"

void Server::join_cmd(Client& client, int clientFd, std::vector<std::string> params) {
	if (params.empty()) {
		sendMessage(clientFd, ":server 461 JOIN :Not enough parameters\r\n");
		return;
	}

	std::string channelName = params[0];
	if (channelName == "0" && params.size() == 1) {
		leaveAllChannels(client);
		sendMessage(clientFd, ":server 331 " + client.getNickName() + " :You have left all channels\r\n");
		return;

	}

	if (channelName == "0" && params.size() > 1) {
		sendMessage(clientFd, "461 JOIN :Too many parameters");
		return;

	}

	if (params.size() > 2) {
		sendMessage(clientFd, "461 JOIN :Too many parameters");
		return;

	}

	if (!isChannelNameValid(channelName, clientFd)) {
		return;

	}

	if (params.size() == 1) {
		joinChannelWithoutPassword(client, clientFd, channelName);
	} else {
		joinChannelWithPassword(client, clientFd, channelName, params[1]);
	}
}

bool Server::isChannelNameValid(const std::string& name, int clientFd) {
	if (name.empty() || (name[0] != '#' && name[0] != '&')) {
		sendMessage(clientFd, ":server 476 " + name + " :Invalid channel name\r\n");
		return false;
	}
	if (name.size() > 50) {
		sendMessage(clientFd, ":server 405 " + name + " :Channel name is too long\r\n");
		return false;
	}

	return true;
}

void Server::joinChannelWithoutPassword(Client &client, int clientFd, const std::string &channelName) {
	if (isChannelExist(channelName)) {
		Channel& channel = channels_[channelName];
		if (channel.isMember(client)) {
			sendMessage(clientFd, "443 " + client.getNickName() + " " + channel.getChannelName() + " :is already on channel\r\n");
			return;

		} else if (!channel.getPasswordMode()) {
			handleChannelJoin(client, clientFd, channel);

		} else {
			sendMessage(clientFd, ":server 475 " + channelName + " :Cannot join channel (+k)\r\n");
		}
	} else {
		createChannel(channelName, client);

		std::string joinMsg = ":" + client.getNickName() + " JOIN " + channelName + "\r\n";
		sendMessage(clientFd, joinMsg);
		sendMessage(clientFd, buildWelcomeMessage(channels_[channelName], client));
		showMembersList(client, channels_[channelName]);
	}
}

void Server::joinChannelWithPassword(Client &client, int clientFd, const std::string &channelName, const std::string &password) {
	if (isChannelExist(channelName)) {
		Channel& channel = channels_[channelName]; 
		if (channel.isMember(client)) {
			sendMessage(clientFd, "443 " + client.getNickName() + " " + channel.getChannelName() + " :is already on channel\r\n");
			return;

		}
		
		if (channel.getPasswordMode() && password == channel.getPassword()) {
			handleChannelJoin(client, clientFd, channel);
			return;

		}

		sendMessage(clientFd, "475 " + client.getNickName() + " " + channelName + " :Cannot join channel (+k)\r\n");

	} else {
		if (!isValidPassword(password)) {
			std::string message = "475 " + client.getNickName() + " " + channelName + " :Invalid password format\r\n";
			sendMessage(clientFd, message);
    		return;
		}
		createChannel(channelName, client);
		channels_[channelName].setPasswordMode(true, password);

		std::string joinMsg = ":" + client.getNickName() + " JOIN " + channelName + "\r\n";
		sendMessage(clientFd, joinMsg);
		sendMessage(clientFd, buildWelcomeMessage(channels_[channelName], client));
		showMembersList(client, channels_[channelName]);
	}
}

void Server::handleChannelJoin(Client &client, int clientFd, Channel &channel) {
	if (channel.getLimitMode() && channel.getMembers().size() >= channel.getLimit()) {
		sendMessage(clientFd, "471 " + client.getNickName() + " " + channel.getChannelName() + " :Cannot join channel (+l)\r\n");
		return;
	}
	if (channel.getInviteMode() && !channel.isInvited(client)) {
		sendMessage(clientFd, "473 " + client.getNickName() + " " + channel.getChannelName() + " :Cannot join channel (+i)\r\n");
		return;
	}

	channel.insertMember(client);

	std::string joinMsg = ":" + client.getNickName() + " JOIN " + channel.getChannelName() + "\r\n";
	sendToChannelExcept(channel, client, joinMsg);
	sendMessage(clientFd, joinMsg);
	sendMessage(clientFd, buildWelcomeMessage(channel, client));
	showMembersList(client, channel);
}

void Server::showMembersList(Client &client, Channel &channel) {
	std::vector<std::string> membersSplit = split(channel.memberList(), " ");
	std::string memberList = ":server 353 " + client.getNickName() + " = " + channel.getChannelName() + " :";

	for (size_t i = 0; i < membersSplit.size(); ++i) {
		if (i < (membersSplit.size() - 1)) {
			memberList += membersSplit[i] + " ";
		} else {
			memberList += membersSplit[i] + "\r\n";
		}
	}
	sendMessage(client.getClientFd(), memberList);
	sendMessage(client.getClientFd(), ":server 366 " + client.getNickName() + " " + channel.getChannelName() + " :End of /NAMES list.\r\n");
}

std::string Server::buildWelcomeMessage(Channel& channel, Client& client) {
    std::string welcomeMessage = "Welcome to the channel " + channel.getChannelName() + "!\r\n";
	if (channel.getTopic().size() == 0) {
		welcomeMessage += ":server 331 " + client.getNickName() + " " + channel.getChannelName() + " :";
		welcomeMessage += "No topic is set";
	} else {
		welcomeMessage += ":server 332 " + client.getNickName() + " " + channel.getChannelName() + " :";
		welcomeMessage += channel.getTopic();
	}
    welcomeMessage += "\r\n";
    return welcomeMessage;
}