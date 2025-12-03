//U2410237
//Soy Yekaterina
#ifndef LIBRARY_H
#define LIBRARY_H

#pragma once
#include "Book.h"
#include "BookAVL.h"
#include <iostream>
#include <limits>
using namespace std;

class LIBirianc {
private:
    BookAVL<Book> books;


public:
    void addBook(const Book& b) { books.insert(b); }
    void removeBook(int id) { books.remove(id); }
    Book* findBookById(int id) { return books.findById(id); }
    void findBookByName(const string& name) { books.findByName(name); }
    void findBookByAuthor(const string& author) { books.findByAuthor(author); }
    void displayBooks() { books.printInOrder(); }

    void loadBooks(const string& filename) { books.loadFromCSV(filename); }
    void saveBooks(const string& filename) { books.saveToCSV(filename); }
};

#endif //LIBRARY_H
