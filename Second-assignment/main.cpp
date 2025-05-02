
#include "Process.h"

int main() {
    srand(time(0));

    int N, M;
    std::cout << "Enter number of processes: ";
    std::cin >> N;

    std::cout << "Enter number of requests: ";
    std::cin >> M;

    std::vector<Process> processes;
    for (int i = 0; i < N; ++i) {
        processes.emplace_back(i, N);
    }

    // Wylosuj M procesów, które będą próbowały wejść do CS
    for (int i = 0; i < M; ++i) {
        int pid = std::rand() % N;

        std::cout << "\n[REQ " << i + 1 << "] Process " << pid << " is requesting CS\n";
        processes[pid].requestCS(processes);

        // Czekaj aż proces uzyska wszystkie REPLY
        while (!processes[pid].canEnterCS()) {
            // symulowane czekanie
        }

        std::cout << "       → Process " << pid << " ENTERED critical section.\n";

        processes[pid].exitCS(processes);
        std::cout << "       → Process " << pid << " EXITED critical section.\n";
    }

    return 0;
}
