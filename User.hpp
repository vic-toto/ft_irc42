#ifndef USER_HPP
#define USER_HPP

class Server;

class User {

    private:
        pollfd      _client;
        int         _fd;
        std::string username_;
        std::string nickname_;
        std::string password_;

        bool        verified;
        bool        userVerified_;
        bool        nickVerified_;
        bool        inChannel;

        std::string whatChannel;
        int noChannels; //limit of 10, check if it should behave like vector or stack
        // std::vector<Channel> channelList;

    public:
        User() {}
        User(pollfd Client, int fd) {
            this->_fd = fd;
            this->verified = false;
            this->userVerified_ = false;
            this->nickVerified_ = false;
            this->inChannel = false;
            this->noChannels = 0;
            this->_client = Client;
        }

        std::string getUsername() const { return username_; }
        std::string getNickname() const { return nickname_; }
        std::string getPassword() const { return password_; }
        int         getFd()       const { return this->_fd; }
        bool         getVerification() {return verified;}
        bool         getUserVerification()       { return userVerified_; }
        bool         getNickVerification()       { return nickVerified_; }
        bool         getChannelVerification()    { return inChannel; }
//
        void setNickname(std::string nickname) { nickname_ = nickname; } //check if max 9 chars
        void setPassword(std::string password) { password_ = password; }
        void setUsername(std::string username) {username_ = username; }
        void setPort(int fd)                  {_fd = fd;}

        void setVerification(bool type)        {this->verified = type;}
        void setUserVerification(bool type)        { this->userVerified_ = type; }
        void setNickVerification(bool type)        { this->nickVerified_ = type; }
        void setChannelVerification(bool type)     { this->inChannel = type; }
        void setInChannel(int n)                {this->inChannel = n;}
        void setWhatChannel(std::string channelName) {this->whatChannel = channelName; }
        int  getInChannel() {return inChannel;} const
        std::string getWhatChannel() const  {return this->whatChannel;}
        pollfd  getPollFd() {return this->_client;}
        //cmds
        bool    channelLimit(){
            if (this->noChannels < 10)
                return (false);
            return true;
        }

        void    increaseChannelNo() {this->noChannels += 1;}
        void    decreaseChannelNo() {this->noChannels -= 1;}

        User    USER(std::string username)
        {
            this->setUsername(username);
            this->setUserVerification(1);
            send(this->getFd(), "\nUsername set to ", 18, 0);
            send(this->getFd(), this->getUsername().data(), username.size(), 0);
            send(this->getFd(), "\n", 2, 0);
            return (*this);
        }

        User    NICK(std::string nickname)
        {
            this->setNickname(nickname.substr(0, nickname.size()));
            this->setNickVerification(1);
            send(this->getFd(), "\nNickname set to ", 17, 0);
            send(this->getFd(), this->getNickname().data(), nickname.size(), 0);
            send(this->getFd(), "\n", 2, 0);
            return (*this);
        }


};

void    clientConsole(User user);
#endif
