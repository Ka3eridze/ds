#pragma once
#include <string>
using namespace std;

class Role {
protected:
    string username;
    string password;
public:
    virtual bool login() = 0;
    virtual void showMenu() = 0;

    string getUsername() { return username; }
    string getPassword() { return password; }
};