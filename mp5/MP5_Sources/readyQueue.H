#include "thread.H"
#include "node.H"
#include "assert.H"
#include "console.H"

class readyQueue{
    private:
        node* first;
        node* last;
        int queueNumber;
        int sizeOfTheQueue;
    public:
        readyQueue(){
            first=nullptr;
            last= nullptr;
        }
        void insert(Thread* t){
            node* newNode = new node(t);
            int curr = 0;
            if(first == nullptr) curr = 1;
            if(last == nullptr) curr = 1;
            if(first==nullptr){
                first = last = newNode;
                newNode->prevNodeNumber = 0;
                newNode->nextNodeNumber = 0;
                newNode->nodeNumber = curr;
                //indicating that both the next and prev nodes are not present
                return;
            }
            
            Console :: puts("inserting a new thread when queue is non empty ");
            Console :: puts("/n");
            curr =  last->nodeNumber;
            curr = curr + 1;
            newNode->nodeNumber = curr;
            last->nextNodeNumber = curr;
            int backNumber = last->nodeNumber;
            newNode->prevNodeNumber = backNumber;
            newNode->nextNodeNumber = 0;
            last->next= newNode;
            last=last->next;
            Console :: puts("Queue size after insertion is : ");
            return;
        }


        Thread* getFirst(){
            node* temporary = first;
            int k = temporary->nodeNumber;
            return first->thread;
        }
	
	
        void pop(){
            if(first == nullptr) return;
            int popping = first->nodeNumber;
            if(first->next == nullptr) { first = last = nullptr; return;}
            node* temporary = first->next;
            int next = temporary->nodeNumber;
            first = first->next;
            return;
        }

        void deleteThisNode(Thread* t){
            node* temp = first;
            int k = 0;
            if(first!= nullptr) k = temp->nodeNumber;
            node* prev = first;
            int r = prev->nodeNumber;
            if(temp->thread == t){
                temp = temp->next;
                return;
            }
            int c = -1;
            
            while(temp!=nullptr && temp->thread != t){
                prev = temp;
                c = prev->nodeNumber;
                k++;
                temp = temp->next;
            }
            if(temp == nullptr){
                return;
            }
            c = prev->nodeNumber;
            k = temp->nodeNumber;
     
            prev->next = temp->next;
            return;
        }

};
