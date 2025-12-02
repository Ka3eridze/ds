#pragma once
#ifndef ADMIN_H
#define ADMIN_H

#include "Role.h"
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "Library.h"
#include "Status_Queue.h"
using namespace std;

class Admin : public Role {
private:
    LIBirianc& libraryRef;
    StatusQueue& statusQueueRef;
    string adminFile;

    bool loadCredentials() {
        ifstream fin(adminFile);
        if (!fin.is_open()) return false;
        if (getline(fin, username) && getline(fin, password)) {
            if (!username.empty() && username.back() == '\r') username.pop_back();
            if (!password.empty() && password.back() == '\r') password.pop_back();
            return true;
        }
        return false;
    }

    void saveCredentials() const {
        ofstream fout(adminFile);
        if (fout.is_open()) {
            fout << username << "\n" << password << "\n";
            fout.close();
        }
    }

public:
    Admin(LIBirianc& lib, StatusQueue& statusQueue, const string& file = "admin.txt")
        : libraryRef(lib), statusQueueRef(statusQueue), adminFile(file) {
    }

    bool login() override {
        if (!loadCredentials()) {
            cout << "\n=== FIRST TIME ADMIN SETUP ===\n";
            cout << "Create Admin Username: "; cin >> username;
            cout << "Create Admin Password: "; cin >> password;
            saveCredentials();
            cout << "Admin registered successfully!\n";
        }

        cout << "\n=== ADMIN LOGIN ===\n";
        string u, p;
        cout << "Username: "; cin >> u;
        cout << "Password: "; cin >> p;

        if (u == username && p == password) {
            cout << "Admin login successful. Welcome, " << username << "!\n";
            return true;
        }
        cout << "Invalid credentials.\n";
        return false;
    }

    void showMenu() override {
        int ch;
        do {
            cout << "\n";
            cout << string(80, '=') << "\n";
            cout << "                        ADMIN MENU                        \n";
            cout << string(80, '=') << "\n";
            cout << "1. Add Book                2. Delete Book\n";
            cout << "3. Display All Books       4. Search Book by ID\n";
            cout << "5. Student Management      6. Book Issue Details\n";
            cout << "0. Exit\n";
            cout << string(80, '-') << "\n";
            cout << "Choose: ";
            cin >> ch;

            switch (ch) {
            case 1: {
                cout << "\n--- ADD NEW BOOK ---\n";
                Book b;
                cout << "Enter book ID: "; cin >> b.id;
                cin.ignore();
                cout << "Title: "; getline(cin, b.name);
                cout << "Author: "; getline(cin, b.author);
                cout << "Total copies: "; cin >> b.totalquant;
                libraryRef.addBook(b);
                statusQueueRef.setTotalCopies(b.id, b.totalquant);
                cout << "Book added successfully!\n";
                break;
            }
            case 2: {
                int id;
                cout << "Enter book ID to delete: ";
                cin >> id;
                libraryRef.removeBook(id);
                cout << "Book ID " << id << " removed from catalog.\n";
                break;
            }
            case 3: {
                cout << "\n" << string(130, '=') << "\n";
                cout << left
                    << setw(6) << "ID"
                    << setw(40) << "TITLE"
                    << setw(25) << "AUTHOR"
                    << setw(8) << "TOTAL"
                    << setw(8) << "AVAIL"
                    << setw(25) << "ISSUED TO"
                    << "QUEUE\n";
                cout << string(130, '-') << "\n";

                cout << "BOOK CATALOG:\n";
                libraryRef.displayBooks();


                cout << "\n" << string(90, '-') << "\n";
                cout << "DETAILED STATUS:\n";
                statusQueueRef.showAllStatus();

                cout << string(130, '=') << "\n";
                break;
            }
            case 4: {
                int id;
                cout << "Enter book ID: ";
                cin >> id;
                Book* b = libraryRef.findBookById(id);
                if (b) {
                    cout << "\nFOUND: " << b->name << " by " << b->author << "\n";
                    statusQueueRef.showBookStatus(id);
                }
                else {
                    cout << "Book not found.\n";
                }
                break;
            }
            case 5: {
                cout << "\n=== STUDENT MANAGEMENT ===\n";
                cout << "1. View all students  2. Register new student\nChoose: ";
                int sub;
                cin >> sub;
                if (sub == 1) {
                    cout << "\n--- ALL REGISTERED STUDENTS ---\n";
                    ifstream f("student.csv");
                    if (!f.is_open() || f.peek() == EOF) {
                        cout << "No students registered yet.\n";
                    }
                    else {
                        string line;
                        int cnt = 1;
                        while (getline(f, line)) {
                            if (line.empty()) continue;
                            stringstream ss(line);
                            string sid, user, pass;
                            getline(ss, sid, ',');
                            getline(ss, user, ',');
                            cout << cnt++ << ". ID: " << sid << " | Username: " << user << endl;
                        }
                    }
                    f.close();
                }
                else if (sub == 2) {
                    string sid, user, pass;
                    cout << "Student ID: "; cin >> sid;
                    cout << "Username: "; cin >> user;
                    cout << "Password: "; cin >> pass;
                    ofstream f("student.csv", ios::app);
                    if (f.is_open()) {
                        f << sid << "," << user << "," << pass << "\n";
                        f.close();
                        cout << "Student " << user << " registered successfully!\n";
                    }
                }
                break;
            }
            case 6: {
                int id;
                cout << "Enter book ID: ";
                cin >> id;
                Book* b = libraryRef.findBookById(id);
                if (!b) {
                    cout << "Book not found.\n";
                    break;
                }
                cout << "\n=== " << b->name << " by " << b->author << " ===\n";
                cout << "Total copies: " << b->totalquant << "\n";
                statusQueueRef.showBookStatus(id);
                break;
            }
            case 0: {
                cout << "\n";
                cout << string(60, '=') << "\n";
                cout << "   Thank you, Admin " << username << "!\n";
                cout << "   Logging out safely...\n";
                cout << string(60, '=') << "\n\n";
                break;
            }
            default:
                cout << "Invalid choice. Please try again.\n";
            }
        } while (ch != 0);
    }
};

#endif
