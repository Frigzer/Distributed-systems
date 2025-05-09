#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

struct Token {
    int currentHolder;
    queue<int> requestQueue;
};

class Node {
public:
    int id;
    int neighborId;
    bool isPhold;

    Node(int id, int totalNodes) : id(id), isPhold(false) {
        neighborId = (id + 1) % totalNodes;
    }

    void sendTokenRequest(queue<pair<string, int>>& events) {
        events.push({ "TR", id });
    }
};

void printQueue(queue<int> q) {
    cout << "Current Token Queue: [";
    while (!q.empty()) {
        cout << q.front();
        q.pop();
        if (!q.empty()) cout << ", ";
    }
    cout << "]\n";
}

int main() {
    int N = 5;  // Number of nodes
    int M = 10; // Number of requests

    vector<Node> nodes;
    for (int i = 0; i < N; ++i)
        nodes.emplace_back(i, N);

    // Initial Phold
    int initialPhold = 0;
    nodes[initialPhold].isPhold = true;

    Token token;
    token.currentHolder = initialPhold;

    queue<pair<string, int>> events;

    // Generate M requests
    for (int i = 0; i < M; ++i) {
        int requester = rand() % N;
        nodes[requester].sendTokenRequest(events);
    }

    int successCount = 0;

    while (successCount < M) {
        // Step 1: Handle all TR events and put them in Phold's queue
        queue<pair<string, int>> remainingEvents;
        while (!events.empty()) {
            auto event = events.front();
            events.pop();
            if (event.first == "TR") {
                int requester = event.second;
                // Forward request until it reaches Phold
                int forwardNode = requester;
                while (!nodes[forwardNode].isPhold) {
                    forwardNode = nodes[forwardNode].neighborId;
                }
                // Phold gets the request
                token.requestQueue.push(requester);
            }
            else {
                remainingEvents.push(event);
            }
        }

        // Step 2: Phold finishes CS and passes the token
        if (!token.requestQueue.empty()) {
            int nextHolder = token.requestQueue.front();
            token.requestQueue.pop();

            nodes[token.currentHolder].isPhold = false;
            nodes[nextHolder].isPhold = true;
            token.currentHolder = nextHolder;

            successCount++;
            printQueue(token.requestQueue);
        }

        events = remainingEvents;
    }

    cout << "All " << M << " requests have been successfully processed.\n";
    return 0;
}
