#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Token {
    int target; // kto ma dostac tokena
    queue<int> q; // kolejne ID do przekazania tokena
    bool inTransit = false; // czy token jest w drodze
};

class Node {
public:
    int id;
    int neighborId;
    bool isPhold = false;
    queue<pair<string, int>> inbox; // komunikaty TR lub TKN

    Node(int id, int totalNodes) : id(id) {
        neighborId = (id + 1) % totalNodes;
    }

    void sendTokenRequest(vector<Node>& nodes) {
        nodes[neighborId].inbox.push({ "TR", id });
        cout << "[New Request] Node " << id << " sent TR to Node " << neighborId << endl;
    }

    void forwardMessage(pair<string, int> msg, vector<Node>& nodes) {
        nodes[neighborId].inbox.push(msg);
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
    srand(time(0));
    int N = 5;
    int M = 10;

    vector<Node> nodes;
    for (int i = 0; i < N; ++i)
        nodes.emplace_back(i, N);

    int phold = 0;
    nodes[phold].isPhold = true;

    Token token;
    token.inTransit = false;

    int sentRequests = 0;
    int successCount = 0;
    int iteration = 0;

    while (successCount < M) {
        cout << "\n=== Iteration " << iteration++ << " ===\n";

        // generate new request
        if (sentRequests < M && (rand() % 100) < 30) {
            int requester = rand() % N;
            nodes[requester].sendTokenRequest(nodes);
            sentRequests++;
        }

        // process messages
        for (auto& node : nodes) {
            queue<pair<string, int>> remaining;
            while (!node.inbox.empty()) {
                auto msg = node.inbox.front();
                node.inbox.pop();

                if (msg.first == "TR") {
                    if (node.isPhold) {
                        token.q.push(msg.second);
                        cout << "[Phold] Node " << node.id << " received TR from Node " << msg.second << endl;
                    }
                    else {
                        node.forwardMessage(msg, nodes);
                        cout << "[Forwarded TR] Node " << node.id << " forwarded TR from Node " << msg.second << " to Node " << node.neighborId << endl;
                    }
                }
                else if (msg.first == "TKN") {
                    if (node.id == token.target) {
                        node.isPhold = true;
                        token.inTransit = false;
                        cout << "[TOKEN RECEIVED] Node " << node.id << " is new Phold\n";
                        successCount++;
                        printQueue(token.q);
                    }
                    else {
                        node.forwardMessage(msg, nodes);
                        cout << "[Forwarded TOKEN] Node " << node.id << " forwarded token to Node " << node.neighborId << endl;
                    }
                }
            }
            node.inbox = remaining;
        }

        // if Phold not busy and queue has requests, send token
        if (!token.inTransit && phold != -1 && nodes[phold].isPhold && !token.q.empty()) {
            int next = token.q.front();
            token.q.pop();

            if (next == phold) {
                // self-handle without sending
                cout << "[TOKEN SELF-HANDLED] Node " << phold << " kept the token\n";
                successCount++;
                printQueue(token.q);
                continue;
            }

            token.target = next;
            token.inTransit = true;
            nodes[phold].isPhold = false;
            nodes[phold].forwardMessage({ "TKN", next }, nodes);

            cout << "[TOKEN SENT] From Node " << phold << " to Node " << next << endl;
            printQueue(token.q);
            phold = -1; // nowy phold jeszcze nieznany
        }

        // update current phold
        for (auto& node : nodes) {
            if (node.isPhold) {
                phold = node.id;
                break;
            }
        }
    }

    cout << "\nAll " << M << " requests have been successfully processed.\n";
    return 0;
}
