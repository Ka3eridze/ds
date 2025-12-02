#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cctype>
#include "BookNode.h"

using namespace std;

template<typename T>
class BookAVL {
private:
    BookNode<T>* root = nullptr;

    int height(BookNode<T>* n) { return n ? n->height : 0; }
    int getBalance(BookNode<T>* n) { return n ? height(n->left) - height(n->right) : 0; }

    BookNode<T>* rotateLeft(BookNode<T>* x) {
        BookNode<T>* y = x->right;
        BookNode<T>* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = 1 + max(height(x->left), height(x->right));
        y->height = 1 + max(height(y->left), height(y->right));
        return y;
    }

    BookNode<T>* rotateRight(BookNode<T>* y) {
        BookNode<T>* x = y->left;
        BookNode<T>* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = 1 + max(height(y->left), height(y->right));
        x->height = 1 + max(height(x->left), height(x->right));
        return x;
    }

    BookNode<T>* insert(BookNode<T>* node, const T& value) {
        if (!node) return new BookNode<T>(value);
        if (value.id < node->data.id) node->left = insert(node->left, value);
        else if (value.id > node->data.id) node->right = insert(node->right, value);
        else return node;

        node->height = 1 + max(height(node->left), height(node->right));
        int balance = getBalance(node);

        if (balance > 1 && value.id < node->left->data.id) return rotateRight(node);
        if (balance < -1 && value.id > node->right->data.id) return rotateLeft(node);
        if (balance > 1 && value.id > node->left->data.id) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && value.id < node->right->data.id) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    BookNode<T>* searchById(BookNode<T>* node, int id) {
        if (!node || id == node->data.id) return node;
        if (id < node->data.id) return searchById(node->left, id);
        return searchById(node->right, id);
    }

    void searchByName(BookNode<T>* node, const string& name) {
        if (!node) return;
        searchByName(node->left, name);

        string nodeName = node->data.name;
        string searchName = name;

        transform(nodeName.begin(), nodeName.end(), nodeName.begin(), ::tolower);
        transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);

        if (nodeName.find(searchName) != string::npos) {
            cout << "ID: " << node->data.id
                << " | Title: " << node->data.name
                << " | Author: " << node->data.author
                << " | Copies: " << node->data.totalquant << endl;
        }

        searchByName(node->right, name);
    }

    void searchByAuthor(BookNode<T>* node, const string& author) {
        if (!node) return;
        searchByAuthor(node->left, author);

        string nodeAuthor = node->data.author;
        string searchAuthor = author;

        transform(nodeAuthor.begin(), nodeAuthor.end(), nodeAuthor.begin(), ::tolower);
        transform(searchAuthor.begin(), searchAuthor.end(), searchAuthor.begin(), ::tolower);

        if (nodeAuthor.find(searchAuthor) != string::npos) {
            cout << "ID: " << node->data.id
                << " | Title: " << node->data.name
                << " | Author: " << node->data.author
                << " | Copies: " << node->data.totalquant << endl;
        }

        searchByAuthor(node->right, author);
    }

    void inorder(BookNode<T>* node) {
        if (!node) return;
        inorder(node->left);
        cout << "ID: " << node->data.id
            << ", Name: " << node->data.name
            << ", Author: " << node->data.author
            << ", Total quantity: " << node->data.totalquant << endl;
        inorder(node->right);
    }


    BookNode<T>* minValueNode(BookNode<T>* node) {
        BookNode<T>* current = node;
        while (current && current->left) current = current->left;
        return current;
    }

    BookNode<T>* deleteNode(BookNode<T>* node, int id) {
        if (!node) return node;
        if (id < node->data.id) node->left = deleteNode(node->left, id);
        else if (id > node->data.id) node->right = deleteNode(node->right, id);
        else {
            if (!node->left || !node->right) {
                BookNode<T>* temp = node->left ? node->left : node->right;
                if (!temp) { delete node; return nullptr; }
                else { *node = *temp; delete temp; }
            }
            else {
                BookNode<T>* temp = minValueNode(node->right);
                node->data = temp->data;
                node->right = deleteNode(node->right, temp->data.id);
            }
        }
        if (!node) return node;

        node->height = 1 + max(height(node->left), height(node->right));
        int balance = getBalance(node);

        if (balance > 1 && getBalance(node->left) >= 0) return rotateRight(node);
        if (balance > 1 && getBalance(node->left) < 0) { node->left = rotateLeft(node->left); return rotateRight(node); }
        if (balance < -1 && getBalance(node->right) <= 0) return rotateLeft(node);
        if (balance < -1 && getBalance(node->right) > 0) { node->right = rotateRight(node->right); return rotateLeft(node); }

        return node;
    }

    void saveCSV(BookNode<T>* node, ofstream& fout) {
        if (!node) return;
        saveCSV(node->left, fout);
        fout << node->data.id << ","
            << node->data.name << ","
            << node->data.author << ","
            << node->data.totalquant << "\n";
        saveCSV(node->right, fout);
    }

public:
    void insert(const T& value) { root = insert(root, value); }
    void remove(int id) { root = deleteNode(root, id); }
    T* findById(int id) {
        BookNode<T>* res = searchById(root, id);
        return res ? &res->data : nullptr;
    }
    void findByName(const string& name) { searchByName(root, name); }
    void findByAuthor(const string& author) { searchByAuthor(root, author); }
    void printInOrder() { inorder(root); }

    void loadFromCSV(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) {
            cout << "File '" << filename << "' not found. A new file will be created when books are saved.\n";
            return;
        }

        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            if (!line.empty() && line.back() == '\r') line.pop_back();

            stringstream ss(line);
            string token;
            vector<string> fields;
            while (getline(ss, token, ',')) {
                fields.push_back(token);
            }
            if (fields.size() < 3) continue;

            Book b;
            try {
                b.id = stoi(fields[0]);
                b.name = fields[1];
                b.author = fields[2];
                b.totalquant = (fields.size() >= 4) ? stoi(fields[3]) : 1;
            }
            catch (...) {
                cout << "Warning: Skipping corrupted line: " << line << endl;
                continue;
            }
            insert(b);
        }
        fin.close();
    }

    void saveToCSV(const string& filename) {
        ofstream fout(filename);
        if (!fout.is_open()) {
            cout << "Error: Cannot create file '" << filename << "'!\n";
            return;
        }
        saveCSV(root, fout);
        fout.close();
        cout << "Books successfully saved to '" << filename << "'\n";
    }
};
