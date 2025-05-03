#pragma once
#include <vector>
#include <iostream>
#include <iomanip>

class Process {
private:
    struct Request {
        int timestamp;
        int sender;

        bool operator<(const Request& other) const;
    };

    int id;
    int clock;
    int csCount;
    bool requesting;
    Request currentRequest;
    std::vector<int> goAhead;
    std::vector<int> deferred;

public:
    Process(int _id, int n);

    void requestCS(std::vector<Process>& all, int reqId);

    void receiveRequest(const Request& req, std::vector<Process>& all);

    void receiveReply(int from);

    bool canEnterCS();

    void exitCS(std::vector<Process>& all);

    int getClock() const;
    int getId() const;
    int getCSCount() const;
};