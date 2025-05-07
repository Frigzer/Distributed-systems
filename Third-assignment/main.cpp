#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>

/*
==========================================
INPUT FORMAT DESCRIPTION
==========================================

>> graph-x.txt (tree structure)
Each line contains two integers: NODE_ID PARENT_ID
Example:
    0 -1     // root node (ID 0, no parent)
    1 0      // node 1 is child of 0
    2 0      // node 2 is child of 0
    3 1      // node 3 is child of 1
    4 1      // node 4 is child of 1
    5 2      // node 5 is child of 2
    6 2      // node 6 is child of 2

>> requests-x.txt (CS requests)
Each line contains a single node ID that requests access to the critical section.
Example:
    6
    3
    4

These request events are processed in order.
==========================================
*/

// Structure representing each node in the tree
struct Node {
    int id;
    int parent;
    Node* holder = nullptr;             // Who currently holds the token
    bool usingCS = false;               // Is currently in critical section
    bool asked = false;                 // Has sent a request upward
    std::queue<Node*> requestQueue;     // Queue of pending requests

    Node(int _id, int _parent = -1) : id(_id), parent(_parent) {}
};

// Mapping of node IDs to Node pointers
std::unordered_map<int, Node*> nodes;

void assignPrivilege(Node* node);
void request(Node* node);

// Return the token back toward the root when it's idle
void returnTokenToRoot(Node* node) {
    while (node->holder == node && !node->usingCS && node->requestQueue.empty() && node->parent != -1) {
        Node* parent = nodes[node->parent];

        // Return only if parent does not already hold the token
        if (parent->holder != parent) {
            std::cout << "Returning token from Node " << node->id << " to parent Node " << parent->id << "\n";
            node->holder = parent;
            parent->holder = parent;
            // Do not push this node to the parent's queue (no new request)
        }

        node = parent;
    }
}

// Print the current state of all nodes
void printState() {
    std::cout << "------------------------------\n";
    for (const auto& it : nodes) {
        const Node* node = it.second;
        std::cout << "Node " << node->id
            << " | Parent: " << node->parent
            << " | Token: " << (node->holder == node ? "Yes" : "No")
            << " | In CS: " << (node->usingCS ? "Yes" : "No")
            << " | Queue: ";
        std::queue<Node*> q = node->requestQueue;
        while (!q.empty()) {
            std::cout << q.front()->id << " ";
            q.pop();
        }
        std::cout << "\n";
    }
    std::cout << "------------------------------\n";
}

// Send a request to access the critical section
void request(Node* node) {
    if (node->holder == node) {
        // If node already has the token, enqueue itself if not already present
        bool alreadyInQueue = false;
        std::queue<Node*> temp;
        while (!node->requestQueue.empty()) {
            if (node->requestQueue.front() == node) alreadyInQueue = true;
            temp.push(node->requestQueue.front());
            node->requestQueue.pop();
        }
        node->requestQueue = temp;

        if (!alreadyInQueue) node->requestQueue.push(node);
        assignPrivilege(node);
    }
    else if (!node->asked) {
        // Forward request up the tree
        node->asked = true;
        std::cout << "Node " << node->id << " sends request to Node " << node->holder->id << "\n";
        node->holder->requestQueue.push(node);
        request(node->holder);
    }
}

// Try to pass the token to the next eligible requester
void assignPrivilege(Node* node) {
    while (node->holder == node && !node->usingCS && !node->requestQueue.empty()) {
        Node* next = node->requestQueue.front();
        node->requestQueue.pop();
        node->asked = false;

        // Node enters critical section
        if (next == node) {
            node->usingCS = true;
            std::cout << "Node " << node->id << " enters critical section\n";
            printState();
            node->usingCS = false;
            std::cout << "Node " << node->id << " exits critical section\n";
        }
        else {
            // Pass token to another node
            node->holder = next;
            std::cout << "Token passed from Node " << node->id << " to Node " << next->id << "\n";
            next->holder = next;
            printState();
            assignPrivilege(next); // Try to process requests immediately
            break;
        }
    }
}

// Load tree structure from input file (format: id parent)
void loadGraph(const std::string& filename) {
    std::ifstream infile(filename);
    int id, parent;
    while (infile >> id >> parent) {
        if (!nodes.count(id)) nodes[id] = new Node(id, parent);
        nodes[id]->parent = parent;
    }
    infile.close();
}

// Load list of request IDs (each line: one requesting node)
std::vector<int> loadRequests(const std::string& filename) {
    std::ifstream infile(filename);
    std::vector<int> requests;
    int r;
    while (infile >> r) requests.push_back(r);
    infile.close();
    return requests;
}

int main() {
    // ---- Load inputs ----
    std::string graphFilename;
    std::cout << "Enter the graph filename (must be in the same directory): ";
    std::cin >> graphFilename;
    loadGraph(graphFilename);


    std::string requestsFilename;
    std::cout << "Enter the requests filename (must be in the same directory): ";
    std::cin >> requestsFilename;
    std::vector<int> reqs = loadRequests(requestsFilename);

    int rootId = 0;
    nodes[rootId]->holder = nodes[rootId];

    // Assign initial holder to be the parent (Raymond's setup)
    for (auto& it : nodes) {
        if (it.first != rootId && it.second->parent != -1) {
            it.second->holder = nodes[it.second->parent];
        }
    }

    // ---- Process Requests ----
    for (int r : reqs) {
        std::cout << "\n>> Node " << r << " is requesting the token\n";
        request(nodes[r]);
        printState();
    }

    // ---- Global Recovery Loop ----
    bool changed = true;
    while (changed) {
        changed = false;

        // Try to satisfy any pending requests
        for (auto& it : nodes) {
            Node* node = it.second;
            if (node->holder == node && !node->usingCS && !node->requestQueue.empty()) {
                assignPrivilege(node);
                changed = true;
            }
        }

        // Check for unsent requests that were forgotten
        for (auto& it : nodes) {
            Node* node = it.second;
            if (!node->requestQueue.empty() && node->holder != node && !node->asked) {
                node->asked = true;
                std::cout << "Node " << node->id << " re-sends (global check) request to Node " << node->holder->id << "\n";
                node->holder->requestQueue.push(node);
                changed = true;
            }
        }
    }

    // ---- Return token back to root if idle ----
    for (auto& it : nodes) {
        returnTokenToRoot(it.second);
    }

    // Print the final system state
    std::cout << "\n== FINAL STATE ==\n";
    printState();

    // Cleanup
    for (auto& pair : nodes) delete pair.second;
    return 0;
}



