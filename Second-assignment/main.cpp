
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

    for (int i = 0; i < M; ++i) {
        int pid = std::rand() % N;

        processes[pid].requestCS(processes, i + 1);

        while (!processes[pid].canEnterCS()) {
            
        }

        processes[pid].exitCS(processes);
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
