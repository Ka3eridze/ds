#pragma once
#include "User.h"
#include "Library.h"
#include "status_Queue.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>
using namespace std;

class Student : public User {
private:
    string studentFile;  // Path to file storing student credentials
    LIBirianc& libraryRef;  // Reference to the Library object
    StatusQueue& statusQueueRef;  // Reference to the StatusQueue object

    // Template method for safe typed input.
    // If user enters a wrong type (e.g. letter instead of number), it clears input
    // but throws an exception to let the outer code handle it.
    template<typename T>
    T safeInput() {
        T value;
        while (true) {
            if (cin >> value) return value;

            cin.clear();
            cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
            throw invalid_argument("Invalid input type");
        }
    }


    // Loads all students from file.
    // Each line is "username,password".
    vector<pair<string, string>> loadAllStudents() {
        vector<pair<string, string>> students;
        ifstream fin(studentFile);
        if (!fin.is_open()) return students;

        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string u, p;
            // Reads username until comma, then password
            if (getline(ss, u, ',') && getline(ss, p)) {
                students.push_back({ u, p });
            }
        }
        return students;
    }
    // Saves all students back to the file.
    void saveAllStudents(const vector<pair<string, string>>& students) {
        ofstream fout(studentFile);
        for (auto& s : students) {
            fout << s.first << "," << s.second << "\n";
        }
    }

    // Register a new student.
    // Ensures username is unique before saving.
    bool registerStudent() {
        vector<pair<string, string>> students = loadAllStudents();
        // Check for duplicate username
        for (auto& s : students) {
            if (s.first == username) {
                cout << "Username already exists.\n";
                return false;
            }
        }
        // Add and save
        students.push_back({ username, password });
        saveAllStudents(students);
        cout << "Student registered successfully!\n";
        return true;
    }

public:
    // Constructor: receives references to library and queue, plus path to student file
    Student(LIBirianc& lib, StatusQueue& statusQueue, const string& file)
        : libraryRef(lib), statusQueueRef(statusQueue), studentFile(file) {
    }

    bool login() override {
        int choice = 0;
        while (true) {
            cout << "\n=== STUDENT LOGIN ===\n"
                << "1. Log in\n"
                << "2. Register\n"
                << "3. Exit \n"
                << "Choose: ";


            // Safe input for menu choice

            try {
                choice = safeInput<int>();
            }
            catch (...) {
                cout << "Invalid choice.\n";
                continue;
            }
            cin.ignore();

            if (choice == 1) {
                string u, p;
                cout << "Username: ";
                cin >> u;
                cout << "Password: ";
                cin >> p;
                cin.ignore();

                auto students = loadAllStudents();
                // Check credentials
                for (auto& s : students) {
                    if (s.first == u && s.second == p) {
                        username = u;
                        password = p;
                        cout << "Login successful. Welcome, " << username << "!\n";
                        return true;
                    }
                }
                cout << "Invalid username or password.\n";
            }
            // registration
            else if (choice == 2) {
                cout << "Create Username: ";
                cin >> username;
                cout << "Create Password: ";
                cin >> password;
                cin.ignore();
                registerStudent();
            }
            // exit
            else if (choice == 3) {
                return false;
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
    }

    // student menu implemntation
    void showMenu() override {
        int ch = 0;
        do {
            cout << "\n===============================================================================\n";
            cout << "                                 STUDENT MENU                                 \n";
            cout << "===============================================================================\n";
            cout << "1. Show All Books                4. Search by Author\n";
            cout << "2. Search by ID                  5. Issue Book\n";
            cout << "3. Search by Name                6. Return Book\n";
            cout << "0. Logout\n";
            cout << "-------------------------------------------------------------------------------\n";
            cout << "Choose: ";
            // this for safe input
            try {
                ch = safeInput<int>();
            }
            catch (...) {
                cout << "Invalid option!\n";
                continue;
            }
            cin.ignore();

            switch (ch) {
            case 1:  // Display all books
                libraryRef.displayBooks();
                break;

            case 2: { // Search by ID
                int id;
                cout << "Enter book ID (0 to cancel): ";
                try { id = safeInput<int>(); }
                catch (...) { cout << "Invalid ID!\n"; break; }

                if (id == 0) {
                    cout << "Cancelled.\n";
                    break;
                }

                Book* b = libraryRef.findBookById(id);
                if (b)
                    cout << "ID: " << b->id << "  |  Title " << b->name
                    << "  |  Author: " << b->author << "  |  Total: " << b->totalquant << endl;
                else
                    cout << "Book not found.\n";
                break;
            }

            case 3: {  // Search by book name
                string name;
                cout << "Enter book  (0 to cancel): ";
                getline(cin, name);

                if (name == "0") {
                    cout << "Cancelled.\n";
                    break;
                }

                libraryRef.findBookByName(name);
                break;
            }

            case 4: {   // Search by author
                string author;
                cout << "Enter author name (0 to cancel): ";
                getline(cin, author);

                if (author == "0") {
                    cout << "Cancelled.\n";
                    break;
                }

                libraryRef.findBookByAuthor(author);
                break;
            }

            case 5: {  // Issue book
                int id;
                cout << "Enter book ID to issue (0 to cancel): ";
                try { id = safeInput<int>(); }
                catch (...) { cout << "Invalid ID!\n"; break; }

                if (id == 0) {
                    cout << "Cancelled.\n";
                    break;
                }

                statusQueueRef.issueBook(id, username);
                break;
            }

            case 6: {   // Return book
                int id;
                cout << "Enter book ID to return (0 to cancel): ";
                try { id = safeInput<int>(); }
                catch (...) { cout << "Invalid ID!\n"; break; }

                if (id == 0) {
                    cout << "Cancelled.\n";
                    break;
                }

                statusQueueRef.returnBook(id);
                break;
            }

            case 0:  // Logout
                cout << "Logging out...\n";
                return;

            default:
                cout << "Invalid choice.\n";
            }
        } while (true);
    }
};

