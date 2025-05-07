#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

struct Node {
    int id;
    int parent;
    Node* holder = nullptr;
    bool usingCS = false;
    bool asked = false;
    queue<Node*> requestQueue;

    Node(int _id, int _parent = -1) : id(_id), parent(_parent) {}
};

unordered_map<int, Node*> nodes;

void assignPrivilege(Node* node);
void request(Node* node);

void returnTokenToRoot(Node* node) {
    while (node->holder == node && !node->usingCS && node->requestQueue.empty() && node->parent != -1) {
        Node* parent = nodes[node->parent];

        // Only return if parent doesn't already hold the token
        if (parent->holder != parent) {
            cout << "Returning token from Node " << node->id << " to parent Node " << parent->id << "\n";
            node->holder = parent;
            parent->holder = parent;
            // Do NOT push 'node' into parent's request queue
        }

        node = parent;
    }
}

void printState() {
    cout << "------------------------------\n";
    for (const auto& it : nodes) {
        const Node* node = it.second;
        cout << "Node " << node->id
            << " | Parent: " << node->parent
            << " | Token: " << (node->holder == node ? "Yes" : "No")
            << " | In CS: " << (node->usingCS ? "Yes" : "No")
            << " | Queue: ";
        queue<Node*> q = node->requestQueue;
        while (!q.empty()) {
            cout << q.front()->id << " ";
            q.pop();
        }
        cout << "\n";
    }
    cout << "------------------------------\n";
}

void request(Node* node) {
    if (node->holder == node) {
        bool alreadyInQueue = false;
        queue<Node*> temp;
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
        node->asked = true;
        cout << "Node " << node->id << " sends request to Node " << node->holder->id << "\n";
        node->holder->requestQueue.push(node);
        request(node->holder);
    }
}

void assignPrivilege(Node* node) {
    while (node->holder == node && !node->usingCS && !node->requestQueue.empty()) {
        Node* next = node->requestQueue.front();
        node->requestQueue.pop();
        node->asked = false;

        if (next == node) {
            node->usingCS = true;
            cout << "Node " << node->id << " enters critical section\n";
            printState();
            node->usingCS = false;
            cout << "Node " << node->id << " exits critical section\n";
        }
        else {
            node->holder = next;
            cout << "Token passed from Node " << node->id << " to Node " << next->id << "\n";
            next->holder = next;
            printState();
            assignPrivilege(next);
            break;
        }
    }
}

void loadGraph(const string& filename) {
    ifstream infile(filename);
    int id, parent;
    while (infile >> id >> parent) {
        if (!nodes.count(id)) nodes[id] = new Node(id, parent);
        nodes[id]->parent = parent;
    }
    infile.close();
}

vector<int> loadRequests(const string& filename) {
    ifstream infile(filename);
    vector<int> requests;
    int r;
    while (infile >> r) requests.push_back(r);
    infile.close();
    return requests;
}

int main() {
    loadGraph("graph-1.txt");
    vector<int> reqs = loadRequests("requests-1.txt");

    int rootId = 0;
    nodes[rootId]->holder = nodes[rootId];

    for (auto& it : nodes) {
        if (it.first != rootId && it.second->parent != -1) {
            it.second->holder = nodes[it.second->parent];
        }
    }

    for (int r : reqs) {
        cout << "\n>> Node " << r << " is requesting the token\n";
        request(nodes[r]);
        printState();
    }

    bool changed = true;
    while (changed) {
        changed = false;

        // Przyznaj token, jeœli ktoœ go ma i s¹ oczekuj¹ce ¿¹dania
        for (auto& it : nodes) {
            Node* node = it.second;
            if (node->holder == node && !node->usingCS && !node->requestQueue.empty()) {
                assignPrivilege(node);
                changed = true;
            }
        }

        // Globalne sprawdzenie, czy ktoœ czeka, ale nikt nie wys³a³ ¿¹dania
        for (auto& it : nodes) {
            Node* node = it.second;
            if (!node->requestQueue.empty() && node->holder != node && !node->asked) {
                node->asked = true;
                cout << "Node " << node->id << " re-sends (global check) request to Node " << node->holder->id << "\n";
                node->holder->requestQueue.push(node);
                changed = true;
            }
        }
    }

    for (auto& it : nodes) {
        returnTokenToRoot(it.second);
    }


    printState();

    for (auto& pair : nodes) delete pair.second;
    return 0;
}



