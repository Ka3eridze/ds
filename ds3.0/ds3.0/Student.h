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
    string studentFile;
    LIBirianc& libraryRef;
    StatusQueue& statusQueueRef;

    // ================== ÄÎÁÀÂËÅÍÎ: áåçîïàñíûé ââîä ==================
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
    // ================================================================


    vector<pair<string, string>> loadAllStudents() {
        vector<pair<string, string>> students;
        ifstream fin(studentFile);
        if (!fin.is_open()) return students;

        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string u, p;
            if (getline(ss, u, ',') && getline(ss, p)) {
                students.push_back({ u, p });
            }
        }
        return students;
    }

    void saveAllStudents(const vector<pair<string, string>>& students) {
        ofstream fout(studentFile);
        for (auto& s : students) {
            fout << s.first << "," << s.second << "\n";
        }
    }

    bool registerStudent() {
        vector<pair<string, string>> students = loadAllStudents();
        for (auto& s : students) {
            if (s.first == username) {
                cout << "Username already exists.\n";
                return false;
            }
        }

        students.push_back({ username, password });
        saveAllStudents(students);
        cout << "Student registered successfully!\n";
        return true;
    }

public:
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
            else if (choice == 2) {
                cout << "Create Username: ";
                cin >> username;
                cout << "Create Password: ";
                cin >> password;
                cin.ignore();
                registerStudent();
            }
            else if (choice == 3) {
                return false;
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
    }


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

            try {
                ch = safeInput<int>();
            }
            catch (...) {
                cout << "Invalid option!\n";
                continue;
            }
            cin.ignore();

            switch (ch) {
            case 1:
                libraryRef.displayBooks();
                break;

            case 2: {
                int id;
                cout << "Enter book ID: ";
                try { id = safeInput<int>(); }
                catch (...) { cout << "Invalid ID!\n"; break; }

                cin.ignore();
                Book* b = libraryRef.findBookById(id);
                if (b)
                    cout << "ID: " << b->id << ", Name: " << b->name
                    << ", Author: " << b->author << ", Total: " << b->totalquant << endl;
                else
                    cout << "Book not found.\n";
                break;
            }

            case 3: {
                string name;
                cout << "Enter book name: ";
                getline(cin, name);
                libraryRef.findBookByName(name);
                break;
            }

            case 4: {
                string author;
                cout << "Enter author name: ";
                getline(cin, author);
                libraryRef.findBookByAuthor(author);
                break;
            }

            case 5: {
                int id;
                cout << "Enter book ID to issue: ";
                try { id = safeInput<int>(); }
                catch (...) { cout << "Invalid ID!\n"; break; }
                cin.ignore();
                statusQueueRef.issueBook(id, username);
                break;
            }

            case 6: {
                int id;
                cout << "Enter book ID to return: ";
                try { id = safeInput<int>(); }
                catch (...) { cout << "Invalid ID!\n"; break; }
                cin.ignore();
                statusQueueRef.returnBook(id);
                break;
            }

            case 0:
                cout << "Logging out...\n";
                return;

            default:
                cout << "Invalid choice.\n";
            }
        } while (true);
    }
};
