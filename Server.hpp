#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#include <cstdlib>

#include <iostream>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <iostream> 
#include <signal.h>

#include <map>
#include <queue>
#include <vector>
#include "tools.hpp"
#include "User.hpp"
#include "Channel.hpp"


#define COLOR_GREEN "\x1b[32m"
#define COLOR_DEFAULT "\x1b[0m"
#define COLOR_RED "\x1b[31m" 
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_CYAN "\x1b[36m"

# define SERV_NAME std::string("VIRC_SERVER")
# define UMODES std::string("ov") // available user _modes
# define CMODES std::string("bkomstnv") // available channel _modes

inline std::string cleanString(std::string str) {
  std::size_t start = str.find_first_not_of(" \t\r\n");
  std::size_t end = str.find_last_not_of(" \t\r\n");
  if (start == std::string::npos || end == std::string::npos) {
    return "";
  }
  return str.substr(start, end - start + 1);
}

class	Server{
	private:
		Server &operator=(Server const & rhs){
			this->~Server();
			new (this) Server(rhs);
			return (*this);
		};

		static const int		MAX_CLIENTS = 64;
		static const ssize_t	BUFFER_SIZE = 1024;

		std::string				__pwd;
		std::string				__port;
		std::string				created_at;
		int						listen_socket;
		int						server_socket;
		fd_set					current_sockets;
		std::vector<int>		_client_fds;
		std::vector<User>		_users;
		std::vector<Channel>	_channels;


	public:

		//Server constructors
		Server() {}
		Server(int const & port, std::string const & password){
			this->__port = port;
			this->__pwd = password;
			std::time_t result = std::time(nullptr);
		    this->created_at = std::asctime(std::localtime(&result));
		    std::cout << COLOR_YELLOW << "👉🏼 Initialising server - - - created @ " << this->created_at << std::endl << COLOR_DEFAULT;
		};
		~Server() { std::cout << COLOR_CYAN<< "IRC Server stopped and shut down...👋🏽\n" << COLOR_DEFAULT; }

		//Server main functions
		int			start(int port);
		void		go();
		void		stop_server();


		//Getters
		int			getUser(int client_socket, User& user);
		std::string	getPort() {return (this->__port);};
		std::string getPassword() { return (this->__pwd);};
		int			getNumberUsers() {return (_users.size()); }
		
		//Setters
		void	setServerSocket(int s){this->server_socket = s;}

		//Security
		bool 		verifyPassword(std::string password) {return (password.compare(0, password.size(), this->getPassword())); };

		bool		isSpace(std::string str) {
			for (int i = 0; str[i]; i++){
				if (str[i] == ' ' || str[i] == 7 || str[i] == ',')
					return true;
			}
			return false;
		}

		//Client related
		bool		isUsernameTaken(std::string username)
		{
			for (std::vector<User>::iterator i = _users.begin(); i < _users.end(); i++)
			{
				if (!((*i).getUsername().compare(username)))
					return true;
			}
			return false;
		}

		bool		isNicknameTaken(std::string nickname)
		{
			for (std::vector<User>::iterator i = _users.begin(); i < _users.end(); i++)
			{
				if (!((*i).getNickname().compare(nickname)))
					return true;
			}
			return false;
		}

		void		handleClientMessage(std::string data, int client_fd);
		User		getUser(int fd) {
			for (int i = 0; i < getNumberUsers(); i++) {
            if (_users[i].getFd() == fd) {
                return _users[i]; }
			}
			return (User());
		}

		User		getUser(std::string nick) {
			for (int i = 0; i <= getNumberUsers(); i++) {
			std::cout << "getting user by nick '" << _users[i].getNickname() << "'" << std::endl;
            if (!(_users[i].getNickname().compare(nick))) {
                return _users[i]; }
			}
			return (User());
		}

		void	updateUser(const User &user){
			for (int i = 0; i < getNumberUsers(); i++){
				if (_users[i].getFd() == user.getFd()) {
					_users[i] = user;
				}
			}
		}

		void	updateChannel(const Channel& chan){
			for (unsigned long i = 0; i < _channels.size(); i++){
				if (_channels[i].getName() == chan.getName()) {
					_channels[i] = chan;
				}
			}
		}

		void addUser(const User& user) {
        _users.push_back(user);
    	}

		void addClientFd(int fd) {
	    _client_fds.push_back(fd);
		}

		bool hasClientFd(int fd) {
		    return std::find(_client_fds.begin(), _client_fds.end(), fd) != _client_fds.end();
		}

		// channels
		bool	channelExists(std::string channelName)
		{
			for (unsigned long i = 0; i < _channels.size(); i++) {
      		if (_channels[i].getName() == channelName) {
      		  return true;
      		}
			}
			return false;
		}

		void addChannel(const Channel& channel) {
        _channels.push_back(channel);
    	}

		int			getNumberChannels() {return (_channels.size()); }

		Channel		getChannel(std::string name) {
			for (int i = 0; i <= getNumberChannels(); i++) {
            if (!(_channels[i].getName().compare(name))) {
                return _channels[i]; }
			}
			return (Channel());
		}

		int checkReadyToWrite(int fd)
		{
		    struct pollfd pfd;
		    pfd.fd = fd;
		    pfd.events = POLLOUT;
		    pfd.revents = 0;

		    int ret = poll(&pfd, 1, 0);
		    if (ret < 0) {
		        std::cout << "Error POLLOUT for User " << fd << std::endl;
		        return 0;
		    } else if (ret == 0) {
		        std::cout << "Fd " << fd << " for User not ready for writing." << std::endl;
		    }
		    if (pfd.revents & POLLOUT)
		        return 1;

		    return 0;
		}

		void    sendMessageToReceiver(int fd, std::string sender, std::string message){
			if(checkReadyToWrite(fd)){
					send(fd, "\n", 2, 0);
                    send(fd, sender.data(), sender.size(), 0);
                    send(fd, ": ", 3, 0);
                    send(fd, message.data(), message.size() - 1, 0);
                    // send(fd, "\n", 2, 0); //TODO: double /n needed?
			}
			else {
				std::cout << "[user] fd " << fd << " not ready to write" << std::endl;
			}
		}

		void	sendMessageToChannel(User const &user, std::string message){
			Channel channel = getChannel(user.getWhatChannel());
			for (std::vector<User>::iterator i = channel.getUsersInChannel().begin(); i < channel.getUsersInChannel().end(); i++){
				User receiver = (*i);
				if (receiver.getFd() != user.getFd()){
					if (checkReadyToWrite(receiver.getFd())){
						sendMessageToReceiver(receiver.getFd(), user.getNickname(), message);
					}
					else 
						std::cout << "[channel] fd " << receiver.getFd() << " not ready to write" << std::endl;
				}
			}
		}

		bool	isCmd(std::string cmd){
			return ((!(cmd.compare(0, 4, "PASS"))) ||
						(!(cmd.compare(0, 4, "USER"))) ||
						(!(cmd.compare(0, 1, "/"))));
		}

		void	printChannels(int fd){
			if (_channels.size() != 0){
				for (size_t i = 0; i < _channels.size(); i++){
					send(fd, _channels[i].getName().data(), _channels[i].getName().size(), 0);
					send(fd, " ", 2, 0);
					send(fd, _channels[i].getChannelOperator().getNickname().data(), _channels[i].getChannelOperator().getNickname().size(), 0);
				}
			} else
				sendMessageToReceiver(fd, "Server: ", "No channels to show");
		}

		void    MSG(std::string data, User user){
            std::string receiverNick = cleanString(data.substr(4, (data.size())));
            int endNick = substr_to_first_space_or_end(receiverNick);
            if (endNick == 0)
                sendMessageToReceiver(user.getFd(), "461", " Not enough parameters\n");
            else {
                receiverNick = receiverNick.substr(0, endNick);
                std::cout << receiverNick << std::endl;
                if (isNicknameTaken(receiverNick)){
                    User receiver = getUser(receiverNick);
                    std::string message = cleanString(data.substr(endNick, data.size()));
                    if (message.size() <= 512)
                        sendMessageToReceiver(receiver.getFd(), user.getNickname(), message);
                    else
                        sendMessageToReceiver(user.getFd(), "Server", "Message too long\n");
                } else 
                    sendMessageToReceiver(user.getFd(), "401 no user with nick ", receiverNick);
            }
        }

		bool isOperator(std::string nick, std::string channelOperatorNick) {return (!(nick.compare(channelOperatorNick)));}

};

void sigint(int sign);


#endif