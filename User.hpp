#ifndef USER_HPP
#define USER_HPP
#include "Server.hpp"

class User {
    public:
        User() {}
        User(int fd) {
            this->_fd = fd;
            this->verified = 0;
            this->userVerified_ = 0;
            this->nickVerified_ = 0;
            this->inChannel = 0;
        }

        std::string getUsername() const { return username_; }
        std::string getNickname() const { return nickname_; }
        std::string getPassword() const { return password_; }
        int         getFd()       const { return this->_fd; }
        int         getUserVerification()       const { return this->userVerified_; }
        int         getNickVerification()       const { return this->nickVerified_; }
        int         getChannelVerification()    const { return this->inChannel; }
//
        void setNickname(std::string nickname) { nickname_ = nickname; } //check if max 9 chars
        void setPassword(std::string password) { password_ = password; }
        void setUsername(std::string username) {username_ = username; }
        void setPort(int fd)                  {_fd = fd;}
        void setUserVerification(int n)        { this->userVerified_ = n; }
        void setNickVerification(int n)        { this->nickVerified_ = n; }
        void setChannelVerification(int n)     { this->inChannel = n; }

        void setVerification(int value) {verified = value;}
        int  getVerification() {return (verified);}

    private:
        int verified;
        int _fd;
        std::string username_;
        std::string nickname_;
        std::string password_;
        int userVerified_;
        int nickVerified_;
        int inChannel;
};

void    clientConsole(User user);

#endif
