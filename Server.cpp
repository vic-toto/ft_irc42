#include "Server.hpp"
#include "clientMessages.hpp"
// STEP 1


int Server::start(int port)
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
	setServerSocket(server_socket);
	//now we wait for a client
	return (0);
}

void	Server::handleClientMessage(std::string data, int client_fd) 
{
    User user = getUser(client_fd);
    std::cout << "data str incoming  " << data << std::endl;
    if(is_char_or_digit(data)) {
        if (isCmd(data)){
            if (!(data.compare(0, 4, "/msg")) && (user.getNickVerification())){
                MSG(data, user);
                //std::string receiverNick = cleanString(data.substr(4, (data.size())));
                //int endNick = substr_to_first_space_or_end(receiverNick);
                //if (endNick == 0)
                //    sendMessageToReceiver(user.getFd(), "Server", "Error, missing message \n");
                //else {
                //    receiverNick = receiverNick.substr(0, endNick);
                //    std::cout << receiverNick << std::endl;
                //    if (isUsernameTaken(receiverNick)){
                //        User receiver = getUser(receiverNick);
                //        std::string message = cleanString(data.substr(receiverNick.size(), data.size()));
                //        if (message.size() >= 512)
                //            sendMessageToReceiver(receiver.getFd(), user.getNickname(), cleanString(message));
                //        else
                //            sendMessageToReceiver(user.getFd(), "Server", "Message too long\n");
                //    } else 
                //        sendMessageToReceiver(user.getFd(), "Error, no user with nick ", receiverNick);
                //}
            }
            if (!(data.compare(0, 4, "PASS"))) { // add if client is authenticated yet to unlock user and other cmds, to do
                std::string message = cleanString(data.substr(5, (data.size())));
                if (!(user.getVerification())){
                    if (!(verifyPassword(message))){
                        user.setVerification(1);
                        sendMessageToReceiver(client_fd, "\x1b[32mServer: ", "Password Ok\x1b[0m\n");
                        updateUser(user);
                        }
                    } 
            } else if (user.getVerification()) {  
                if (!(data.compare(0, 4, "USER")) && (!user.getUserVerification())) {
                    std::string message = cleanString(data.substr(5, (data.size())));
                    if (message.empty() || message[0] == '#' || message.size() > 200)
                        sendMessageToReceiver(user.getFd(), "\x1b[31mServer :", "Invalid Username\x1b[0m\n\n");
                    else if (isUsernameTaken(message))
                        sendMessageToReceiver(user.getFd(), "\x1b[31mServer :", "Username already in use\x1b[0m\n\n");
                    else {
                        user.USER(message);
                        updateUser(user);
                    }
                } else if (!(data.compare(0, 5, "/nick")) && (user.getUserVerification())) {
                        std::string message = cleanString(data.substr(6, (data.size())));
                        if (message.empty() || message[0] == '#' || message.size() > 9)
                            sendMessageToReceiver(user.getFd(), "\x1b[31mServer :", "Invalid nickname\x1b[0m\n\n");
                        else if (isNicknameTaken(message))
                            sendMessageToReceiver(user.getFd(), "\x1b[31mServer :", "Nickname already in use\x1b[0m\n\n");
                        else {
                            user.NICK(message);
                            updateUser(user);
                        }
                } else if (!(data.compare(0, 5, "/list"))&& (user.getNickVerification())){
                        printChannels(user.getFd());
                } else if (!(data.compare(0, 5, "/join")) && (!(user.getInChannel()) && ((user.getNickVerification())))) {
                        std::string message = cleanString(data.substr(5, (data.size())));
                        if (message.empty() || message[0] != '#' || message.size() > 200 || isSpace(message))
                            sendMessageToReceiver(user.getFd(), "Server: ", "Invalid channel name\n");
                        else if (channelExists(message)){
                            std::cout << "channel exists\n";
                            Channel channel = getChannel(message);
                            if (!(channel.getblackList(user.getNickname()))){
                                    std::cout << "arrivo\n";
                                if (!(user.channelLimit())){
                                    channel.addUser(user);
                                    user.setInChannel(1);
                                    user.setWhatChannel(channel.getName());
                                    updateUser(user);
                                    updateChannel(channel);
                                    sendMessageToChannel(user, " has joined the channel.\n"); // TODO: check \n
                                    // need to add a check for nickname changes. 
                                    // if nick changes, it has to change in all chan
                                    }
                                }
                                else
                                    sendMessageToReceiver(user.getFd(), "Server", "Channel limit reached\n");
                        } else {
                            Channel *channel = new Channel(message);
                            channel->setName(message.substr(0, message.size()));
                            channel->setChannelOperator(user);
                            channel->addUser(user);
                            user.setInChannel(1);
                            user.setWhatChannel(channel->getName());
                            user.increaseChannelNo();
                            updateUser(user);
                            addChannel(*channel);
                        }
                } else if (user.getInChannel() && (user.getNickVerification())){
                        Channel channel = getChannel(user.getWhatChannel());
                    if (!(data.compare(0, 5, "/part")) || (!(data.compare(0, 6, "/leave")))){
                        sendMessageToChannel(user, " has left the channel.\n"); // TODO: check \n
                        channel.removeUser(user);
                        user.setInChannel(0);
                        user.setWhatChannel("");
                        updateUser(user);
                        // have to add check if no users in channel, delete channel
                    } else if (!(data.compare(0, 5, "/kick")) && (isOperator(user.getNickname(), channel.getChannelOperator().getNickname()))) {
                        std::string userToRemove(data.substr(5, data.size()));
                        userToRemove = cleanString(userToRemove);
                        std::cout << "here usetoremove '" << userToRemove << "'\n";
                        User removeUser = getUser(userToRemove);
                        sendMessageToChannel(removeUser, " was kicked from the channel.\n");
                        channel.removeUser(removeUser);
                        removeUser.setInChannel(0);
                        removeUser.setWhatChannel("");
                        updateUser(removeUser);
                        send(removeUser.getFd(), "\n", 1, 0);
                    }
                    updateChannel(channel);
                
                } else if (!(data.compare(0, 5, "/quit")) && (user.getNickVerification()))
                    sigint(SIGINT); // add delete users and other memory shit
                 else if (user.getNickVerification())
                    sendMessageToReceiver(client_fd, "Server :", "Invalid Command\n");
            }
        }   
    }
    if (user.getInChannel())
            sendMessageToChannel(user, data);
    clientConsole(user);
}



void	Server::go()
{
	std::vector<pollfd> fds;
	struct pollfd pfd;
	pfd.fd = this->server_socket;
	pfd.events = POLLIN | POLLOUT;
	pfd.revents = 0;
	fds.push_back(pfd);
    
	while (true)
    {
        int ret = poll(fds.data(), fds.size(), 0);
        if (ret < 0)
        {
            std::cerr << "Error polling: " << strerror(errno) << std::endl;
            continue;
        }
        if (fds[0].revents & POLLIN)
        {
            sockaddr_in client_sock;
            socklen_t client_sock_len = sizeof(client_sock); //accept incoming connections
           	int  client_socket = accept(this->server_socket, (sockaddr*)&client_sock, &client_sock_len);
            if (client_socket < 0){
                std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
                continue;
            }
            int flags = fcntl(client_socket, F_GETFL, 0);
            fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);
			struct pollfd newClient;
			newClient.fd = client_socket;
			newClient.events = POLLIN | POLLOUT;
			newClient.revents = 0;
			fds.push_back(newClient);
            std::cout << "Incoming connection from " << inet_ntoa(client_sock.sin_addr) << ":" << ntohs(client_sock.sin_port) << std::endl;
            std::cout << "Secure connection and confirm" << std::endl;
            User* user = new User(newClient, newClient.fd);
            this->addUser(*user);
            if (!(user->getVerification())){
                if (checkReadyToWrite(user->getFd())){
                    send(user->getFd(), WELCOME, 84, 0);}
            }
        }
        for (std::vector<pollfd>::size_type i = 1; i < fds.size(); i++)
        { 
            if (fds[i].revents & POLLIN)
            {
                char buffer[1024];
                int ret = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (ret <= 0)
                {
                    std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    i--;
                    continue;
                }
                std::string message(buffer, ret);
                std::cout << message << std::endl;
                cleanString(message);
                //
                int check = poll(&fds[i], 1, 0);
                if (check < 0)
                {
                    std::cerr << "Error polling: " << strerror(errno) << std::endl;
                    continue;
                }
                if (fds[i].revents & POLLOUT)
                    this->handleClientMessage(message, fds[i].fd);
            
		    }
	    }
    }
}

void    clientConsole(User user)
{
    if (!(user.getVerification())){
        send(user.getFd(), WELCOME, 84, 0);
        return ;
    } else {
        if (!(user.getUserVerification())){
            send(user.getFd(), "\nPlease set username with USER your_username\n", 46, 0);
            return ; }
        if (!(user.getNickVerification())) {
            send(user.getFd(), "\nPlease set nickname with /nick your_nickname\n", 46, 0);
            return ; 
        }
    }
    if (user.getInChannel()){
        send(user.getFd(), user.getWhatChannel().data(), user.getWhatChannel().size(), 0);
        send(user.getFd(), ": ", 3, 0);}
    send(user.getFd(), user.getNickname().data(), user.getNickname().size(), 0);
    send(user.getFd(), " - ", 4, 0);
}
