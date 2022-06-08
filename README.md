# ProducerConsumer
![image](https://user-images.githubusercontent.com/92650578/172707234-bd3fe762-060d-4886-a2a2-1e25823d6171.png)
Implemention of the producer consumer scenario - using multi-threaded program in CPP:

Every producer has a BoundedQueue - that he shares with the dispatcher.
The producer addes new articles to the queue and the dispatcher reads the articles and then sorts them by type - News,Sports,Weather

The dispatcher has 3 Unbounded queues(1 for each category) that he shares with the co editors,
he addes articles to the queues and the co-editors remove articles from the queue

Every co-editor has a bounded queue with the screen manager, they push articles to the queue and the screen manager prints articles from the queue.
