#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <vector>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <sys/select.h>
# include <cstdlib>
# include <poll.h>
# include <map>

# include <set>


class Client
{
	public:
		Client();
		~Client();

		// getters
		int			getClientFd() const;
		int			getClientPort() const;
		std::string	getNickName() const;
		std::string	getUserName() const;
		std::string	getClientAddress() const;
		std::string	getRealName() const;
		bool		getIsRegistratedWithPass() const;
		bool		getIsAuthenticated() const;
		bool 		getModeInvisible() const;

		// setters
		void		setClientFd(int clientFd);
		void		setClientPort(int port);
		void		setNickName(const std::string &nickName);
		void		setIsAuthenticated(bool authenticated);
		void		setUserName(const std::string &userName);
		void		setClientAddress(const std::string &clientAddress);
		void		setRealName(const std::string &realName);
		void		setIsRegisteredWithPass(bool registeredWithPass);
		void 		setModeInvisible(bool modeInvisible);

	private:
		int			clientFd_;
		int			clientPort_;
		std::string	nickName_;
		std::string userName_;
		std::string clientAddress_;
		std::string	realName_;
		bool		isRegisteredWithPass_;
		bool		isAuthenticated_;
		bool		modeInvisible_;
};

#endif