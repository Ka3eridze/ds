//U2410237
//Soy Yekaterina
#pragma once
#include <iostream>     // for input/output (cout, etc.)
#include <fstream>      // for file reading/writing
#include <algorithm>    // for std::max
#include <vector>       // for dynamic array to store CSV tokens
#include <sstream>      // for stringstream to split CSV line
#include <cctype>       // for tolower() in case-insensitive search
#include "BookNode.h"   // node structure for the AVL tree

using namespace std;

// Template AVL tree class that stores objects of type T (in this case, Book)
template<typename T>
class BookAVL {
private:
    BookNode<T>* root = nullptr;   // root of the AVL tree

    // Returns the height of a node (0 if nullptr)
    int height(BookNode<T>* n) { return n ? n->height : 0; }

    // Returns balance factor of a node (left height - right height)
    int getBalance(BookNode<T>* n) { return n ? height(n->left) - height(n->right) : 0; }

    // Performs a left rotation to balance the tree
    BookNode<T>* rotateLeft(BookNode<T>* x) {
        BookNode<T>* y = x->right;     // y becomes the new root of this subtree
        BookNode<T>* T2 = y->left;     // T2 is y's left subtree

        // Rotation
        y->left = x;
        x->right = T2;

        // Update heights
        x->height = 1 + max(height(x->left), height(x->right));
        y->height = 1 + max(height(y->left), height(y->right));

        // Return new root
        return y;
    }

    // Performs a right rotation to balance the tree
    BookNode<T>* rotateRight(BookNode<T>* y) {
        BookNode<T>* x = y->left;   // x becomes the new root
        BookNode<T>* T2 = x->right;

        // Rotation
        x->right = y;
        y->left = T2;

        // Update heights
        y->height = 1 + max(height(y->left), height(y->right));
        x->height = 1 + max(height(x->left), height(x->right));

        return x;
    }

    // Inserts a new value into the AVL tree and rebalances it if necessary
    BookNode<T>* insert(BookNode<T>* node, const T& value) {
        // Standard BST insertion
        if (!node) return new BookNode<T>(value);

        if (value.id < node->data.id)
            node->left = insert(node->left, value);
        else if (value.id > node->data.id)
            node->right = insert(node->right, value);
        else
            return node;  // duplicate ID -> do nothing

        // Update height after insertion
        node->height = 1 + max(height(node->left), height(node->right));

        // Check balance factor
        int balance = getBalance(node);

        // Four AVL rotation cases:

        // Left Left Case
        if (balance > 1 && value.id < node->left->data.id)
            return rotateRight(node);

        // Right Right Case
        if (balance < -1 && value.id > node->right->data.id)
            return rotateLeft(node);

        // Left Right Case
        if (balance > 1 && value.id > node->left->data.id) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Right Left Case
        if (balance < -1 && value.id < node->right->data.id) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;  // no rotation needed
    }

    // Searches for a book by ID (BST search)
    BookNode<T>* searchById(BookNode<T>* node, int id) {
        if (!node || id == node->data.id) return node;
        if (id < node->data.id) return searchById(node->left, id);
        return searchById(node->right, id);
    }

    // Searches books by name (case-insensitive, checks substrings)
    void searchByName(BookNode<T>* node, const string& name) {
        if (!node) return;

        searchByName(node->left, name);

        string nodeName = node->data.name;
        string searchName = name;

        // Convert both to lowercase for case-insensitive comparison
        transform(nodeName.begin(), nodeName.end(), nodeName.begin(), ::tolower);
        transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);

        // If the name contains the search term
        if (nodeName.find(searchName) != string::npos) {
            cout << "ID: " << node->data.id
                << " | Title: " << node->data.name
                << " | Author: " << node->data.author
                << " | Copies: " << node->data.totalquant << endl;
        }

        searchByName(node->right, name);
    }

    // Searches books by author (case-insensitive)
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

    // Prints tree nodes in sorted order (in-order traversal)
    void inorder(BookNode<T>* node) {
        if (!node) return;
        inorder(node->left);
        cout << "ID: " << node->data.id
            << ", Name: " << node->data.name
            << ", Author: " << node->data.author
            << ", Total quantity: " << node->data.totalquant << endl;
        inorder(node->right);
    }

    // Returns the node with the smallest ID
    BookNode<T>* minValueNode(BookNode<T>* node) {
        BookNode<T>* current = node;
        while (current && current->left)
            current = current->left;
        return current;
    }

    // Deletes a book by ID and rebalances the tree
    BookNode<T>* deleteNode(BookNode<T>* node, int id) {
        if (!node) return node;

        // Standard BST delete
        if (id < node->data.id)
            node->left = deleteNode(node->left, id);
        else if (id > node->data.id)
            node->right = deleteNode(node->right, id);
        else {
            // Case 1: node has 0 or 1 child
            if (!node->left || !node->right) {
                BookNode<T>* temp = node->left ? node->left : node->right;
                if (!temp) { delete node; return nullptr; }
                else { *node = *temp; delete temp; }
            }
            // Case 2: node has 2 children
            else {
                BookNode<T>* temp = minValueNode(node->right);
                node->data = temp->data;
                node->right = deleteNode(node->right, temp->data.id);
            }
        }

        if (!node) return node;

        // Update height
        node->height = 1 + max(height(node->left), height(node->right));

        // Rebalance
        int balance = getBalance(node);

        if (balance > 1 && getBalance(node->left) >= 0)
            return rotateRight(node);

        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1 && getBalance(node->right) <= 0)
            return rotateLeft(node);

        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    // Saves the tree into a CSV file (in-order, sorted by ID)
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
    // Public wrappers around private recursive functions
    void insert(const T& value) { root = insert(root, value); }
    void remove(int id) { root = deleteNode(root, id); }

    // Returns pointer to book data or nullptr
    T* findById(int id) {
        BookNode<T>* res = searchById(root, id);
        return res ? &res->data : nullptr;
    }

    void findByName(const string& name) { searchByName(root, name); }
    void findByAuthor(const string& author) { searchByAuthor(root, author); }
    void printInOrder() { inorder(root); }

    // Reads books from CSV file and inserts them into the AVL tree
    void loadFromCSV(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) {
            cout << "File '" << filename << "' not found. A new file will be created when books are saved.\n";
            return;
        }

        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            if (!line.empty() && line.back() == '\r') line.pop_back(); // fix Windows line endings

            stringstream ss(line);
            string token;
            vector<string> fields;

            // Split by commas
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

    // Saves the entire AVL tree to a CSV file
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

