#include <iostream>//d
#include <limits>
#include "Library.h"
#include "Admin.h"
#include "Student.h"
#include "Book.h"
#include "status_Queue.h"

using namespace std;

int main() {
    LIBirianc lib;
    StatusQueue statusQueue;

    // Загружаем данные
    lib.loadBooks("books.csv");
    statusQueue.loadFromCSV("status.csv");

    int roleChoice;

    while (true) {
        cout << "\n========================================\n";
        cout << "        Library Management System       \n";
        cout << "========================================\n\n";

        cout << "Select Role:\n";
        cout << " 1. Admin\n";
        cout << " 2. Student\n";
        cout << " 0. Exit\n";
        cout << "Choice: ";

        if (!(cin >> roleChoice)) {  // проверка ввода
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input! Enter a number.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // очистка буфера

        if (roleChoice == 1) {
            cout << "\n======== Admin Login ========\n";
            Admin admin(lib, statusQueue, "admin.txt");
            if (admin.login()) {
                cout << "\n========= Admin Menu =========\n";
                admin.showMenu();
            }
            else {
                cout << "\nInvalid credentials. Returning to main menu...\n";
            }
        }
        else if (roleChoice == 2) {
            cout << "\n======= Student Login =======\n";
            Student student(lib, statusQueue, "student.csv");
            if (student.login()) {
                cout << "\n======== Student Menu ========\n";
                student.showMenu();
            }
            else {
                cout << "\nInvalid credentials. Returning to main menu...\n";
            }
        }
        else if (roleChoice == 0) {
            cout << "\nExiting program... Goodbye!\n";
            break;
        }
        else {
            cout << "\nInvalid choice. Try again.\n";
        }
    }

    // Сохраняем данные
    lib.saveBooks("books.csv");
    statusQueue.saveToCSV("status.csv");

    return 0;
}