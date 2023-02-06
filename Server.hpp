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

#include <map>
#include <queue>
#include <vector>
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
		
		//Setters
		void	setServerSocket(int s){this->server_socket = s;}

		//Security
		bool 		verifyPassword(std::string password) {return (password.compare(0, this->getPassword().size(), this->getPassword())); };

		//Client related
		bool		isUser(std::string nick)
		{
			for (int i = 0; i < getNumberUsers(); i++)
			{
				std::cout << "checking user nick '" << _users[i].getNickname() << "'" <<std::endl;
				std::cout << "checking real nick '" << nick << "'" <<std::endl;
				if (!(_users[i].getNickname().compare(nick)))
					return true;
			}
			return false;
		}
		int			getNumberUsers() {return (_users.size()); }
		void		handleClientMessage(std::string data, int client_fd);
		//void		process_command(std::string data, int client_socket);
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
			std::cout << "getting channem by name '" << _channels[i].getName() << "'" << std::endl;
            if (!(_channels[i].getName().compare(name))) {
                return _channels[i]; }
			}
			return (Channel());
		}


};

void sigint(int sign);

#endif