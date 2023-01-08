#include "Server.hpp"

Server::Server(int const & port, std::string const & password)
{
    this->port = port;
    this->password = password;
    this->serverName = "ircserv";
    this->motd = "Welcome to Victoria's IRC server.";
	this->version = "ver. 0.0.1";
	this->user_modes = "i";
	this->channel_modes = "ciao";
	this->operatorPassword = "password";
}

Server::Server(Server const & other) :hostname(other.hostname)
{}

Server & Server::operator=(Server const & rhs)
{
	this->~Server();
	new (this) Server(rhs);
	return (*this);
}

Server::~Server(void)
{
	std::cout << COLOR_GREEN << "IRC Server stopped and shut down...\n" << COLOR_DEFAULT;
}