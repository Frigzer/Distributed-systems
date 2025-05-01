#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Request {
    int timestamp;
    int sender;
};

struct Process {
    int id;
    int clock = 0;
    bool requesting = false;
    Request currentRequest;
    vector<int> go_ahead;
    vector<int> deferred;
};

bool hasPriority(const Request& a, const Request& b) {
    return (a.timestamp < b.timestamp) || (a.timestamp == b.timestamp && a.sender < b.sender);
}

void sendRequests(vector<Process>& processes, int requesterId) {
    Process& requester = processes[requesterId];
    for (int i = 0; i < processes.size(); ++i) {
        if (i == requesterId) continue;
        Process& receiver = processes[i];

        if (!receiver.requesting || hasPriority(requester.currentRequest, receiver.currentRequest)) {
            requester.go_ahead[i] = 1; // Immediate reply
        }
        else {
            receiver.deferred.push_back(requesterId); // Defer reply
        }
    }
}

bool hasAllReplies(const vector<Process>& processes, int pid) {
    const Process& p = processes[pid];
    for (int i = 0; i < processes.size(); ++i) {
        if (i != pid && p.go_ahead[i] == 0) {
            return false;
        }
    }
    return true;
}

void releaseCS(vector<Process>& processes, int pid) {
    Process& p = processes[pid];
    for (int deferredPid : p.deferred) {
        if (deferredPid >= 0 && deferredPid < processes.size()) {
            processes[deferredPid].go_ahead[p.id] = 1;
        }
    }
    p.deferred.clear();
    p.requesting = false;
}

int main() {
    srand(time(0));

    int N, M;
    cout << "Enter number of processes (nodes): ";
    cin >> N;
    cout << "Enter number of requests: ";
    cin >> M;

    vector<Process> processes;
    for (int i = 0; i < N; ++i) {
        Process proc{ i };
        proc.go_ahead.resize(N, 0); // prepare N-sized go-ahead list
        processes.push_back(proc);
    }

    for (int i = 0; i < M; ++i) {
        int requester = rand() % N;
        Process& p = processes[requester];
        p.clock++;
        p.currentRequest = { p.clock, requester };
        p.requesting = true;
        fill(p.go_ahead.begin(), p.go_ahead.end(), 0);
        p.go_ahead[requester] = 1; // we don't wait for ourselves

        cout << "\nProcess " << requester << " is requesting CS (timestamp = " << p.clock << ")\n";

        sendRequests(processes, requester);

        while (!hasAllReplies(processes, requester)) {
            // passive waiting in simulation
        }

        cout << "Process " << requester << " ENTERED CS. Go-ahead list: ";
        for (int i = 0; i < N; ++i)
            if (i != requester) cout << "P" << i << "=" << p.go_ahead[i] << " ";
        cout << endl;

        releaseCS(processes, requester);
    }

    return 0;
}
