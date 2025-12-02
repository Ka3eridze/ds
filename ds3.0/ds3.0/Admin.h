#ifndef ADMIN_H
#define ADMIN_H

#include "Role.h"
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include "Library.h"
#include "Status_Queue.h"
using namespace std;

class Admin : public Role {
private:
    LIBirianc& libraryRef;
    StatusQueue& statusQueueRef;
    string adminFile;

    template<typename T>
    T safeInput(const string& prompt = "") {
        T value;
        if (!prompt.empty()) cout << prompt;
        while (!(cin >> value)) {
            cout << "Invalid input! Enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
    }

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
        int ch = -1;
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

            ch = safeInput<int>();

            switch (ch) {
            case 1: {
                while (true) {
                cout << "\n--- ADD NEW BOOK ---\n";

                    int nextId = 1;
                    while (libraryRef.findBookById(nextId) != nullptr) nextId++;

                    cout << "Suggested ID: " << nextId << " (next available)\n";
                    cout << "Enter book ID (press Enter to use " << nextId << ", or 0 to cancel): ";

                    string input;
                    getline(cin, input);

                    // Отмена по 0 или пустому вводу + 0
                    if (input == "0") {
                        cout << "Add book cancelled.\n";
                        break;
                    }

                    int bookId = nextId;
                    if (!input.empty()) {
                        stringstream ss(input);
                        if (!(ss >> bookId) || bookId <= 0) {
                            cout << "Invalid ID! Using suggested ID: " << nextId << "\n";
                            bookId = nextId;
                        }
                    }


                    // Проверка дубликата
                    if (libraryRef.findBookById(bookId) != nullptr) {
                        cout << "ERROR: Book with ID " << bookId << " already exists!\n";
                        cout << "1. Try another ID    0. Cancel adding\nChoose: ";
                        if (safeInput<int>() == 0) {
                            cout << "Add book cancelled.\n";
                            break;
                        }
                        continue; // попробовать снова
                    }

                Book b;
                    b.id = bookId;

                    cout << "Title: ";
                    if (!getline(cin, b.name) || b.name.empty()) {
                        cout << "Title cannot be empty! Cancelling...\n";
                        break;
                    }
                    cout << "Author: ";
                    if (!getline(cin, b.author) || b.author.empty()) {
                        cout << "Author cannot be empty! Cancelling...\n";
                        break;
                    }

                    cout << "Total copies: ";
                    b.totalquant = safeInput<int>();
                    if (b.totalquant <= 0) {
                        cout << "Invalid quantity! Cancelling...\n";
                        break;
                    }

                libraryRef.addBook(b);
                statusQueueRef.setTotalCopies(b.id, b.totalquant);
                    cout << "Book added successfully with ID " << b.id << "!\n";
                break;
            }
                break;
            }
            case 2: {
                while (true) {
                    cout << "\n--- DELETE BOOK ---\n";
                    int id = safeInput<int>("Enter book ID to delete (0 to cancel): ");
                    if (id == 0) break;

                    if (libraryRef.findBookById(id) == nullptr) {
                        cout << "Book with ID " << id << " not found!\n";
                        cout << "1. Try again    0. Cancel\nChoose: ";
                        if (safeInput<int>() == 0) break;
                        continue;
                    }

                libraryRef.removeBook(id);
                    cout << "Book ID " << id << " removed successfully!\n";
                break;
            }
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

                libraryRef.displayBooks();
                cout << "\n" << string(90, '-') << "\n";
                cout << "DETAILED STATUS:\n";
                statusQueueRef.showAllStatus();
                cout << string(130, '=') << "\n";
                break;
            }
            case 4: {
                while (true) {
                    int id = safeInput<int>("\nEnter book ID to search (0 to cancel): ");
                    if (id == 0) break;


                Book* b = libraryRef.findBookById(id);
                if (b) {
                    cout << "\nFOUND: " << b->name << " by " << b->author << "\n";
                        cout << "Total copies: " << b->totalquant << "\n";
                    statusQueueRef.showBookStatus(id);
                }
                else {
                        cout << "Book not found!\n";
                        cout << "1. Try again    0. Cancel\nChoose: ";
                        if (safeInput<int>() == 0) break;
                }
                }
                break;
            }
            case 5: {
                while (true) {
                cout << "\n=== STUDENT MANAGEMENT ===\n";
                    cout << "1. View all students  2. Register new student  0. Back\nChoose: ";
                    int sub = safeInput<int>();
                    if (sub == 0) break;

                if (sub == 1) {
                    cout << "\n--- ALL REGISTERED STUDENTS ---\n";
                    ifstream f("student.csv");
                    if (!f.is_open() || f.peek() == EOF) {
                            cout << "No students registered.\n";
                    }
                    else {
                            string line; int cnt = 1;
                        while (getline(f, line)) {
                            if (line.empty()) continue;
                            stringstream ss(line);
                            string sid, user, pass;
                                getline(ss, sid, ','); getline(ss, user, ',');
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
                            cout << "Student registered!\n";
                        }
                    }
                }
                break;
            }
            case 6: {
                while (true) {
                    int id = safeInput<int>("\nEnter book ID for details (0 to cancel): ");
                    if (id == 0) break;
                Book* b = libraryRef.findBookById(id);
                if (!b) {
                        cout << "Book not found!\n";
                        cout << "1. Try again    0. Cancel\nChoose: ";
                        if (safeInput<int>() == 0) break;
                        continue;
                }
                cout << "\n=== " << b->name << " by " << b->author << " ===\n";
                cout << "Total copies: " << b->totalquant << "\n";
                statusQueueRef.showBookStatus(id);
                }
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
                cout << "Invalid choice!\n";
            }
        } while (ch != 0);
    }
};

#endif
