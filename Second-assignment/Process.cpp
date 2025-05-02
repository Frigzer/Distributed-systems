#include "Process.h"

bool Process::Request::operator<(const Request& other) const {
	return (timestamp < other.timestamp) || (timestamp == other.timestamp && sender < other.sender);
}

Process::Process(int _id, int n) : id(_id), clock(n), requesting(false) {
	goAhead.resize(n, 0);
	csCount = 0;
}

void Process::requestCS(std::vector<Process>& all) {
	clock++;
	requesting = true;
	currentRequest = { clock,id };
	std::fill(goAhead.begin(), goAhead.end(), 0);
	goAhead[id] = 1;

	for (Process& p : all) {
		if (p.id != id) {
			p.receiveRequest(currentRequest, all);
		}
	}
}

void Process::receiveRequest(const Request& req, std::vector<Process>& all) {
	clock = std::max(clock, req.timestamp) + 1;
	std::cout << "\t[REQUEST] P" << req.sender << " -> P" << id << "\n";

	if (!requesting || req < currentRequest) {
		std::cout << "\t[REPLY]   P" << id << " replies to P" << req.sender << "\n";
		all[req.sender].receiveReply(id);
	}
	else {
		std::cout << "\t[DEFER]   P" << id << " defers reply to P" << req.sender << "\n";
		deferred.push_back(req.sender);
	}
}

void Process::receiveReply(int from) {
	clock++;
	std::cout << "\t[REPLY RECEIVED] P" << id << " got reply from P" << from << "\n";
	goAhead[from] = 1;
}

bool Process::canEnterCS() {
	for (int i = 0; i < goAhead.size(); ++i) {
		if (i != id && goAhead[i] == 0)
			return false;
	}
	return true;
}

void Process::exitCS(std::vector<Process>& all) {
	requesting = false;
	csCount++;
	clock++;

	std::cout << "\t-> Process " << id << " replying to deferred: ";
	for (int pid : deferred) {
		all[pid].receiveReply(id);
		std::cout << pid << " ";
	}
	std::cout << std::endl;

	deferred.clear();
}

int Process::getClock() const {
	return clock;
}

int Process::getId() const {
	return id;
}

int Process::getCSCount() const {
	return csCount;
}


