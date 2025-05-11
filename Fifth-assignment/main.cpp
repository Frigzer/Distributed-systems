#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>

class Node {
private:
	int privateLabel;

public:
	int publicLabel;

	int id;
	std::vector<Node*> waitingFor;
	std::vector<Node*> blockedBy;


	Node(int _id) : privateLabel(_id), publicLabel(_id), id(_id) {
		
	}

	void setID(const int _id) {
		privateLabel = _id;
		publicLabel = _id;
	}

	bool detect(const Node* blocking) const {
		if (privateLabel == publicLabel && publicLabel == blocking->publicLabel)
			return true;
		return false;
	}

	friend std::ostream& operator<<(std::ostream& out, const Node& node);
};

std::ostream& operator<<(std::ostream& out, const Node& node) {
	out << "Node ID: " << node.id << ", " << node.publicLabel << "/" << node.privateLabel;
	return out;
}

void block(Node* blocked , Node* blocking) {
	int newID = std::max(blocked->publicLabel, blocking->publicLabel) + 1;
	blocked->setID(newID);

	std::cout << "[BLOCK] " << blocking->id << " blocked " << blocked->id << ", new ID = " << newID << std::endl;
}

bool transmit(Node* current) {
	bool updated = false;

	for (Node* blocker : current->blockedBy) {
		if (blocker->publicLabel > current->publicLabel) {
			std::cout << "[TRANSMIT] Node " << blocker->id << " -> Node " << current->id
				<< " (updating " << current->publicLabel
				<< " -> " << blocker->publicLabel << ")\n";

			current->publicLabel = blocker->publicLabel;
			updated = true;
		}
	}

	return updated;
}

std::vector<Node> processGraphFromFile(const std::string filename) {
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

	bool deadlockDetected = false;

	int a, b;
	while (file >> a >> b) {
		Node* blocked = &nodes[a];
		Node* blocking = &nodes[b];

		block(blocked, blocking);
		blocked->blockedBy.push_back(blocking);
		blocking->waitingFor.push_back(blocked);

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
					std::cout << "[DETECT] Deadlock detected between node: "
						<< i << " and node " << (blocker - &nodes[0]) << "\n";
					deadlockDetected = true;
				}


			}
		}
	}

	if (!deadlockDetected) {
		std::cout << "No deadlocks detected. System is safe.\n";
	}

	return nodes;
}

int main() {
	std::string filename;
	std::cout << "Enter file name (must be in the same folder): ";
	std::cin >> filename;

	auto nodes = processGraphFromFile(filename);
	
	std::cout << "\n======== FINAL STATE ========\n";
	for (const Node& node : nodes) {
		std::cout << node;
		std::cout << ", blocked by: ";
		for (Node* blocker : node.blockedBy) {
			std::cout << blocker->id << " ";
		}
		std::cout << std::endl;
	}

	return 0;
}