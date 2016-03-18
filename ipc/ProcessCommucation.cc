#include "MessageQueue.h"
#include <thread>
#include <fstream>

/* the times the game will be done */
const int MAX_GAME_NUM = 1000;

/* Player function, generate randomly */
void Player(int queue_id, int res_id, std::string filename) {
	
	std::ofstream out(filename);
	if (!out.is_open()) {
		std::cerr << "create file " << filename
			      << "failed.\n";
		return;
	}
	
	for (int i = 0; i < MAX_GAME_NUM; ++i) {
		message msg(1, rand()%3+1), msg_get;
		SendMessage(queue_id, msg);
		ReceiveMessage(res_id, msg_get);
		out << hands[msg.data] << " " << msg_get.data << "\n";
	}
	out.close();
	std::cout << "Player process complete.\n";
}

/* Judger function, acquire p1 and p2's data, then write result */
void Judger(int q1, int q2, int a1, int a2, std::string filename) {
	
	std::ofstream out(filename);
	if (!out.is_open()) {
		std::cerr << "create file " << filename
			      << "failed.\n";
		return;
	}

	message msg_1, msg_2;
	message res_1, res_2;
	
	for (int i = 0; i < MAX_GAME_NUM; ++i) {
		ReceiveMessage(q1, msg_1);
		ReceiveMessage(q2, msg_2);

		if (msg_1.data == msg_2.data) {
			res_1.data = 0;
		} else if(msg_1.data+msg_2.data == 4) {
			if (msg_1.data == 1) res_1.data = -1;
			else res_1.data = 1;
		} else {
			if (msg_1.data < msg_2.data) res_1.data = 1;
			else res_1.data = -1;
		}
		res_2.data = -res_1.data;
		
		SendMessage(a1, res_1);
		SendMessage(a2, res_2);

		out << hands[msg_1.data] << " " << hands[msg_2.data] << " "
			<< res_1.data << " " << res_2.data << "\n";
	}
	out.close();
	std::cout << "Judger process complete.\n";
}

int main() {
	srand(time(0));
	
	/* define the key parameter */
	const int play1Key = 1111;
	const int play2Key = 2222;
	const int res1Key = 3333;
	const int res2Key = 4444;

	/* create message queue */
	MessageQueue play1Queue(play1Key);
	MessageQueue play2Queue(play2Key);
	MessageQueue res1Queue(res1Key);
	MessageQueue res2Queue(res2Key);
	
	/* create player and judger process */
	std::thread p1(Player, play1Queue.getID(), res1Queue.getID(), "play1.txt");
	std::thread p2(Player, play2Queue.getID(), res2Queue.getID(), "play2.txt");
	std::thread p3(Judger, play1Queue.getID(), play2Queue.getID(), 
				res1Queue.getID(), res2Queue.getID(), "results.txt");
	
	/* wait for them to complete */
	p1.join();
	p2.join();
	p3.join();	
	
	/* read results from file */
	std::ifstream in("results.txt");
	if (!in.is_open()) {
		std::cerr << "open results file failed.\n";
		return 0;
	}
	
	/* count and show results */
	int cnt1 = 0, cnt2 = 0;
	for (int i = 0; i < MAX_GAME_NUM; ++i) {
		std::string temp1, temp2;
		int res1, res2;
		in >> temp1 >> temp2 >> res1 >> res2;
		if (res1 == 1) cnt1++;
		if (res2 == 1) cnt2++;
	}
	
	std::cout << "Player 1 wins " << cnt1 << " times.\n";
	std::cout << "Player 2 wins " << cnt2 << " times.\n";
	std::cout << "Draw " << MAX_GAME_NUM-cnt1-cnt2 << " times.\n";
	
	if (cnt1 == cnt2) {
		std::cout << "So it's a draw.\n";
	} else {
		std::cout << "So the winner is " << (cnt1 > cnt2 ? "Player 1" : "Player 2")
				  << "\n";
	}
	return 0;
}
