#include "Book.h"
#include <iostream>
#include <limits>
using namespace std;
//function to input book details
Book inputBook() {
    Book b;
    cout << "\nEnter book ID: ";
    cin >> b.id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter book name: ";
    getline(cin, b.name);
    cout << "Enter author name: ";
    getline(cin, b.author);
    cout << "Enter total quantity of books: ";
    cin >> b.totalquant;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return b;
};