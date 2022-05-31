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
// command to run - g++ -std=c++11 -o ex3.out -pthread main.cpp
/*
 * Bounded queue -  has a mutex for locking access to the queue,
 * and 2 semaphores - full and empty meaning how much space left/taken in queue
 */
class BoundedQueue {
    queue<string> q;
    mutex m;    /* mutual exclusion semaphore  */
    sem_t empty;    /* count of empty buffer slots */
    sem_t full;    /* count of full  buffer slots */
 
public:
    BoundedQueue(int n) {
        sem_init(&empty, 0, n);
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

/*
 * UnBounded queue -  has a mutex for locking access to the queue,
 * and 1 semaphores - full meaning how much space taken in queue.
 */
class UnBoundedQueue {
    queue<string> q;
    mutex m;    /* mutual exclusion semaphore  */
    sem_t full;    /* count of empty buffer slots */

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

//struct for saving a certain producer
typedef struct Producer {
    int id;
    int size;
    int amount;
} producer;
//category array
string category[] = {"SPORTS", "WEATHER", "NEWS"};

//bounded queues for each producer that he shares with the dispatcher.
vector<BoundedQueue *> BoundedQueueProducers;

//unbounded queues for dispatcher and co editors.

UnBoundedQueue *S = new UnBoundedQueue();
UnBoundedQueue *W = new UnBoundedQueue();
UnBoundedQueue *N = new UnBoundedQueue();

/*producer - adds a new bounded queue to the vector of bounded queues,
 * and then for the amount of items given in conf file he adds to that queue.
 * send done in the end to notify the dispatcher that the bounded queue of producer i is done.
*/
void produce(Producer p) {
    int size = p.size, amount = p.amount, id = p.id;
    char x[100];
    //random in order to get random categories
    int r = rand() % 3;
    BoundedQueueProducers[id - 1] = new BoundedQueue(size);
    for (int j = 0; j < amount; j++) {
        sprintf(x, "Producer %d %s %d", id, category[r].c_str(), j + 1);
        BoundedQueueProducers[id - 1]->insert(x);
        r = rand() % 3;
    }
    usleep(10000);
    sprintf(x, "DONE");
//    cout<<"line 103 "<<id<<endl;
    BoundedQueueProducers.at(id - 1)->insert(x);
}

/* dispatcher
 * gets values from producer bounded queue and in the end gets "done" message,
 * every messages he gets he puts in the unbounded queue according to the news category
 * and sends done in the end for every queue.
 */
void dispatcher(int size) {
    int i = 0;
    string val;
    //vector of ints that means what indexes are null in the BoundedQueueProducers
    //when it gets to the size of the bounded queue of producers we send a done message.
    vector<int> v;
    int doneCounter = 0;
    bool flag = true;
    while (doneCounter != size) {
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
                BoundedQueueProducers[i] = nullptr;
                doneCounter++;
            }
        }
        //checking that all are null
//        if (BoundedQueueProducers[i] == nullptr) {
//            for (auto x: v) {
//                if (x == i) {
//                    flag = false;
//                }
//            }
//            if (flag) {
//                v.push_back(i);
//            }
//            flag = true;
//        }
        i++;
        i = i % (size);
    }
    usleep(10000);
    val = "DONE";
    N->insert(val);
    S->insert(val);
    W->insert(val);
}


/* co-editor
 * gets as an argument the screen bounded queue and an unbounded queue specific for him - N,S,W
 * he transfers messages to the screen printer and sends done in the end.
 */
void coEditor(UnBoundedQueue *q, BoundedQueue *screen) {
    string val;
    while ((val = (q->remove())).find("DONE") == string::npos) {
        screen->insert(val);
    }
    usleep(10000);
    screen->insert("DONE");
    delete q;
}

/* screen printer
 * gets messages from co editors and prints them to the screen until he receives 3 done messages.
 */
void screenPrinter(BoundedQueue *screen) {
    string val;
    int counter = 0;
    while (counter < 3) {
        while ((val = (screen->remove())).find("DONE") == string::npos) {
            cout << val << endl;
        }
        counter++;
    }
    delete screen;
}


int main() {
    //file reading config txt in relative path
    string filename = "config.txt";
    string line;

    ifstream ifs(filename);
    if (!ifs) {
        std::cerr << "couldn't open config.txt for reading\n";
    }
    //counter used to know what kind of information we get from config.txt
    int counter = 0;
    //the producer id
    int producerId = 0;
    //the amount the producer wants to create
    int productAmount = 0;
    //the size of the bounded queue
    int size = 0;
    //the size of the screen printer bounded queue
    int sizeScreenBuffer = 0;
    producer p;
    //vector of producers - later creating a vector of bounded queues using produce.
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
            p.amount = productAmount;
            p.size = size;
            p.id = producerId;
            producersVector.push_back(p);
            counter = 0;
        }
    }
    //thread vector in order to use join on them.
    vector<thread> threads;
    BoundedQueue *Screen = new BoundedQueue(sizeScreenBuffer);
    int vectorSize = producersVector.size();
    BoundedQueueProducers.resize(vectorSize);
    //creating threads for each producer,dispatcher,co editor and screen printer
    for (int m = 0; m < vectorSize; m++) {
        thread th(produce, producersVector[m]);
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

