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
#include "colors.h"

class	Server{

	public:

		Server(int const & port, std::string const & password);
		~Server(void);
		int		init_server();
		void	run_server();
		void	stop_server();

	private:
		Server(Server const & other);
		Server &operator=(Server const & rhs);

		static const int		MAX_CLIENTS = 64;
		static const ssize_t	BUFFER_SIZE = 1024;

		std::string				serverName;
		std::string				password;
		std::string				operatorPassword;
		std::string				hostname;
		std::string				version;
		std::string				user_modes;
		std::string				channel_modes;
		std::string				motd;
		struct sockaddr_in		server_address;
		struct sockaddr_in		client;
		struct pollfd			clients_pollfd[MAX_CLIENTS];
		socklen_t				client_number;
		int						server_socket;
		int						clients_size;
		int						return_accept;
		int						port;	
};

#endif