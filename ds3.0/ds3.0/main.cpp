//U2410003 
// ABDIJABBOROVA LAZIZA

//U2420004
//Abdikadirova Malikaxon
#include <iostream>  // 
#include <limits> //numeric_limits
#include "Library.h" // for library management 
#include "Admin.h" //for admin role
#include "Student.h" //for student role
#include "Book.h" //for book structure
#include "status_Queue.h" // for status Queue

using namespace std;
 
int main() {
    LIBirianc lib;  // reference to library
    StatusQueue statusQueue; // reference to status queue

 
    lib.loadBooks("books.csv"); // load books from CSV
    statusQueue.loadFromCSV("status.csv"); // load status from CSV

    int roleChoice; // to choose role
    
    //menu loop
    while (true) {
        cout << "\n========================================\n";
        cout << "        Library Management System       \n";
        cout << "========================================\n\n";
    //role selection
        cout << "Select Role:\n";
        cout << " 1. Admin\n";
        cout << " 2. Student\n";
        cout << " 0. Exit\n";
        cout << "Choice: ";
         
        // Validate input (must be a number)
        if (!(cin >> roleChoice)) {  
            cin.clear();// Clear the error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); //avoid infinite loop
            cout << "\nInvalid input! Enter a number.\n";
            continue; // Return to menu
        }
         // Remove any extra characters after number (like ENTER)
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        if (roleChoice == 1) {
             // Create Admin object
            Admin admin(lib, statusQueue, "admin.txt");
            if (admin.login()) {
                
                admin.showMenu(); // Open Admin menu
            }
            else {
                cout << "\nInvalid credentials. Returning to main menu...\n";
            }
        }
        else if (roleChoice == 2) {
            // Create Student object
            Student student(lib, statusQueue, "student.csv");
            // Login process
            if (student.login()) {   
                student.showMenu(); // Open Student menu
            }
            else {
                cout << "\nInvalid credentials. Returning to main menu...\n";
            }
        }
        else if (roleChoice == 0) {
             // Exit program
            cout << "\nExiting program... Goodbye!\n";
            break;
        }
        else {
            cout << "\nInvalid choice. Try again.\n";
        }
    }

 // Save data before exiting
    lib.saveBooks("books.csv");
    statusQueue.saveToCSV("status.csv");

    return 0;
}

