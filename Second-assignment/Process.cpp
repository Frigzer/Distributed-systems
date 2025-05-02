#include "Process.h"

bool Process::Request::operator<(const Request& other) const {
	return (timestamp < other.timestamp) || (timestamp == other.timestamp && sender < other.sender);
}

Process::Process(int _id, int n) : id(_id), clock(n), requesting(false) {
	goAhead.resize(n, 0);
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
	if (!requesting || req < currentRequest) {
		all[req.sender].receiveReply(id);
	}
	else {
		deferred.push_back(req.sender);
	}
}

void Process::receiveReply(int from) {
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
	for (int pid : deferred) {
		all[pid].receiveReply(id);
	}
	deferred.clear();
}


