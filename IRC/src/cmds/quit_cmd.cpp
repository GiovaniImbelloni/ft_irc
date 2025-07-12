#include "Server.hpp"
#include "Utils.hpp"

void Server::quit_cmd(Client& client, int clientFd, std::vector<std::string> params) {
    std::string quitMessage = "Client Quit";

    if (!params.empty()) {
        quitMessage = params[0];
        for (size_t i = 1; i < params.size(); ++i) {
            quitMessage += " " + params[i];
        }
    }

    std::map<std::string, Channel>::iterator it = channels_.begin();
    std::set<int> notifiedClients;

    while (it != channels_.end()) {
        Channel& channel = it->second;

        if (channel.isMember(client)) {
            std::string quitMsg = ":" + client.getNickName() + " QUIT :" + quitMessage + "\r\n";

            const std::map<int, Client*>& members = channel.getMembers();
            for (std::map<int, Client*>::const_iterator memberIt = members.begin(); 
                 memberIt != members.end(); ++memberIt) {

                if (memberIt->second->getClientFd() != clientFd && 
                    notifiedClients.find(memberIt->second->getClientFd()) == notifiedClients.end()) {
                    sendMessage(memberIt->second->getClientFd(), quitMsg);
                    notifiedClients.insert(memberIt->second->getClientFd());
                }
            }
        }
		++it;
    }

    sendMessage(clientFd, "ERROR :Closing Link: " + client.getClientAddress() + " (" + quitMessage + ")\r\n");
    cleanupClient(clientFd);
}