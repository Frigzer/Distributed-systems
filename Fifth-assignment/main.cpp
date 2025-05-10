#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>

class Node {
private:
	

public:
	int privateID;
	int publicID;

	int normalID;
	std::vector<Node*> waitingFor;
	std::vector<Node*> blockedBy;


	Node(int _id) : privateID(_id), publicID(_id), normalID(_id) {
		
	}

	void setID(const int _id) {
		privateID = _id;
		publicID = _id;
	}

	bool detect(const Node* blocking) const {
		if (privateID == publicID && publicID == blocking->publicID)
			return true;
		return false;
	}
};

void block(Node* blocked , Node* blocking) {
	int newID = std::max(blocked->publicID, blocking->publicID) + 1;
	blocked->setID(newID);

	std::cout << "[BLOCK] " << blocking->normalID << " blocked " << blocked->normalID << ", new ID = " << newID << std::endl;
}

bool transmit(Node* current) {
	bool updated = false;

	for (Node* blocker : current->blockedBy) {
		if (blocker->publicID > current->publicID) {
			current->publicID = blocker->publicID;
			updated = true;
		}
	}

	return updated;
}

std::vector<Node> readNodesFromFile(const std::string filename) {
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Failed to open " << filename << std::endl;
		exit(1);
	}

	int N;
	file >> N;

	std::vector<Node> nodes;
	for (int i = 0; i < N; ++i) {
		nodes.emplace_back(i);
	}

	int a, b;
	while (file >> a >> b) {
		Node* blocked = &nodes[a];
		Node* blocking = &nodes[b];

		block(blocked, blocking);
		blocked->blockedBy.push_back(blocking);
		blocking->waitingFor.push_back(blocked);
	}

	return nodes;
}

int main() {
	auto nodes = readNodesFromFile("input-2.txt");

	bool changed;
	do {
		changed = false;
		for (Node& node : nodes) {
			if (transmit(&node))
				changed = true;
		}
	} while (changed);


	for (size_t i = 0; i < nodes.size(); ++i) {
		for (Node* blocker : nodes[i].blockedBy) {
			if (nodes[i].detect(blocker)) {
				std::cout << "Deadlock detected between node: "
					<< i << " and node " << (blocker - &nodes[0]) << "\n";
			}


		}
	}

	for (const Node& node : nodes) {
		std::cout << "Node " << node.publicID << "/" << node.privateID << "\n";
		std::cout << "Blocked by: ";
		for (Node* blocker : node.blockedBy) {
			std::cout <<  blocker->normalID << " ";
		}
		std::cout << std::endl;
	}

	return 0;
}