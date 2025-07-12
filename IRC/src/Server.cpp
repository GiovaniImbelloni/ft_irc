#include "Server.hpp"
#include "Utils.hpp"

volatile bool g_running = true;

void signalHandler(int signum) {
	if (signum == SIGINT) {
		std::cout << "\nSIGINT received. ";
		g_running = false;
	}
}

Server::Server(int port, const std::string &password) 
	:	port_(port),
		server_fd_(-1),
		password_(password), 
		cmd_(),
		serverAddress_(),
		pollfds_(),
		params_(),
		authenticatedClients_(),
		clientBuffers_(),
		mapClients_(),
		channels_() 
{
	preAuthCommands_["CAP"] = &Server::cap_cmd;
	preAuthCommands_["PASS"] = &Server::pass_cmd;
	preAuthCommands_["NICK"] = &Server::nick_cmd;
	preAuthCommands_["USER"] = &Server::user_cmd;

	postAuthCommands_["CAP"] = &Server::cap_cmd;
	postAuthCommands_["PASS"] = &Server::pass_cmd;
	postAuthCommands_["NICK"] = &Server::nick_cmd;
	postAuthCommands_["USER"] = &Server::user_cmd;
	postAuthCommands_["JOIN"] = &Server::join_cmd;
	postAuthCommands_["INVITE"] = &Server::invite_cmd;
	postAuthCommands_["KICK"] = &Server::kick_cmd;
	postAuthCommands_["LIST"] = &Server::list_cmd;
	postAuthCommands_["MODE"] = &Server::mode_cmd;
	postAuthCommands_["NAMES"] = &Server::names_cmd;
	postAuthCommands_["PRIVMSG"] = &Server::privmsg_cmd;
	postAuthCommands_["TOPIC"] = &Server::topic_cmd;
	postAuthCommands_["PART"] = &Server::part_cmd;
	postAuthCommands_["QUIT"] = &Server::quit_cmd;

}

Server::~Server() {
	for (std::map<int, Client>::iterator it = mapClients_.begin(); it != mapClients_.end(); ++it) {
		sendMessage(it->first, "ERROR :Server shutting down\r\n");
		close(it->first);
	}

	if (!pollfds_.empty()) {
		close(pollfds_[0].fd);
	}

	mapClients_.clear();
	pollfds_.clear();
	authenticatedClients_.clear();
	clientBuffers_.clear();

	std::cout << "Server shutting down..." << std::endl;
}

void Server::printClientInfo(int clientFd) {
	Client client = mapClients_[clientFd];
	std::cout << client.getNickName() << "@" << client.getUserName()
		<< " [" << client.getClientAddress() << ":" << client.getClientPort()
		<< "] (fd: " << client.getClientFd() << ")" << std::endl;
}


void Server::start() {
	server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd_ == -1) {
		throw std::runtime_error("Failed to create socket! Error: " + std::string(strerror(errno)));
	}

	int opt = 1;
	if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		close(server_fd_);
		throw std::runtime_error("setsockopt(SO_REUSEADDR) failed! Error " + std::string(strerror(errno)));
	}

	// Set the server socket to non-blocking mode directly
	if (fcntl(server_fd_, F_SETFL, O_NONBLOCK) == -1) {
		close(server_fd_);
		throw std::runtime_error("Failed to set non-blocking mode! Error: " + std::string(strerror(errno)));
	}

	// Specify socket address
	serverAddress_.sin_family = AF_INET;            // IPv4
	serverAddress_.sin_port = htons(port_);         // Convert to network byte order
	serverAddress_.sin_addr.s_addr = INADDR_ANY;    // Any available IP address

	if (bind(server_fd_, (sockaddr*)& serverAddress_, sizeof(serverAddress_)) == -1) {
		close(server_fd_);
		throw std::runtime_error("Binding failed! Error: " + std::string(strerror(errno)));
	}

	if (listen(server_fd_, 5) == -1) {
		close(server_fd_);
		throw std::runtime_error("Listening failed! Error: " + std::string(strerror(errno)));
	}

	std::cout << "Server is running and waiting for connections on port " << port_ << "..." << std::endl;
}

void Server::run() {
	start();
	signal(SIGINT, signalHandler);
	signal(SIGPIPE, SIG_IGN);
	struct pollfd serverPollFd;
	serverPollFd.fd = server_fd_;
	serverPollFd.events = POLLIN; // Monitor for incoming connections
	serverPollFd.revents = 0;
	pollfds_.push_back(serverPollFd);

	while (g_running) {
		int pollResult = poll(pollfds_.data(), pollfds_.size(), -1);

		if (pollResult == -1) {
			if (errno == EINTR) {
				break;
			} else {
				throw std::runtime_error("Poll failed! Error: " + std::string(strerror(errno)));
			}
		}

		// Handle new connection
		if (pollfds_[0].revents & POLLIN) {
			std::cout << "Handle New Connection" << std::endl;
			try {
				handleNewConnection();
			} catch (const std::exception& e) {
				std::cerr << "Connection error: " << e.what() << std::endl;
			}			
		}

		// Handle client activity
		for (size_t i = 1; i < pollfds_.size(); ++i) {
			if (pollfds_[i].revents & POLLIN) {
				handleClientData(pollfds_[i].fd);
			}
		}
	}
}

void Server::handleClientData(int fd) {
	char buffer[4096];
	int bytesReceived = recv(fd, buffer, sizeof(buffer), 0);
	if (bytesReceived == -1) {
		std::cerr << "Failed to receive message from client ";
		printClientInfo(fd);
		std::cout << " Error: " << strerror(errno);
		std::cout << ". Client disconnected!" << std::endl;
		cleanupClient(fd);
	}
	else if (bytesReceived == 0) {
		std::cout << "Client disconnected: ";
		printClientInfo(fd);
		cleanupClient(fd);
	}
	else {
		std::cout << buffer << std::endl;
		std::cout << "FD: " << fd << std::endl;
		clientBuffers_[fd] += std::string(buffer, bytesReceived);
		processClientData(fd);	
	}

}

void Server::handleNewConnection() {
	Client		client;
	sockaddr_in	clientAddress;

	socklen_t clientSize = sizeof(clientAddress);

	int clientFd = accept(server_fd_, (sockaddr*)&clientAddress, &clientSize);
	if (clientFd == -1) {
		throw std::runtime_error("Failed to accept connection! Error: " + std::string(strerror(errno)));
	}

	// Set the new socket to non-blocking mode directly
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1) {
		close(clientFd);
		throw std::runtime_error("Failed to set non-blocking mode for client socket! Error: " + std::string(strerror(errno)));
	}

	// Add new client socket to pollfds
	struct pollfd clientPollFd;
	clientPollFd.fd = clientFd;
	clientPollFd.events = POLLIN; // Monitor for incoming data
	clientPollFd.revents = 0;
	pollfds_.push_back(clientPollFd);

	// Store the new client
	client.setClientFd(clientFd);
	client.setClientAddress(inet_ntoa(clientAddress.sin_addr));
	client.setClientPort(ntohs(clientAddress.sin_port));
	mapClients_[clientFd] = client;
	authenticatedClients_[clientFd] = false;

	std::cout << "Client connected from " << client.getClientAddress() << ":" << ntohs(clientAddress.sin_port) << std::endl;
	sendMessage(clientFd, "NOTICE AUTH :Welcome! To register, you must send PASS, NICK and USER commands.\r\n");

}

void Server::sendToChannel(const std::string& channelName, const std::string& message) {
	std::map<std::string, Channel>::iterator it = channels_.find(channelName);
	if (it != channels_.end()) {
		Channel& channel = it->second;
		const std::map<int, Client*>& members = channel.getMembers();

		for (std::map<int, Client*>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
			Client* client = memberIt->second;
			if (client) {
				int clientFd = client->getClientFd();
				sendMessage(clientFd, message);
			}
		}
	}
	else {
		std::cerr << "Channel " << channelName << " does not exist." << std::endl;
	}
}

void Server::sendToChannelExcept(const Channel& channel, const Client& sender, const std::string& message) const {
	const std::map<int, Client*>& members = channel.getMembers();

	for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
		if (it->second->getClientFd() != sender.getClientFd()) {
			sendMessage(it->first, message);
		}
	}
}

void Server::createChannel(const std::string& channelName, Client& client) {
	channels_.insert(std::make_pair(channelName, Channel(channelName, client)));
}

void	Server::deleteChannel(const std::string& channelName) {
	channels_.erase(channelName);
}

void	Server::leaveAllChannels(Client &client) {
	std::map<std::string, Channel>::iterator it = channels_.begin();

	while (it != channels_.end()) {
		Channel& channel = it->second;
		channel.removeInvited(client);

		if (channel.isMember(client)) {
			std::string channelName = channel.getChannelName();
			std::string partMessage = ":" + client.getNickName() + " PART " + channelName + "\r\n";

			if (channel.isOperator(client)) {
				channel.removeOperator(client);
			} 

			channel.removeMember(client);

			std::cout << client.getNickName() << " left the channel " << channelName << std::endl;

			++it;
			if (channel.getMembers().empty()) {
				deleteChannel(channelName);
			} else {
				sendToChannel(channelName, partMessage);
			}
		} else {
			++it;
		}
	}
}

bool Server::isChannelExist(const std::string& channelName) const {
	std::map<std::string, Channel>::const_iterator it = channels_.find(channelName);
	if (it == channels_.end()) {
		return false;
	}
	return true;
}

void Server::cleanupClient(int clientFd) {
	std::cout << "Cleaning up client " << clientFd << " " << std::endl;

	if (mapClients_.find(clientFd) != mapClients_.end()) {
		Client& client = mapClients_[clientFd];
		leaveAllChannels(client);
	}	

	close(clientFd);

	authenticatedClients_.erase(clientFd);
	mapClients_.erase(clientFd);
	clientBuffers_.erase(clientFd);

	for (std::vector<struct pollfd>::iterator it = pollfds_.begin(); it != pollfds_.end(); ++it) {
		if (it->fd == clientFd) {
			pollfds_.erase(it);
			break;
		}
	}
}

void Server::tryAuthenticate(Client& client, int clientFd) {
	if (client.getIsAuthenticated())
		return;

	if (client.getIsRegistratedWithPass()
		&& !client.getNickName().empty()
		&& !client.getUserName().empty())
	{
		client.setIsAuthenticated(true);
		authenticatedClients_[clientFd] = true;

		// 001 = Welcome message (IRC standard)
		std::string welcomeMsg = "001 " + client.getNickName() + " :Welcome to the IRC server\r\n";
		sendMessage(clientFd, welcomeMsg);

	}
}


bool Server::isNicknameInUse(const std::string &nick) const {
	std::map<int, Client>::const_iterator it;

	for (it = mapClients_.begin(); it != mapClients_.end(); ++it) {
		if (it->second.getNickName() == nick) {
			return true;
		}
	}
	return false;
}

void Server::processClientMessage(int clientFd, std::string cmd, std::vector<std::string> params) {
	Client &client = mapClients_[clientFd];

	std::cout << "cmd: " << cmd << std::endl;
	for (std::size_t i = 0; i < params.size(); i++) {
		std::cout << "params: " << params[i] << std::endl;
	}

	if (cmd.empty())
		return;

	if (!authenticatedClients_[clientFd]) {
		std::map<std::string, CommandHandler>::iterator it = preAuthCommands_.find(cmd);
		if (it != preAuthCommands_.end()) {
			CommandHandler handler = it->second;
			(this->*handler)(client, clientFd, params);
		} else {
			sendMessage(clientFd, "451 :You have not registered\r\n");
		}
	} else {
		std::map<std::string, CommandHandler>::iterator it = postAuthCommands_.find(cmd);
		if (it != postAuthCommands_.end()) {
			CommandHandler handler = it->second;
			(this->*handler)(client, clientFd, params);
		} else {
			sendMessage(clientFd, "421 " + cmd + " :Unknown command\r\n");
		}
	}
}

void Server::splitCmdLine(std::string input) {
	params_.clear();
	cmd_.clear();

	if (input.empty())
		return;

	size_t first_not_space = input.find_first_not_of(" \t\v\f");
	if (first_not_space == std::string::npos)
		return;
	input = input.substr(first_not_space);

	size_t space_pos = input.find_first_of(" \t\v\f");
	cmd_ = input.substr(0, space_pos);

	for (size_t i = 0; i < cmd_.size(); i++) {
		cmd_[i] = std::toupper(static_cast<unsigned char>(cmd_[i]));
	}

	if (space_pos != std::string::npos) {
		input.erase(0, space_pos + 1);
	} else {
		input.clear();
	}

	while (!input.empty()) {
		size_t first_not_space = input.find_first_not_of(" \t\v\f");
		if (first_not_space == std::string::npos)
			break;
		input = input.substr(first_not_space);
		
		if (input[0] == ':') {
			input.erase(0, 1);
			if (!input.empty())
				params_.push_back(input);
			break;
		}
		space_pos = input.find_first_of(" \t\v\f");
		if (space_pos == std::string::npos)
		{
			params_.push_back(input);
			break;
		}
		else
		{
			params_.push_back(input.substr(0, space_pos));
			input.erase(0, ++space_pos);
		}
	}
}

std::string sanitizeInput(const std::string& input) {
	std::string result;
	for (size_t i = 0; i < input.size(); ++i) {
		if (isprint(input[i]) || input[i] == '\r' || input[i] == '\n') {
			result += input[i];
		}
	}
	return result;
}

void Server::processClientData(int clientFd) {
	int maxCommandsPerLoop = 10;
	int commandsProcessed = 0;
	std::string &buffer = clientBuffers_[clientFd];

	buffer = sanitizeInput(buffer);

	size_t pos;

	while ((pos = buffer.find_first_of("\r\n")) != std::string::npos && commandsProcessed < maxCommandsPerLoop) {
		std::string singleMessage = buffer.substr(0, pos);
		buffer.erase(0, pos + 1);

		if (!buffer.empty() && (buffer[0] == '\r' || buffer[0] == '\n')) {
			buffer.erase(0, 1);
		}
		splitCmdLine(singleMessage);
		processClientMessage(clientFd, cmd_, params_);
		++commandsProcessed;
	}
}

Client* Server::findClientByNick(const std::string& nickname) {
	for (std::map<int, Client>::iterator it = mapClients_.begin(); it != mapClients_.end(); ++it) {
		if (it->second.getNickName() == nickname) {
			return &it->second;
		}
	}
	return NULL;
}

bool Server::isClientInChannel(const Client& client, const std::string& channelName) const {
	std::map<std::string, Channel>::const_iterator it = channels_.find(channelName);
	if (it != channels_.end()) {
		return it->second.isMember(client);
	}
	return false;
}