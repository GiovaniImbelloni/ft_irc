#include "Server.hpp"
#include "Utils.hpp"
#include <sstream>

void Server::list_cmd(Client &client, int clientFd, std::vector<std::string> params) {
	sendMessage(clientFd, "321 " + client.getNickName() + " Channel :Users Name\r\n");

	if (params.empty()) {
		for (std::map<std::string, Channel>::iterator it = channels_.begin(); it != channels_.end(); ++it) {
			Channel& channel = it->second;
			std::string channelName = channel.getChannelName();
			size_t userCount = channel.getMembers().size();
			std::string topic = channel.getTopic();

			std::ostringstream oss;
			oss << userCount;
			std::string userCountStr = oss.str();

			std::string listMsg = "322 " + client.getNickName() + " " + channelName + " " + 
								userCountStr + " :" + topic + "\r\n";
			sendMessage(clientFd, listMsg);
		}
	} else {
		for (size_t i = 0; i < params.size(); ++i) {
			std::string channelName = params[i];
			if (isChannelExist(channelName)) {
				Channel& channel = channels_[channelName];
				size_t userCount = channel.getMembers().size();
				std::string topic = channel.getTopic();

				std::ostringstream oss;
				oss << userCount;
				std::string userCountStr = oss.str();
				
				std::string listMsg = "322 " + client.getNickName() + " " + channelName + " " + 
									userCountStr + " :" + topic + "\r\n";
				sendMessage(clientFd, listMsg);
			}
		}
	}

	sendMessage(clientFd, "323 " + client.getNickName() + " :End of /LIST\r\n");
}