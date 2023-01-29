#include "Server.hpp"
#include "tools.hpp"
// STEP 1

Server::Server(int const & port, std::string const & password)
{
	this->__port = port;
	this->__pwd = password;
	std::time_t result = std::time(nullptr);
    this->created_at = std::asctime(std::localtime(&result));
    std::cout << COLOR_YELLOW << "Initialising server - - - created @ " << this->created_at << std::endl << COLOR_DEFAULT;
}

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

int Server::go(int port)
{
    int server_socket;
    int yes = 1;
    sockaddr_in sock;

	//setting up what IP and port server will listen on
    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    int addr = inet_pton(AF_INET, "127.0.0.1", &sock.sin_addr);
    if (addr <= 0) {
    std::cerr << "Invalid IP address" << std::endl;
    return 1;
    }
	//creating server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        throw std::runtime_error(strerror(errno));
        return 1;
    }
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	//binding server to socket address
    if (bind(server_socket, (sockaddr*)&sock, sizeof(sock)) < 0)
    {
        close(server_socket);
        throw std::runtime_error(strerror(errno));
        return 1;
    }
	//set to listening mode
    if (listen(server_socket, SOMAXCONN) < 0)
    {
        std::cerr << "Error listening: " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "IRC Server successfully initialized and listening on " << inet_ntoa(sock.sin_addr) << ":" << ntohs(sock.sin_port) << std::endl;
	//now we wait for a client
    while (true)
    {
        sockaddr_in client_sock;
        socklen_t client_sock_len = sizeof(client_sock);
		//accept incoming connections
        int client_socket = accept(server_socket, (sockaddr*)&client_sock, &client_sock_len);
        if (client_socket < 0)
        {
            std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
            continue;
        }
        std::cout << "Incoming connection from " << inet_ntoa(client_sock.sin_addr) << ":" << ntohs(client_sock.sin_port) << std::endl;
        std::cout << "Secure connection and confirm" << std::endl;
		handleClient(client_socket);
	}
	
}

bool Server::verifyPassword(std::string password)
{
    password.erase(0,4); 
    password = trimWhitespace(password);
    return (password == getPassword());
}

void Server::process_command(std::string data, int client_socket) 
{
    User user;
    if (!(data.compare(0, 4, "PASS"))){ // add if client is authenticated yet to unlock user and other cmds, to do
        if (!(verifyPassword(data))) {
            std::cout << COLOR_RED << "wrong password" << std::endl;
            send(client_socket, "Password rejected\n", 17, 0);
            // close the connection or take additional measures for rejected clients
             }
            std::cout << "Client password accepted" << std::endl;
            std::string message = COLOR_GREEN "Password accepted" COLOR_DEFAULT "\n";
            send(client_socket, message.c_str(), message.length(), 0);
            //User user;
            user.setVerification(0);
            //send(client_socket, "Create User with USER cmd\n", 26, 0);
            // additional code to handle authenticated client goes here
    }
    else if (!(data.compare(0, 4, "USER")))
        user.setUsername(client_socket, data);
        

	     else {
        std::cout << "Invalid command" << std::endl;
    }
}

void Server::handleClient(int client_socket) {

    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(client_socket, &read_set);

    while (true) {
        int ready = select(client_socket + 1, &read_set, nullptr, nullptr, nullptr);
        if (ready < 0) {
            std::cerr << "Error in select: " << strerror(errno) << std::endl;
            continue;
        }

        if (FD_ISSET(client_socket, &read_set)) {
            char buffer[4096];
            int received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (received < 0) {
                std::cerr << "Error in recv: " << strerror(errno) << std::endl;
                continue;
            }
            std::string data(buffer, received);
            process_command(data, client_socket);
        }
    }
}


//
//void Server::handleCmds(int client_socket) {
//
//    fd_set read_set;
//    FD_ZERO(&read_set);
//    FD_SET(client_socket, &read_set);
//
//    while (true) {
//        int ready = select(client_socket + 1, &read_set, nullptr, nullptr, nullptr);
//        if (ready < 0) {
//            std::cerr << "Error in select: " << strerror(errno) << std::endl;
//            continue;
//        }
//
//        if (FD_ISSET(client_socket, &read_set)) {
//            char buffer[4096];
//            int received = recv(client_socket, buffer, sizeof(buffer), 0);
//            if (received < 0) {
//                std::cerr << "Error in recv: " << strerror(errno) << std::endl;
//                continue;
//            }
//
//            std::string data(buffer, received);
//            process_command(data, client_socket);
//        }
//    }
//}
//
//
//
//
//int Server::getUser(int client_socket, User& user)
//{
//
//	//Receive password
//	char password_buf[1024];
//	int recv_bytes_pass = recv(client_socket, password_buf, sizeof(password_buf), 0);
//		std::string password;
//	if (recv_bytes_pass > 0){
//		password.assign(password_buf, recv_bytes_pass);
//		user.setPassword(password);
//
//		//verify password before continuing
//		if (password == this->__pwd) 
//        std::cout << COLOR_GREEN << "\n PASSWORD OK" << std::endl;
//		else {
//    	    std::cout << "Error: incorrect password." << std::endl;
//			return (1);
//    	}
//
//	}
//    // Receive username
//    //char username_buf[1024];
//    //int recv_bytes = recv(client_socket, username_buf, sizeof(username_buf), 0);
//    //std::string username;
//    //if (recv_bytes > 0) {
//    //    username.assign(username_buf, recv_bytes);
//    //    user.setUsername(username);
//    //} else {
//    //    std::cout << "Error receiving username." << std::endl;
//    //    return 1;
//    //}
//
//    // Receive nickname
//   // char nickname_buf[1024];
//   // recv_bytes = recv(client_socket, nickname_buf, sizeof(nickname_buf), 0);
//   // std::string nickname;
//   // if (recv_bytes > 0) {
//   //     nickname.assign(nickname_buf, recv_bytes);
//   //     user.setNickname(nickname);
//   // } else {
//   //     std::cout << "Error receiving nickname." << std::endl;
//   //     return 1;
//   // }
//	//std::cout << COLOR_GREEN << "\n User " << user.getUsername() << " with nickname " << user.getNickname() << " connected successfully 👌🏼" << std::endl;
//	return (0);
//}
//