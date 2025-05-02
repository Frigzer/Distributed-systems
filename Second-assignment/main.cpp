
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

        std::cout << "\t-> Process " << pid << " ENTERED critical section.\n";

        processes[pid].exitCS(processes);
        std::cout << "\t-> Process " << pid << " EXITED critical section.\n";
    }

    std::cout << "\n=== Final State of Processes ===\n";
    for (const auto& p : processes) {
        std::cout << "Process " << p.getId()
            << " | Final clock: " << p.getClock()
            << " | Times in CS: " << p.getCSCount()
            << std::endl;
    }

    return 0;
}
