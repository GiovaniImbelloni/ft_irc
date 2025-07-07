#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <cctype>
# include <algorithm>
# include <set>
# include <map>
# include <vector>
# include "Client.hpp"

class Channel
{
	public:
		Channel();
		Channel(const std::string& name, Client& creator);
		~Channel();

		void							setTopic(const std::string& topic);
		void							setTopicMode(bool status);
		void							setPassword(const std::string& password);
		void							setPasswordMode(bool status, const std::string& password);
		void							setLimit(unsigned int limit);
		void							setInvitedMode(bool status);
		void							setLimitMode(bool status, unsigned int limit);
		int 							getUsersCount() const;
		std::string						getChannelName() const;
		std::string						getTopic() const;
		std::string						getPassword() const;
		unsigned int					getLimit() const;
		const std::map<int, Client*>&	getMembers() const;
		const std::map<int, Client*>&	getOperators() const;
		std::set<int>					getInvited() const;
		bool							getInviteMode() const;
		bool							getTopicMode() const;
		bool							getPasswordMode() const;
		bool							getLimitMode() const;

		void							insertOperator(Client &client);
		void							insertMember(Client &client);
		void							insertInvited(Client &client);

		void							removeOperator(Client &client);
		void							removeMember(Client &client);
		void							removeInvited(Client &client);

		bool							isOperator(const Client& client) const;
		bool							isMember(const Client& client) const;
		bool							isInvited(const Client& client) const;
		bool							isEmpty() const;
		std::string						memberList() const;

	private:
		std::string						channelName_;
		std::string						topic_;
		std::string						password_;
		unsigned int					limit_;
		std::map<int, Client*>			members_;
		std::map<int, Client*>			operators_;
		std::set<int>					invited_;
		bool							inviteMode_;
		bool							limitMode_;
		bool							passwordMode_;
		bool							topicMode_;
};

#endif
