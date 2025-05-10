#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

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

void printRingState(const vector<Node>& nodes) {
    cout << "[Ring State]\n";
    for (const auto& node : nodes) {
        cout << "Node " << node.id << ": Phold = " << (node.isPhold ? "true" : "false");
        if (node.isPhold) cout << "  <-- has token";
        cout << endl;
    }
    cout << endl;
}

int main() {
    srand(time(0));

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

    int successCount = 0, sentRequests = 0;
    int csDelayCounter = 0;
    bool inCS = false;
    int currentTarget = -1;

    int iteration = 0;

    while (successCount < M || sentRequests < M) {
        cout << "\n=== Iteration " << iteration << " ===\n";

        // Generate M requests
        if (sentRequests < M && (rand() % 100) < 30) {
            int requester = rand() % N;
            nodes[requester].sendTokenRequest(events);
            sentRequests++;
            cout << "[New Request] Node " << requester << " sent TR\n";
            
        }


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
                printQueue(token.requestQueue);
            }
            else {
                remainingEvents.push(event);
            }
        }

        // Step 2: Phold finishes CS and passes the token
        if (!token.requestQueue.empty()) {
            if (!inCS) {
                inCS = true;
                csDelayCounter = 4;
                currentTarget = token.requestQueue.front();
                cout << "[CS] Phold started processing request for Node " << currentTarget << "\n";
            }
            else {
                csDelayCounter--;
                if (csDelayCounter == 0) {
                    cout << "[CS] Phold finished processing Node " << currentTarget << "\n";
                    int currentPhold = token.currentHolder;
                    int nextHolder = token.requestQueue.front();
                    token.requestQueue.pop();

                    nodes[currentPhold].isPhold = false;
                    nodes[nextHolder].isPhold = true;
                    token.currentHolder = nextHolder;

                    successCount++;
                    printQueue(token.requestQueue);
                    printRingState(nodes);

                    inCS = false;
                }
            }
        }

       events = remainingEvents;
       iteration++;
    }

    cout << "All " << M << " requests have been successfully processed.\n";
    return 0;
}
