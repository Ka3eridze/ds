#ifndef ADMIN_H
#define ADMIN_H
// 1. Header Guard — защита от двойного включения файла

#include "Role.h"
#include <string>      // строки: username, password, названия книг
#include <fstream>     // чтение/запись файлов (admin.txt, student.csv)
#include <iostream>    // cout, cin
#include <iomanip>     // setw(), left — красивый табличный вывод
#include <sstream>     // stringstream — парсинг строк и "перехват" вывода
#include <limits>      // numeric_limits — безопасная очистка cin
#include "Library.h"
#include "Status_Queue.h"
using namespace std;

class Admin : public Role {
private:
    LIBirianc& libraryRef;           // 3. Ссылка на общую библиотеку (один экземпляр)
    StatusQueue& statusQueueRef;     // 3. Ссылка на общую систему выдачи (синхронизация с Student)
    string adminFile;                // 3. Путь к файлу с учёткой админа

    // 4. Безопасный ввод числа — программа НЕ падает при вводе букв
    template<typename T>
    T safeInput(const string& prompt = "") {
        T value;
        if (!prompt.empty()) cout << prompt;
        while (!(cin >> value)) {
            cout << "Invalid input! Enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 4. Очистка буфера
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
    }

    // 5. Загрузка логина/пароля админа из файла
    bool loadCredentials() {
        ifstream fin(adminFile);                    // 5. fin — просто имя (file input)
        if (!fin.is_open()) return false;
        if (getline(fin, username) && getline(fin, password)) {
            // 6. Убираем \r — баг Windows при чтении строк
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
    // 7. Конструктор: принимает ссылки — работает с одним объектом во всей программе
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

            ch = safeInput<int>();  // 8. Безопасный ввод — не падает при вводе букв

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

                        if (libraryRef.findBookById(bookId) != nullptr) {
                            cout << "ERROR: Book with ID " << bookId << " already exists!\n";
                            cout << "1. Try another ID    0. Cancel adding\nChoose: ";
                            if (safeInput<int>() == 0) {
                                cout << "Add book cancelled.\n";
                                break;
                            }
                            continue;
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
                        statusQueueRef.setTotalCopies(b.id, b.totalquant);  // 9. Синхронизация статуса!
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
                    cout << "\n" << string(110, '=') << "\n";
                    cout << left
                         << setw(5)  << "ID"
                         << setw(36) << "TITLE"
                         << setw(23) << "AUTHOR"
                         << setw(8)  << "TOTAL"
                         << setw(8)  << "AVAIL"
                         << "STATUS\n";
                    cout << string(110, '-') << "\n";

                    bool hasBooks = false;
                    for (int id = 1; id <= 1000; ++id) {
                        Book* b = libraryRef.findBookById(id);
                        if (!b) continue;  // 11. nullptr — книга не найдена

                        hasBooks = true;
                        int available = b->totalquant;
                        string status = "All available";

                        // 12. Перехват вывода showBookStatus() в строку — гениальный способ получить данные
                        stringstream buffer;
                        streambuf* old = cout.rdbuf(buffer.rdbuf());
                        statusQueueRef.showBookStatus(id);
                        cout.rdbuf(old);

                        size_t p = buffer.str().find("Available: ");
                        if (p != string::npos) {
                            available = stoi(buffer.str().substr(p + 11));
                            int issued = b->totalquant - available;
                            status = issued ? to_string(issued) + " issued" : "All available";
                            if (buffer.str().find("Waiting queue") != string::npos && 
                                buffer.str().find("(empty)") == string::npos)
                                status += ", queue";
                        }

                        // 13. setw() — задаёт ширину колонки для ровного вывода
                        cout << left
                             << setw(5)  << b->id
                             << setw(36) << (b->name.substr(0,35) + (b->name.size()>35 ? "..." : ""))
                             << setw(23) << (b->author.substr(0,22) + (b->author.size()>22 ? "..." : ""))
                             << setw(8)  << b->totalquant
                             << setw(8)  << available
                             << status << "\n";
                    }

                    if (!hasBooks) {
                        cout << "                         No books in the library yet.\n";
                    }

                    cout << string(110, '=') << "\n";
                    break;
                }
                case 4: {
                    while (true) {
                        int id = safeInput<int>("\nEnter book ID to search (0 to cancel): ");
                        if (id == 0) break;

                        Book* b = libraryRef.findBookById(id);
                        if (b) {
                            cout << "\nFOUND: " << b->name << " by " << b->author << "\n";
                            cout << "Total copies: " << b->totalquant << "\n";  // 14. Показываем общую инфу
                            statusQueueRef.showBookStatus(id);                // 14. Показываем статус выдачи
                        } else {
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
                        cout << "1. View all students\n";
                        cout << "2. Register new student\n";
                        cout << "0. Back to main menu\n";
                        cout << "Choose: ";
                        int sub = safeInput<int>();

                        if (sub == 0) break;

                        if (sub == 1) {
                            cout << "\n--- REGISTERED STUDENTS ---\n";
                            ifstream f("student.csv");
                            // 15. Проверка пустого файла
                            if (!f.is_open() || f.peek() == EOF) {
                                cout << "No students registered yet.\n";
                            } else {
                                string line;
                                int cnt = 1;
                                while (getline(f, line)) {
                                    if (line.empty()) continue;
                                    stringstream ss(line);
                                    string user, pass;
                                    getline(ss, user, ',');
                                    cout << cnt++ << ". Username: " << user << endl;
                                }
                            }
                            f.close();
                        }
                        else if (sub == 2) {
                            cout << "\n--- REGISTER NEW STUDENT ---\n";
                            string user, pass;

                            cout << "Enter Username (0 to cancel): ";
                            getline(cin, user);
                            if (user == "0") { cout << "Registration cancelled.\n"; continue; }
                            if (user.empty()) { cout << "Username cannot be empty!\n"; continue; }

                            cout << "Enter Password (0 to cancel): ";
                            getline(cin, pass);
                            if (pass == "0") { cout << "Registration cancelled.\n"; continue; }
                            if (pass.empty()) { cout << "Password cannot be empty!\n"; continue; }

                            // 16. Проверка дубликата username
                            ifstream check("student.csv");
                            string line;
                            bool exists = false;
                            while (getline(check, line)) {
                                if (line.empty()) continue;
                                stringstream ss(line);
                                string existingUser;
                                getline(ss, existingUser, ',');
                                if (existingUser == user) { exists = true; break; }
                            }
                            check.close();

                            if (exists) {
                                cout << "Username '" << user << "' already exists!\n";
                                continue;
                            }

                            // 17. ios::app — дозапись в конец файла
                            ofstream f("student.csv", ios::app);
                            if (f.is_open()) {
                                f << user << "," << pass << "\n";
                                f.close();
                                cout << "Student '" << user << "' registered successfully!\n";
                            }
                        }
                    }
                    break;
                }
                                                                  case 6: {
                    cout << "\n" << string(100, '=') << "\n";
                    cout << "               CURRENTLY ISSUED BOOKS               \n";
                    cout << string(100, '-') << "\n";

                    bool hasIssued = false;
                    for (int id = 1; id <= 1000; ++id) {
                        Book* b = libraryRef.findBookById(id);
                        if (!b) continue;

                        stringstream temp;
                        streambuf* old = cout.rdbuf(temp.rdbuf());
                        statusQueueRef.showBookStatus(id);
                        cout.rdbuf(old);

                        string s = temp.str();
                        size_t p = s.find("Available: ");
                        if (p == string::npos) continue;

                        int available = stoi(s.substr(p + 11));
                        int issued = b->totalquant - available;
                        if (issued <= 0) continue;

                        hasIssued = true;
                        cout << "ID: " << id 
                             << " | " << b->name 
                             << " | Issued: " << issued << "/" << b->totalquant
                             << " | Student: ";

                        size_t ip = s.find("Issued to: ");
                        if (ip != string::npos) {
                            string students = s.substr(ip + 11);
                            size_t nl = students.find('\n');
                            if (nl != string::npos) students = students.substr(0, nl);
                            cout << students;
                        } else {
                            cout << "Unknown";
                        }
                        cout << "\n";
                    }

                    if (!hasIssued) cout << "                     No books currently issued.\n";
                    cout << string(100, '=') << "\n";
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

