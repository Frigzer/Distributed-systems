#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>

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
    void sendTokenRequest(std::vector<Node>& nodes, std::vector<std::string>& logBuffer) {
        nodes[neighborId].inbox.push({ "TR", id });
        std::ostringstream ss;
        ss << "TR " << id << "->" << neighborId;
        logBuffer.push_back(ss.str());
    }

    // Forwards any message to the next neighbor
    void forwardMessage(std::pair<std::string, int> msg, std::vector<Node>& nodes, std::vector<std::string>& logBuffer) {
        nodes[neighborId].inbox.push(msg);
        if (msg.first == "TR") {
            std::ostringstream ss;
            ss << "TR " << msg.second << "->" << neighborId;
            logBuffer.push_back(ss.str());
        }
        else if (msg.first == "TKN") {
            std::ostringstream ss;
            ss << "TKN ->" << neighborId;
            logBuffer.push_back(ss.str());
        }
    }
};

std::string formatQueue(std::queue<int> q) {
    std::ostringstream ss;
    ss << "Q=[";
    while (!q.empty()) {
        ss << q.front();
        q.pop();
        if (!q.empty()) ss << ",";
    }
    ss << "]";
    return ss.str();
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

    std::cout << std::endl;

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
        std::vector<std::string> logBuffer; // collect log events for this iteration

        // Send next scheduled TR if available
        if (requestIndex < M) {
            int requester = scheduledRequests[requestIndex++];
            nodes[requester].sendTokenRequest(nodes, logBuffer);
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
                        std::ostringstream ss;
                        ss << "PH " << node.id << " <- TR " << msg.second;
                        logBuffer.push_back(ss.str());
                    }
                    else {
                        node.forwardMessage(msg, nodes, logBuffer);
                    }
                }
                else if (msg.first == "TKN") {
                    if (node.id == token.target) {
                        node.isPhold = true;
                        token.inTransit = false;
                        std::ostringstream ss;
                        ss << "PH = " << node.id << " (RECEIVED TKN)";
                        logBuffer.push_back(ss.str());
                        successCount++;
                    }
                    else {
                        node.forwardMessage(msg, nodes, logBuffer);
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
                std::ostringstream ss;
                ss << "PH = " << phold << " (SELF-HANDLE)";
                logBuffer.push_back(ss.str());
                successCount++;
            }
            else {
                token.target = next;
                token.inTransit = true;
                nodes[phold].isPhold = false;
                nodes[phold].forwardMessage({ "TKN", next }, nodes, logBuffer);
                std::ostringstream ss;
                ss << "TKN sent -> " << next;
                logBuffer.push_back(ss.str());
                phold = -1;
            }
        }

        // Update Phold
        for (auto& node : nodes) {
            if (node.isPhold) {
                phold = node.id;
                break;
            }
        }

        // Print one-line log for this iteration
        std::cout << "It " << iteration++ << " | ";
        for (size_t i = 0; i < logBuffer.size(); ++i) {
            std::cout << logBuffer[i];
            if (i != logBuffer.size() - 1) std::cout << " | ";
        }
        if (!token.q.empty()) {
            std::cout << " | " << formatQueue(token.q);
        }
        std::cout << std::endl;
    }

    std::cout << "\nAll " << M << " requests have been successfully processed.\n";
    return 0;
}
