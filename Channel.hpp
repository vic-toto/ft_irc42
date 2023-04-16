#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <iterator>

//CHANNEL - named group of one or more clinets which all receive message addressed to channel 
//        - names are str with & or #
//        - max name len 200
//        - name cannot have white spaces, ascii 7 or ','
//        - max 10 channels per USER
//        - +i means you can join channel only via invite
//        - has 'operator' which is the user owner of channel 
//        - channel has a mode
//        - has a ban list of users that cannot access
//        - can have a password

class Channel
{
private:
    int                 _invitation;
    std::string         _password;
    User                channelOperator;
    std::vector<User>   blackList;
    std::vector<User>   usersInChannel;
    std::string         name;

    /* data */
public:
    Channel() {}
    Channel(std::string name) {this->name = name;};
    Channel(std::string name, std::string password, User channelOperator);
    Channel(std::string name, std::string password, User channelOperator, int invite);
    ~Channel() {};


    void    setName(std::string name) {this->name = name;}
    void    setChannelOperator(User user) {this->channelOperator = user;}
    void    addUser(User user) {usersInChannel.push_back(user);}
    
    User    getChannelOperator() {return this->channelOperator;}
    void removeUser(User &user) {
        for (std::vector<User>::iterator it = usersInChannel.begin(); it != usersInChannel.end(); it++) {
            if (it->getNickname() == user.getNickname()) {
                // If a user with the same nickname is found, remove them from the list
                usersInChannel.erase(it);
                break;
            }
        }
    }

	std::vector<User>& getUsersInChannel() { return usersInChannel; }
    std::string getName() const {return this->name;}
    int         getNoUsersInChannel() {return usersInChannel.size();}
    bool	getblackList(std::string nick){
			for (unsigned long i = 0; i < blackList.size(); i++) {
      		if (blackList[i].getNickname() == nick)
      		  return true;
			}
			return false;
		}
};




#endif
