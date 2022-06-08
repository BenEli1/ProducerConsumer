# ProducerConsumer
![image](https://user-images.githubusercontent.com/92650578/172707234-bd3fe762-060d-4886-a2a2-1e25823d6171.png)

Implemention of the producer consumer scenario - using multi-threaded program in CPP :

Every producer has a BoundedQueue - that he shares with the dispatcher.
The producer inserts new articles to the queue and the dispatcher pops the articles and then sorts them by type - News,Sports,Weather

The dispatcher has 3 Unbounded queues(1 for each category) that he shares with the co editors,
he inserts articles to the queues and the co-editors pop articles from the queue.

All co-editors have a shared bounded queue with the screen manager, they push articles to the queue and the screen manager prints articles from the queue.

# Input:

the sizes of the bounded queues are given in the config file :

first row - producer number ,second row - the amount of items the producer wants to publish,third row - the queue size and then an empty line that way for each producer,
the last line is the size of the screen manager queue.
