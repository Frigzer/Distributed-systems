#include "Process.h"

bool Process::Request::operator<(const Request& other) const {
	return (timestamp < other.timestamp) || (timestamp == other.timestamp && sender < other.sender);
}

Process::Process(int _id, int n) : id(_id), clock(n), requesting(false) {
	goAhead.resize(n, 0);
	csCount = 0;
}

void Process::requestCS(std::vector<Process>& all, int reqId) {
	if (requesting) {
		std::cout << "\t" << std::left << std::setw(22) << "[SKIP]" << " P" << id << " already requesting\n";
		return;
	}

	clock++;
	requesting = true;
	currentRequest = { clock,id };
	std::fill(goAhead.begin(), goAhead.end(), 0);
	goAhead[id] = 1;

	std::cout << "\n[REQ " << reqId << "] Process " << id << " is requesting CS (ts = " << clock << ")\n";
	std::cout << "\t" << std::left << std::setw(22) 
			  << "[ENTER CS]" << " P" << id << " (clock = " << clock << ")\n";

	for (Process& p : all) {
		if (p.id != id) {
			std::cout << "\t" << std::left << std::setw(22) 
					  << "[SENT REQUEST]" << " P" << id << " -> P" << p.id << "\n";
			p.receiveRequest(currentRequest, all);
		}
	}
}

void Process::receiveRequest(const Request& req, std::vector<Process>& all) {
	clock = std::max(clock, req.timestamp) + 1;
	std::cout << "\t" << std::left << std::setw(22) 
			  << "[RECEIVED REQUEST]" << " P" << id << " got request from P" << req.sender
			  << " (ts = " << req.timestamp << ", clock = " << clock << ")\n";

	if (!requesting || req < currentRequest) {
		std::cout << "\t" << std::left << std::setw(22) 
				  << "[SENT REPLY]" << " P" << id << " replies to P" << req.sender << "\n";
		all[req.sender].receiveReply(id);
	}
	else {
		std::cout << "\t" << std::left << std::setw(22) 
				  << "[DEFER]" << " P" << id << " defers reply to P" << req.sender << "\n";
		deferred.push_back(req.sender);
	}
}

void Process::receiveReply(int from) {
	clock++;
	std::cout << "\t" << std::left << std::setw(22) 
			  << "[RECEIVED REPLY]" << " P" << id << " got reply from P" << from
			  << " (clock = " << clock << ")\n";
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

	std::cout << "\t" << std::left << std::setw(22) 
			  << "[DEFERRED REPLIES]" << " P" << id << ": ";
	if (deferred.empty()) {
		std::cout << "none\n";
	}
	else {
		for (int pid : deferred) {
			std::cout << "P" << " ";
			all[pid].receiveReply(id);	
		}
		std::cout << "\n";
	}

	std::cout << "\t" << std::left << std::setw(22) 
			  << "[EXIT CS]" << " P" << id << " (clock = " << clock << ")\n";

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


