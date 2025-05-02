#pragma once
#include <vector>

class Process {
private:
    struct Request {
        int timestamp;
        int sender;

        bool operator<(const Request& other) const;
    };

    int id;
    int clock;
    bool requesting;
    Request currentRequest;
    std::vector<int> goAhead;
    std::vector<int> deferred;

public:
    Process(int _id, int n);

    void requestCS(std::vector<Process>& all);

    void receiveRequest(const Request& req, std::vector<Process>& all);

    void receiveReply(int from);

    bool canEnterCS();

    void exitCS(std::vector<Process>& all);
};