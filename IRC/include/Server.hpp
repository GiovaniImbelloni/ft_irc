# include <iostream>
# include <string>
# include <stack>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <sys/select.h>
# include <cstdlib>
# include <poll.h>
# include <map>
# include <queue>
# include <cerrno>
# include <sstream>
# include <iostream>
# include <csignal>
# include <cstdlib>
# include <set>

# include "Client.hpp"
# include "Channel.hpp"

class Server {
public:
	Server(int port, const std::string& password);
	~Server();
	void run();

private:
	int								port_;
	int								server_fd_;
	std::string						password_;
	std::string 					cmd_;
	sockaddr_in						serverAddress_;
	std::vector<struct pollfd>		pollfds_;
	std::vector<std::string> 		params_;
	std::map<int, bool>				authenticatedClients_;
	std::map<int, std::string>		clientBuffers_;
	std::map<int, Client> 			mapClients_;
	std::map<std::string, Channel>	channels_;
	
	void	start();
	void	handleNewConnection();
	void 	handleClientData(int fd);
	void	processClientMessage(int clientFd, std::string cmd, std::vector<std::string>params);
	void	cleanupClient(int clientFd);
	void	printClientInfo(int clientFd);
	void	splitCmdLine(std::string input);
	bool 	isNicknameInUse(const std::string &nick) const;
	void	processClientData(int clientFd);
	void	tryAuthenticate(Client& client, int clientFd);

	void	createChannel(const std::string &channelName, Client &creator);
	void	deleteChannel(std::string &channelName);
	bool	isChannelExist(const std::string &channelName) const;

	void	pass_cmd(Client &client, int clientFd, std::vector<std::string> params);
	void	nick_cmd(Client &client, int clientFd, std::vector<std::string> params);
	void	user_cmd(Client &client, int clientFd, std::vector<std::string> params);
	void	join_cmd(Client &client, int clientFd, std::vector<std::string> params);

	bool	isChannelNameValid(const std::string& name, int clientFd);
	void	joinChannelWithoutPassword(Client &client, int clientFd, const std::string &channelName);
	void	joinChannelWithPassword(Client &client, int clientFd, const std::string &channelName, const std::string &password);
	void	handleChannelJoin(Client &client, int clientFd, Channel &channel);
	void	showMembersList(Client &client, Channel &channel);
	void 	sendToChannel(const std::string& channelName, const std::string& message);
	void 	sendToChannelExcept(const Channel& channel, const Client& sender, const std::string& message) const;
	void	leaveAllChannels(Client &client);

	std::string buildWelcomeMessage(Channel &channel, Client& client);
};