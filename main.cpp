#include <iostream>
#include <string.h>
#include <queue>
#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <mutex>
#include <semaphore.h>
#include <fstream>
#include <vector>

using namespace std;

class BoundedQueue{
    queue<string> q;
    mutex m;	/* mutual exclusion semaphore  */
    sem_t empty;	/* count of empty buffer slots */
    sem_t full;	/* count of full  buffer slots */
    string value;

public:
    BoundedQueue(int n) {
        sem_init(&empty,0,n);
        sem_init(&full,0,0);
    }
    void insert(string x){
        sleep(0.1);
        sem_trywait(&empty);
        m.lock();
        q.push(x);
        m.unlock();
        sem_post(&full);
    }
    string remove(){
        sleep(0.1);
        string x;
        sem_trywait(&full);
        m.lock();
        x=q.front();
        q.pop();
        m.unlock();
        sem_post(&empty);
        return x;
    }

};

class UnBoundedQueue{
    queue<string> q;
    mutex m;	/* mutual exclusion semaphore  */
    sem_t full;	/* count of empty buffer slots */
    string value;

public:
    UnBoundedQueue() {
        sem_init(&full,0,0);
    }
    void insert(string x){
        sleep(0.1);
        m.lock();
        q.push(x);
        m.unlock();
        sem_post(&full);
    }
    string remove(){
        sleep(0.1);
        string x;
        sem_trywait(&full);
        m.lock();
        x=q.front();
        q.pop();
        m.unlock();
        return x;
    }

};





typedef struct Producer{
    int id;
    int size;
    int amount;
}producer;
string catg[]={"SPORTS","WEATHER","NEWS"};
vector<BoundedQueue*> BoundedQueueProducers;
UnBoundedQueue* S=new UnBoundedQueue();
UnBoundedQueue* W=new UnBoundedQueue();
UnBoundedQueue* N=new UnBoundedQueue();

void produce(Producer p){
    int size=p.size,amount=p.amount,id=p.id;
    char x[100];
    BoundedQueueProducers.push_back(new BoundedQueue(size));
    for(int j =0;j<amount;j++){
        sprintf(x,"Producer %d %s %d",id,catg[j%3].c_str(),j);
        BoundedQueueProducers.at(id-1)->insert(x);
    }
}
void* dispatcher(){
    int i=0;
    int counter=0;
    string val;
    while(counter<BoundedQueueProducers.size())
        if(BoundedQueueProducers[i]!= nullptr){
            val = BoundedQueueProducers[i]->remove();

        }
    i++;
    i=i%BoundedQueueProducers.size();
    }


int main(int argc, char *argv[]) {
    string filename = argv[1];
    string line;
    ifstream ifs(filename);
    if (!ifs)
        std::cerr << "couldn't open conf.txt for reading\n";
    int counter=0;
    int producerId =0;
    int productAmount=0;
    int size=0;
    vector<Producer> producersVector;
    while (getline(ifs, line)){
        if(line==""){
            getline(ifs, line);
        }
            counter++;
            if (counter == 1) {
            producerId=stoi(line);
            }
            if (counter == 2) {
                productAmount=stoi(line);
            }
            if (counter == 3) {
                size=stoi(line);
                producer p;
                p.amount=productAmount;
                p.size=size;
                p.id=producerId;
                producersVector.push_back(p);
                counter = 0;
            }
        }
    for(int i =0;i<producersVector.size();i++){
        thread th(produce,producersVector[i]);
        th.join();
    }
    return 0;
}

