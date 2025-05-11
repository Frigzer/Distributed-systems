#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>

/*
 * INPUT FILE FORMAT:
 * ------------------
 * First line: single integer N — number of nodes
 * Following lines: pairs of integers "a b" meaning
 * that process 'a' is blocked by process 'b'
 *
 * Example:
 * 5
 * 0 1
 * 1 2
 * 2 0
 * 3 4
 */

class Node {
private:
	int privateLabel; // Private label used for deadlock detection

public:
	int publicLabel;  // Public label propagated via transmit rule
	int id;           // Node's identifier

	// Lists to track blocking relationships
	std::vector<Node*> waitingFor; // Who this node is blocking
	std::vector<Node*> blockedBy;  // Who is blocking this node

	// Constructor initializes labels and ID to the same value
	Node(int _id) : privateLabel(_id), publicLabel(_id), id(_id) {}

	// Updates both private and public labels (used during block)
	void setID(const int _id) {
		privateLabel = _id;
		publicLabel = _id;
	}

	// Deadlock detection rule:
	// If a node's private and public labels are equal,
	// and also equal to the public label of a blocking node,
	// a deadlock is detected.
	bool detect(const Node* blocking) const {
		return privateLabel == publicLabel &&
			publicLabel == blocking->publicLabel;
	}

	// Stream output operator for debugging / printing final state
	friend std::ostream& operator<<(std::ostream& out, const Node& node);
};

// Prints the node's ID and label pair (public/private)
std::ostream& operator<<(std::ostream& out, const Node& node) {
	out << "Node ID: " << node.id << ", " << node.publicLabel << "/" << node.privateLabel;
	return out;
}

// Block rule: when 'blocked' is blocked by 'blocking',
// assign a new label to 'blocked' (max + 1)
void block(Node* blocked, Node* blocking) {
	int newID = std::max(blocked->publicLabel, blocking->publicLabel) + 1;
	blocked->setID(newID);

	std::cout << "[BLOCK] " << blocking->id << " blocked " << blocked->id
		<< ", new ID = " << newID << std::endl;
}

// Transmit rule: if any blocker has a higher public label,
// propagate it to this node. Returns true if an update occurred.
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

// Processes the graph from input file, applying block, transmit, and detect rules
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

		// Step 1: Apply block rule
		block(blocked, blocking);

		// Step 2: Update wait-for relationships
		blocked->blockedBy.push_back(blocking);
		blocking->waitingFor.push_back(blocked);

		// Step 3: Propagate public labels using transmit until stable
		bool changed;
		do {
			changed = false;
			for (Node& node : nodes) {
				if (transmit(&node))
					changed = true;
			}
		} while (changed);

		// Step 4: Check for deadlock after each update
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

// Entry point of the program: reads graph, processes it, and prints final state
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
