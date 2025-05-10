#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <fstream>


// Token structure holding the current target and the queue of requests
struct Token {
    int target;                  // target node to receive the token
    std::queue<int> q;               // queue of pending requests
    bool inTransit = false;     // is token currently being forwarded
};

// Node class representing each participant in the ring
class Node {
public:
    int id;                     // Node's ID
    int neighborId;             // ID of the next node in the ring
    bool isPhold = false;       // true if this node currently holds the token
    std::queue<std::pair<std::string, int>> inbox; // incoming messages: TR (token request) or TKN (token)

    Node(int id, int totalNodes) : id(id) {
        neighborId = (id + 1) % totalNodes; // set neighbor in ring
    }

    // Initiates a TR message to neighbor
    void sendTokenRequest(std::vector<Node>& nodes) {
        nodes[neighborId].inbox.push({ "TR", id });
        std::cout << "[New Request] Node " << id << " sent TR to Node " << neighborId << std::endl;
    }

    // Forwards any message to the next neighbor
    void forwardMessage(std::pair<std::string, int> msg, std::vector<Node>& nodes) {
        nodes[neighborId].inbox.push(msg);
    }
};

// Utility to print the current token queue
void printQueue(std::queue<int> q) {
   std:: cout << "Current Token Queue: [";
    while (!q.empty()) {
        std::cout << q.front();
        q.pop();
        if (!q.empty()) std::cout << ", ";
    }
    std::cout << "]\n";
}

/*
Expected input format (input.txt):

Line 1: number of nodes N (e.g., 5)
Lines 2...: list of requests (node IDs that request access to CS), e.g.:
5
0
3
2
1
4
0
3
1
4
2
*/

int main() {
    std::string filename;

    std::cout << "Enter filename (must be in the same folder): ";
    std::cin >> filename;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return 1;
    }

    int N;
    file >> N;
    std::vector<int> scheduledRequests;
    int val;
    while (file >> val) {
        scheduledRequests.push_back(val); // read each TR request
    }
    file.close();

    std::vector<Node> nodes;
    for (int i = 0; i < N; ++i)
        nodes.emplace_back(i, N); // initialize nodes with ring neighbors

    int phold = 0; // initial token holder
    nodes[phold].isPhold = true;

    Token token;
    token.inTransit = false;

    int successCount = 0;
    int iteration = 0;
    int requestIndex = 0;
    int M = scheduledRequests.size(); // total number of requests

    while (successCount < M) {
        std::cout << "\n=== Iteration " << iteration++ << " ===\n";

        // Send next scheduled TR if available
        if (requestIndex < M) {
            int requester = scheduledRequests[requestIndex++];
            nodes[requester].sendTokenRequest(nodes);
        }

        // Process all incoming messages for each node
        for (auto& node : nodes) {
            std::queue<std::pair<std::string, int>> remaining;
            while (!node.inbox.empty()) {
                auto msg = node.inbox.front();
                node.inbox.pop();

                if (msg.first == "TR") {
                    if (node.isPhold) {
                        token.q.push(msg.second);
                        std::cout << "[Phold] Node " << node.id << " received TR from Node " << msg.second << std::endl;
                    }
                    else {
                        node.forwardMessage(msg, nodes);
                        std::cout << "[Forwarded TR] Node " << node.id << " forwarded TR from Node " << msg.second << " to Node " << node.neighborId << std::endl;
                    }
                }
                else if (msg.first == "TKN") {
                    if (node.id == token.target) {
                        node.isPhold = true;
                        token.inTransit = false;
                        std::cout << "[TOKEN RECEIVED] Node " << node.id << " is new Phold\n";
                        successCount++;
                        printQueue(token.q);
                    }
                    else {
                        node.forwardMessage(msg, nodes);
                        std::cout << "[Forwarded TOKEN] Node " << node.id << " forwarded token to Node " << node.neighborId << std::endl;
                    }
                }
            }
            node.inbox = remaining;
        }

        // If Phold has pending requests and token is free, send token to next target
        if (!token.inTransit && phold != -1 && nodes[phold].isPhold && !token.q.empty()) {
            int next = token.q.front();
            token.q.pop();

            if (next == phold) {
                // If token is for current Phold, process immediately
                std::cout << "[TOKEN SELF-HANDLED] Node " << phold << " kept the token\n";
                successCount++;
                printQueue(token.q);
                continue;
            }

            token.target = next;
            token.inTransit = true;
            nodes[phold].isPhold = false;
            nodes[phold].forwardMessage({ "TKN", next }, nodes);

            std::cout << "[TOKEN SENT] From Node " << phold << " to Node " << next << std::endl;
            printQueue(token.q);
            phold = -1; // token is in transit, Phold unknown
        }

        // Update who is the current Phold after token is received
        for (auto& node : nodes) {
            if (node.isPhold) {
                phold = node.id;
                break;
            }
        }
    }

    std::cout << "\nAll " << M << " requests have been successfully processed.\n";
    return 0;
}
