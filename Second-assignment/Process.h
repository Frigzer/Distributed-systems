#pragma once
#include <vector>
#include <iostream>
#include <iomanip>

// This class represents a single process in the Ricart-Agrawala mutual exclusion algorithm.
class Process {
private:
    // Struct to represent a request to enter the critical section
    struct Request {
        int timestamp;
        int sender;

        bool operator<(const Request& other) const;
    };

    int id;                     // Process ID
    int clock;                  // Logical clock (Lamport clock)
    int csCount;                // Number of times the process entered CS
    bool requesting;            // Is the process currently requesting CS?
    Request currentRequest;     // Active request from this process
    std::vector<int> goAhead;   // Tracks REPLYs received (1 = received)
    std::vector<int> deferred;  // List of processes whose replies are deferred

public:
    Process(int _id, int n);

    // Called when this process requests to enter the CS
    void requestCS(std::vector<Process>& all, int reqId);

    // Called when another process sends this one a REQUEST
    void receiveRequest(const Request& req, std::vector<Process>& all);

    // Called when this process receives a REPLY
    void receiveReply(int from);

    // Returns true if this process can now enter the CS
    bool canEnterCS();

    // Called when this process exits the CS and processes deferred replies
    void exitCS(std::vector<Process>& all);

    int getClock() const;
    int getId() const;
    int getCSCount() const;
};