#include "thread.H"

class node{
    public:
        Thread* thread;
        node* next;
        int nodeNumber;
        int nextNodeNumber;
        int prevNodeNumber;
        node(Thread* t){
            thread=t;
            next= nullptr;
        }
};
