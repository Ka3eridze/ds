//U2420004
//Abdikadirova Malikaxon
#pragma once
#include <string>
using namespace std;
// Abstract role class 
class Role {
protected:
    string username; // Stores the username of the user (admin/student)
    string password; // Stores the password of the user
public:

    // Pure virtual function for login
    // Each derived class (Admin/Student) must implement its own login method
    virtual bool login() = 0;

    // Pure virtual function to display a menu
    // Admin and Student will implement different menus
    virtual void showMenu() = 0;

    // Getter for username
     // Getter for password
    string getUsername() { return username; }
    string getPassword() { return password; }

};
