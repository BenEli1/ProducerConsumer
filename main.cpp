#include <iostream>
#include <string.h>
#include <queue>
#include <pthread.h>
#include<thread>
#include <stdlib.h>
#include <string.h>
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

pthread_t ntid;
char* catg[]={"SPORTS","WEATHER","NEWS"};
int id = 0;
vector<BoundedQueue*> ProducersVector;
char* produce(int i){
    char *x;
    sprintf(x,"Producer %d %s %d",i,catg[id%3],id++);
    return x;
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
    while (getline(ifs, line)){
        if(line=="\n"){
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
                ProducersVector.push_back(new BoundedQueue(stoi(line)));
                for(int i =0;i<productAmount;i++){
                    ProducersVector.at(producerId)->insert(produce(producerId));
                }
                counter = 0;
            }
        }

    int    err;
    err = pthread_create(&ntid, NULL, NULL, NULL);
    if (err != 0)
        perror("can't create thread:{err}");
    return 0;
}

