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
#include "User.hpp"

# define SERV_NAME std::string("VIRC_SERVER")
# define UMODES std::string("ov") // available user _modes
# define CMODES std::string("bkomstnv") // available channel _modes

class	Server{

	public:

		Server(int const & port, std::string const & password);
		~Server(void);
		int		init();
		void	run();
		void	stop_server();
		int		getUser(int client_socket, User& user);

	private:
		Server &operator=(Server const & rhs);

		static const int		MAX_CLIENTS = 64;
		static const ssize_t	BUFFER_SIZE = 1024;

		std::string				__pwd;
		std::string				__port;
		std::string				created_at;
		int						listen_socket;
		fd_set					current_sockets;
		std::vector<User>		clients_;
};

#endif