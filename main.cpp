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

using namespace std;
int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

class BoundedQueue:public queue<string>{
    mutex m;	/* mutual exclusion semaphore  */
    semaphore empty = N;	/* count of empty buffer slots */
    semaphore full  = 0;	/* count of full  buffer slots */


public:
    BoundedQueue(int n) {
        empty=n;
        full=0;
    }
    void up(int lock){
        lock++;
    }
    void down(int lock){
        lock--;
    }
    void push(string x){
        lock(m);
    }
    string pop(){

    }

};