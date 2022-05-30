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

class BoundedQueue {
    queue<string> q;
    mutex m;    /* mutual exclusion semaphore  */
    sem_t empty;    /* count of empty buffer slots */
    sem_t full;    /* count of full  buffer slots */
    string value;

public:
    BoundedQueue(int n) {
        sem_init(&empty, 0, n-1);
        sem_init(&full, 0, 0);
    }

    void insert(string x) {
        usleep(10000);
        sem_wait(&empty);
        m.lock();
        q.push(x);
        m.unlock();
        sem_post(&full);
    }

    string remove() {
        usleep(10000);
        string x;
        sem_wait(&full);
        m.lock();
        x = q.front();
        q.pop();
        m.unlock();
        sem_post(&empty);
        return x;
    }

};

class UnBoundedQueue {
    queue<string> q;
    mutex m;    /* mutual exclusion semaphore  */
    sem_t full;    /* count of empty buffer slots */
    string value;

public:
    UnBoundedQueue() {
        sem_init(&full, 0, 0);
    }

    void insert(string x) {
        usleep(10000);
        m.lock();
        q.push(x);
        m.unlock();
        sem_post(&full);
    }

    string remove() {
        usleep(10000);
        string x;
        sem_wait(&full);
        m.lock();
        x = q.front();
        q.pop();
        m.unlock();
        return x;
    }

};


typedef struct Producer {
    int id;
    int size;
    int amount;
} producer;
string catg[] = {"SPORTS", "WEATHER", "NEWS"};
vector<BoundedQueue *> BoundedQueueProducers;
UnBoundedQueue *S = new UnBoundedQueue();
UnBoundedQueue *W = new UnBoundedQueue();
UnBoundedQueue *N = new UnBoundedQueue();
//producer
void* produce(Producer p) {
    int size = p.size, amount = p.amount, id = p.id;
    char x[100];
    BoundedQueueProducers.push_back(new BoundedQueue(size));
    for (int j = 0; j < amount; j++) {
        sprintf(x, "Producer %d %s %d", id, catg[j % 3].c_str(), j+1);
        BoundedQueueProducers.at(id - 1)->insert(x);
    }
    usleep(10000);
    sprintf(x, "DONE");
//    cout<<"line 103 "<<id<<endl;
    BoundedQueueProducers.at(id - 1)->insert(x);
}
//coeditor
void *coEditor(UnBoundedQueue *q, BoundedQueue *screen) {
    string val;
    while ((val = (q->remove())).find("DONE") == string::npos) {
        screen->insert(val);
    }
    usleep(10000);
    screen->insert("DONE");
}
//screen
void *screenPrinter(BoundedQueue *screen) {
    string val;
    int counter = 0;
    while (counter < 3) {
        while ((val = (screen->remove())).find("DONE") == string::npos) {
            cout << val << endl;
        }
        counter++;
    }
}
//dispatcher
void *dispatcher(int size) {
    int i = 0;
    string val;
    vector<int>v;
    bool flag = true;
    while (v.size()!=size) {
//        cout<<"line 131 "<<i<<endl;
        if (BoundedQueueProducers[i] != nullptr) {
            val = BoundedQueueProducers[i]->remove();
            if (val.find("NEWS") != string::npos) {
                N->insert(val);
            }
            if (val.find("SPORTS") != string::npos) {
                S->insert(val);
            }
            if (val.find("WEATHER") != string::npos) {
                W->insert(val);
            }
            if (val.find("DONE") != string::npos) {
                BoundedQueueProducers[i]= nullptr;
            }
        }
        if(BoundedQueueProducers[i] == nullptr){
            for(auto x : v){
                if(x==i)
                {
                    flag=false;
                }
            }
            if(flag){
                v.push_back(i);
            }
            flag=true;
        }
        i++;
        i = i % (size);
    }
    usleep(10000);
    val = "DONE";
    N->insert(val);
    S->insert(val);
    W->insert(val);
}


int main(int argc, char *argv[]) {
    string filename = argv[1];
    string line;
    ifstream ifs(filename);
    if (!ifs)
        std::cerr << "couldn't open conf.txt for reading\n";
    int counter = 0;
    int producerId = 0;
    int productAmount = 0;
    int size = 0;
    int sizeScreenBuffer = 0;
    vector<Producer> producersVector;
    while (getline(ifs, line)) {
        if (line == "") {
            getline(ifs, line);
        }
        counter++;
        if (counter == 1) {
            producerId = stoi(line);
            sizeScreenBuffer = stoi(line);
        }
        if (counter == 2) {
            productAmount = stoi(line);
        }
        if (counter == 3) {
            size = stoi(line);
            producer p;
            p.amount = productAmount;
            p.size = size;
            p.id = producerId;
            producersVector.push_back(p);
            counter = 0;
        }
    }
    vector<thread> threads;
    BoundedQueue *Screen = new BoundedQueue(sizeScreenBuffer);
    int vectorSize = producersVector.size();
    for (int i = 0; i < producersVector.size(); i++) {
        thread th(produce, producersVector[i]);
        threads.push_back(move(th));
    }
    thread th(dispatcher, vectorSize);
    thread th1(coEditor, N, Screen);
    thread th2(coEditor, W, Screen);
    thread th3(coEditor, S, Screen);
    thread th4(screenPrinter, Screen);

    for (int k = 0; k < threads.size(); k++) {
        threads[k].join();
    }
    th.join();
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    return 0;
}

