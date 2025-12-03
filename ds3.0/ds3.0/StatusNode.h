//U2410207
//RIXSITILLAYEVA LOBAR
#ifndef STATUSNODE_H
#define STATUSNODE_H

#include <string>
#include <vector>
using namespace std;

// its loop for waitings (name students)
struct WaitNode {
    string studentName;
    WaitNode* next;

    WaitNode(const string& name) : studentName(name), next(nullptr) {}
};

// loop for the status of the books 
// for every book, id, available copies and waiting list
struct StatusNode {
    int bookId;
    int totalCopies;
    int availableCopies;

    WaitNode* front;
    WaitNode* rear;

    vector<string> issuedTo;

    StatusNode* next;
    StatusNode(int id, int total, int available)
        : bookId(id),
          totalCopies(total),
          availableCopies(available),
          front(nullptr),
          rear(nullptr),
          next(nullptr),
        issuedTo() {
    }
};


#endif
