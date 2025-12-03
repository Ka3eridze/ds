//U2410207
//RIXSITILLAYEVA LOBAR
#ifndef STATUSQUEUE_H
#define STATUSQUEUE_H

#include "StatusNode.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

// this class manages with all book statuses and their queue, btw it works with csv file
class StatusQueue {
private:
    StatusNode* head;

    // find book with id
    StatusNode* findStatus(int bookId) {
        StatusNode* temp = head;
        while (temp) {
            if (temp->bookId == bookId) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    // create new status (if book isnt in status csv yet)
    StatusNode* createStatus(int bookId, int totalCopies) {
        StatusNode* node = new StatusNode(bookId, totalCopies, totalCopies);
        if (!head) {
            head = node;
        }
        else {
            StatusNode* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = node;
        }
        return node;
    }

    //check if there is same name in queue (do not add twice )
    bool isInQueue(StatusNode* bookStatus, const string& studentName) {
        WaitNode* cur = bookStatus->front;
        while (cur) {
            if (cur->studentName == studentName) return true;
            cur = cur->next;
        }
        return false;
    }

    //add student to the waiting list
    void enqueue(StatusNode* bookStatus, const string& studentName) {
        WaitNode* node = new WaitNode(studentName);
        if (!bookStatus->rear) {
            bookStatus->front = bookStatus->rear = node;
        }
        else {
            bookStatus->rear->next = node;
            bookStatus->rear = node;
        }
    }

    // remove next student from the queue
    string dequeue(StatusNode* bookStatus) {
        if (!bookStatus->front) return "";

        WaitNode* temp = bookStatus->front;

        string name = temp->studentName;

        bookStatus->front = bookStatus->front->next;
        if (!bookStatus->front) {
            bookStatus->rear = nullptr;
        }
        delete temp;
        return name;
    }


    void printStatus(StatusNode* node) {
        if (!node) return;
        cout << "Book ID: " << node->bookId
            << " | Total: " << node->totalCopies
            << " | Available: " << node->availableCopies
            << " | Borrowed: " << (node->totalCopies - node->availableCopies)
            << endl;

        // we are showing who borrowed the book
        if (!node->issuedTo.empty()) {
            cout << "  Issued to: ";
            for (size_t i = 0; i < node->issuedTo.size(); ++i) {
                if (i > 0) cout << ", ";
                cout << node->issuedTo[i];
            }
            cout << endl;
        }

        if (!node->front) {
            cout << "  Waiting queue: (empty)\n";
        }
        else {
            cout << "  Waiting queue: ";
            WaitNode* cur = node->front;
            bool first = true;
            while (cur) {
                if (!first) cout << " -> ";
                cout << cur->studentName;
                first = false;
                cur = cur->next;
            }
            cout << endl;
        }
    }

public:
    StatusQueue() : head(nullptr) {}

    ~StatusQueue() {
        StatusNode* temp = head;
        while (temp) {
            WaitNode* w = temp->front;
            while (w) {
                WaitNode* nextW = w->next;
                delete w;
                w = nextW;
            }
            StatusNode* nextS = temp->next;
            delete temp;
            temp = nextS;
        }
    }

    // download all statuses from status.csv
    void loadFromCSV(const string& filename = "status.csv") {
        ifstream file(filename);
        if (!file.is_open()) {
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string idStr, totalStr, availStr;

            if (!getline(ss, idStr, ',')) continue;
            if (!getline(ss, totalStr, ',')) continue;
            if (!getline(ss, availStr, ',')) continue;

            int id = stoi(idStr);
            int total = stoi(totalStr);
            int avail = stoi(availStr);

            StatusNode* node = createStatus(id, total);
            node->availableCopies = avail;


            string name;
            while (getline(ss, name, ',')) {
                if (!name.empty()) {
                    enqueue(node, name);
                }
            }
        }

        file.close();
    }

    //saving all statuses in status.csv
    void saveToCSV(const string& filename = "status.csv") {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error: cannot open " << filename << " for writing.\n";
            return;
        }

        StatusNode* temp = head;
        while (temp) {
            file << temp->bookId << ","
                << temp->totalCopies << ","
                << temp->availableCopies;

            WaitNode* cur = temp->front;
            while (cur) {
                file << "," << cur->studentName;
                cur = cur->next;
            }
            file << "\n";
            temp = temp->next;
        }

        file.close();
    }

    //download or upload total number of books
    void setTotalCopies(int bookId, int totalCopies) {
        if (totalCopies < 0) {
            cout << "Total copies cannot be negative.\n";
            return;
        }

        StatusNode* node = findStatus(bookId);
        if (!node) {
            node = createStatus(bookId, totalCopies);
        }
        else {
            node->totalCopies = totalCopies;
            if (node->availableCopies > node->totalCopies) {
                node->availableCopies = node->totalCopies;
            }
        }
        cout << "Total copies for book " << bookId
            << " set to " << node->totalCopies << ".\n";
    }

    // handing out the book to the student
    // if there are available book - we give them out, if nono, we add them to the queue
    void issueBook(int bookId, const string& studentName) {
        if (studentName.empty()) {
            cout << "Student name cannot be empty.\n";
            return;
        }

        StatusNode* node = findStatus(bookId);
        if (!node) {
            cout << "No status found for book " << bookId << ".\n";
            cout << "Please set total copies first.\n";
            return;
        }

        if (node->availableCopies > 0) {
            node->availableCopies--;
            node->issuedTo.push_back(studentName);
            cout << "Book " << bookId << " issued to " << studentName << ".\n";
            cout << "Available copies left: " << node->availableCopies << ".\n";
        }
        else {
            if (isInQueue(node, studentName)) {
                cout << "Student " << studentName
                    << " is already in the waiting queue for book " << bookId << ".\n";
                return;
            }
            enqueue(node, studentName);


            int pos = 1;
            WaitNode* cur = node->front;
            while (cur && cur->studentName != studentName) {
                pos++;
                cur = cur->next;
            }

            cout << "No available copies for book " << bookId << ".\n";
            cout << "Student " << studentName << " added to waiting queue at position "
                << pos << ".\n";
        }
    }

    // return of the book
    // if queue is empty increase available
    // if queue is not empty  – we immediately give it to the next one
    void returnBook(int bookId) {
        StatusNode* node = findStatus(bookId);
        if (!node) {
            cout << "No status found for book " << bookId << ".\n";
            return;
        }


        if (!node->front) {
            if (node->availableCopies < node->totalCopies) {
                node->availableCopies++;
                cout << "Book " << bookId << " returned. "
                    << "Available: " << node->availableCopies << ".\n";
            }
            else {
                cout << "Book " << bookId
                    << " returned, but all copies are already marked as available.\n";
            }
        }
        else {
            string nextStudent = dequeue(node);
            node->issuedTo.push_back(nextStudent);
            cout << "Book " << bookId
                << " returned and immediately issued to waiting student: "
                << nextStudent << ".\n";
            cout << "Available copies remain: " << node->availableCopies << ".\n";
        }
    }

    // show the status of the specific book
    void showBookStatus(int bookId) {
        StatusNode* node = findStatus(bookId);
        if (!node) {
            cout << "No status found for book " << bookId << ".\n";
            return;
        }
        printStatus(node);
    }

    //show the status of all books 
    void showAllStatus() {
        if (!head) {
            cout << "No status information available.\n";
            return;
        }
        StatusNode* temp = head;
        while (temp) {
            printStatus(temp);
            temp = temp->next;
        }
    }
};


#endif
