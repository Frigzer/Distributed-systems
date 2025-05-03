#include "Process.h"

// Entry point for the Ricart-Agrawala mutual exclusion simulation.
// This simulates a distributed system with N processes,
// each randomly attempting to enter the critical section M times.

int main() {
    srand(time(0));

    // Input format:
    // First, the user is prompted to enter:
    //   - Number of processes in the distributed system (N)
    //   - Number of total critical section requests (M)
    // The requests will be randomly assigned to processes.

    int N, M;
    std::cout << "Enter number of processes: ";
    std::cin >> N;

    std::cout << "Enter number of requests: ";
    std::cin >> M;

    // Create N independent processes (simulating nodes in a distributed system)
    std::vector<Process> processes;
    for (int i = 0; i < N; ++i) {
        processes.emplace_back(i, N);
    }

    // Perform M critical section requests from randomly chosen processes
    for (int i = 0; i < M; ++i) {
        int pid = std::rand() % N;

        // Simulate process sending a request to enter the CS
        processes[pid].requestCS(processes, i + 1);

        // Wait (simulate) until this process has received permission from all others
        while (!processes[pid].canEnterCS()) {
            // busy wait for simplicity in this simulation
        }

        // Exit the critical section and notify deferred processes
        processes[pid].exitCS(processes);
    }

    // Print final logical clocks and number of times each process entered CS
    std::cout << "\n=== Final State of Processes ===\n";
    for (const auto& p : processes) {
        std::cout << "Process " << p.getId()
            << " | Final clock: " << p.getClock()
            << " | Times in CS: " << p.getCSCount()
            << std::endl;
    }

    return 0;
}
