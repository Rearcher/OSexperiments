#ifndef _MESSAGE_QUEUE_
#define _MESSAGE_QUEUE_

#include <iostream>
#include <sys/msg.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

/* 1 means stone, 2 means scissor, 3 means cloth */
enum hand {
	stone = 1, scissor = 2, cloth = 3
};

std::vector<std::string> hands{"", "stone", "scissor", "cloth"};

/* my data struct */
struct message {
	long type;
	int data;
	
	message(long t=1, int d=-2) :
		type(t), data(d) {}
};

/* wrapper class to hold message queue */
class MessageQueue {
public:
	MessageQueue(int k) : key(k), id(-1) {
		id = msgget(key, 0600|IPC_CREAT);
		if (id == -1)
			std::cerr << "message queue create failed.\n";
	}

	~MessageQueue() {
		/* delete message queue */
		msgctl(id, IPC_RMID, 0);
	}

	int getID() { return id; }

private:
	int key, id;
};

/* non-member wrapper function for msgsnd */
void SendMessage(int id, message &msg) {
	if (msgsnd(id, &msg, sizeof(int), 0) == -1)
		std::cerr << "send message error.\n";
}

/* non-member wrapper function for msgrcv */
void ReceiveMessage(int id, message &msg) {
	if (msgrcv(id, &msg, sizeof(int), 0, 0) == -1)
		std::cerr << "receive message error.\n";
}

#endif // _MESSAGE_QUEUE_
