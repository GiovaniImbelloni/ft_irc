#include "Channel.hpp"
#include "Utils.hpp"

Channel::Channel() : channelName_("default") {}

Channel::Channel(const std::string& channelName, Client& client)
	:	channelName_(channelName),
		topic_(""),
		password_(""),
		limit_(0),
		inviteMode_(false),
		limitMode_(false),
		passwordMode_(false),
		topicMode_(false)

{
	insertMember(client);
	insertOperator(client);
	const std::string infoMessage = "Channel " + getChannelName() + " created!\n";
	sendMessage(client.getClientFd(), infoMessage);
}

Channel::~Channel() {}

// /************************************************************************************/

void	Channel::setTopic(const std::string& topic) {
	topic_ = topic;
}

void	Channel::setTopicMode(bool status) {
	topicMode_ = status;
}

void	Channel::setPassword(const std::string& password) {
	password_ = password;
}

void	Channel::setPasswordMode(bool status, const std::string& password) {
	passwordMode_ = status;

	if (status == true) {
		setPassword(password);
	} else {
		password_.clear();
	}
}

void	Channel::setLimit(unsigned int limit) {
	limit_ = limit;
}

void	Channel::setLimitMode(bool status, unsigned int limit) {
	limitMode_ = status;

	if(status) {
		setLimit(limit);
	}
}

void	Channel::setInvitedMode(bool status) {
	inviteMode_ = status;
}

// /************************************************************************************/

std::string	Channel::getChannelName() const {
	return channelName_;
}

std::string	Channel::getTopic() const {
		return topic_;
}

std::string	Channel::getPassword() const {
	return password_;
}

unsigned int	Channel::getLimit() const {
	return limit_;
}

const std::map<int, Client*>&	Channel::getMembers() const {
	return members_;
}

const std::map<int, Client*>&	Channel::getOperators() const {
	return operators_;
}

std::set<int>	Channel::getInvited() const {
	return invited_;
}

bool	Channel::getInviteMode() const {
	return inviteMode_;
}

bool	Channel::getTopicMode() const {
	return topicMode_;
}

bool	Channel::getPasswordMode() const {
	return passwordMode_;
}

bool	Channel::getLimitMode() const {
	return limitMode_;
}

// /************************************************************************************/

void Channel::insertMember(Client &client) {
	members_.insert(std::make_pair(client.getClientFd(), &client));
	invited_.erase(client.getClientFd());
}

void	Channel::insertOperator(Client &client) {
	operators_.insert(std::make_pair(client.getClientFd(), &client));
}

void	Channel::insertInvited(Client &client) {
	invited_.insert(client.getClientFd());
}

/************************************************************************************/

void	Channel::removeMember(Client &client) {
	members_.erase(client.getClientFd());
}

void	Channel::removeOperator(Client &client) {
	operators_.erase(client.getClientFd());
}

void	Channel::removeInvited(Client &client) {
	invited_.erase(client.getClientFd());
}

// /************************************************************************************/

bool	Channel::isOperator(const Client& client) const {
	return operators_.find(client.getClientFd()) != operators_.end();
}

bool	Channel::isMember(const Client& client) const {
	return members_.find(client.getClientFd()) != members_.end();
}

bool	Channel::isInvited(const Client& client) const {
	return invited_.find(client.getClientFd()) != invited_.end();
}

bool	Channel::isEmpty() const {
    return members_.empty();
}

// /************************************************************************************/

std::string Channel::memberList() const {
	std::string nicknames;

	for (std::map<int, Client*>::const_iterator it = operators_.begin(); it != operators_.end(); ++it) {
		if (!nicknames.empty()) {
			nicknames += " ";
		}
		nicknames += "@" + it->second->getNickName();
	}

	for (std::map<int, Client*>::const_iterator it = members_.begin(); it != members_.end(); ++it) {
		if (operators_.find(it->first) == operators_.end()) {
			if (!nicknames.empty()) {
				nicknames += " ";
			}
			nicknames += it->second->getNickName();
		}
	}

	return nicknames;
}
