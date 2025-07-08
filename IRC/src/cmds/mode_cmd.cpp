#include "Server.hpp"
#include "Utils.hpp"
#include <sstream>

void Server::mode_cmd(Client &client, int clientFd, std::vector<std::string> params) {
	if (params.empty()) {
		sendMessage(clientFd, "461 MODE :Not enough parameters\r\n");
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
		std::string modes = "+";
		if (channel.getInviteMode()) modes += "i";
		if (channel.getTopicMode()) modes += "t";
		if (channel.getPasswordMode()) modes += "k";
		if (channel.getLimitMode()) modes += "l";
		
		sendMessage(clientFd, "324 " + client.getNickName() + " " + channelName + " " + modes + "\r\n");
		return;
	}

	if (!channel.isOperator(client)) {
		sendMessage(clientFd, "482 " + client.getNickName() + " " + channelName + " :You're not channel operator\r\n");
		return;
	}

	std::string modeString = params[1];
	size_t paramIndex = 2;
	bool adding = true;
	std::string addedModes;
	std::string removedModes;
	std::string modeParams;
	
	for (size_t i = 0; i < modeString.length(); ++i) {
		char mode = modeString[i];
	
		if (mode == '+') {
			adding = true;
			continue;
		} else if (mode == '-') {
			adding = false;
			continue;
		}

		switch (mode) {
			case 'i':
				channel.setInvitedMode(adding);
				if (adding) {
					addedModes += "i";
				} else {
					removedModes += "i";
				}
				break;

			case 't':
				channel.setTopicMode(adding);
				if (adding) {
					addedModes += "t";
				} else {
					removedModes += "t";
				}
				break;

			case 'k':
				if (adding && paramIndex < params.size()) {
					std::string password = params[paramIndex++];
					if (isValidPassword(password)) {
						channel.setPasswordMode(true, password);
						addedModes += "k";
						modeParams += " " + password;
					}
				} else if (!adding) {
					channel.setPasswordMode(false, "");
					removedModes += "k";
				}
				break;

			case 'l':
				if (adding && paramIndex < params.size()) {
					std::string limitStr = params[paramIndex++];
					std::istringstream iss(limitStr);
					unsigned int limit;
					if (iss >> limit && limit > 0) {
						channel.setLimitMode(true, limit);
						addedModes += "l";
						modeParams += " " + limitStr;
					}
				} else if (!adding) {
					channel.setLimitMode(false, 0);
					removedModes += "l";
				}
				break;

			case 'o':
				if (paramIndex < params.size()) {
					std::string targetNick = params[paramIndex++];
					Client* targetClient = findClientByNick(targetNick);
					
					if (targetClient && channel.isMember(*targetClient)) {
						if (adding) {
							channel.insertOperator(*targetClient);
							addedModes += "o";
						} else {
							channel.removeOperator(*targetClient);
							removedModes += "o";
						}
						modeParams += " " + targetNick;
					}
				}
				break;

			default:
				sendMessage(clientFd, "472 " + client.getNickName() + " " + std::string(1, mode) + " :is unknown mode char to me\r\n");
				continue;
		}
	}

	if (!addedModes.empty() || !removedModes.empty()) {
		std::string modeChanges;
		if (!addedModes.empty()) {
			modeChanges += "+" + addedModes;
		}
		if (!removedModes.empty()) {
			modeChanges += "-" + removedModes;
		}

		std::string modeMsg = ":" + client.getNickName() + " MODE " + channelName + " " + modeChanges + modeParams + "\r\n";
		sendToChannel(channelName, modeMsg);
	}
}