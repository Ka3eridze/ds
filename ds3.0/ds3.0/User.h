//U2420004
//Abdikadirova Malikaxon
#ifndef USER_H
#define USER_H

#include <string>

class User {
protected:
    std::string username;
    std::string password;

public:
    User() = default;
    User(const std::string& u, const std::string& p) : username(u), password(p) {}

    virtual ~User() = default;

    const std::string& getUsername() const { return username; }
    const std::string& getPassword() const { return password; }

    void setUsername(const std::string& u) { username = u; }
    void setPassword(const std::string& p) { password = p; }

    // pure virtuals
    virtual bool login() = 0;
    virtual void showMenu() = 0;
};


#endif // USER_H
