#include <iostream>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <vector>

class Node {
public:
	std::unordered_map<std::string, std::set<std::string>> pst;
	std::unordered_map<std::string, std::string> rst;

	void addProcessWaiting(const std::string& process, const std::string& resource) {
		pst[process].insert(resource);
	}

	void assignResourceToProcess(const std::string& resource, const std::string& process) {
		rst[resource] = process;
	}
};

struct WaitForGraph {
	std::unordered_map<std::string, std::set<std::string>> graph;

	void addEdge(const std::string& from, const std::string& to) {
		graph[from].insert(to);
	}

	void print() const {
		for (const auto& it : graph) {
			for (const auto& to : it.second) {
				std::cout << it.first << " -> " << to << "\n";
			}
		}
	}

	bool hasCycleUtil(const std::string& node, std::unordered_set<std::string>& visited, std::unordered_set<std::string>& recStack) const {
		visited.insert(node);
		recStack.insert(node);

		for (const auto& neighbor : graph.at(node)) {
			if (recStack.count(neighbor)) return true;
			if (!visited.count(neighbor) && hasCycleUtil(neighbor, visited, recStack)) return true;
		}

		recStack.erase(node);
		return false;
	}

	bool hasCycle() const {
		std::unordered_set<std::string> visited;
		std::unordered_set<std::string> recStack;

		for (const auto& it : graph) {
			if (!visited.count(it.first)) {
				if (hasCycleUtil(it.first, visited, recStack)) return true;
			}
		}

		return false;
	}
};

WaitForGraph buildGlobalWFG(const std::vector<Node>& allNodes) {
	WaitForGraph wfg;

	std::unordered_map<std::string, std::string> globalRST;
	for (const auto& node : allNodes) {
		for (const auto& it : node.rst) {
			globalRST[it.first] = it.second;
		}
	}

	for (const auto& node : allNodes) {
		for (const auto& it : node.pst) {
			for (const auto& res : it.second) {
				auto temp = globalRST.find(res);
				if (temp != globalRST.end() && temp->second != it.first) {
					wfg.addEdge(it.first, temp->second);
				}
			}
		}
	}

	return wfg;
}



std::vector<Node> loadNodeFromFile(const std::string& filename) {
	std::vector<Node> nodes;
	std::ifstream file(filename);
	std::string line;

	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filename << "\n";
		return nodes;
	}

	int numSites = 0;
	std::getline(file, line);
	std::stringstream(line) >> numSites;

	int currentSite = -1;
	Node node;

	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '#') continue;

		std::istringstream iss(line);
		std::string token;
		iss >> token;

		if (token == "SITE") {
			if (currentSite != -1) {
				nodes.push_back(node);
				node = Node();
			}
			iss >> currentSite;
		}
		else if (token == "RESOURCE") {
			std::string res, proc;
			iss >> res >> proc;
			node.assignResourceToProcess(res, proc);
		}
		else if (token == "WAIT") {
			std::string proc, res;
			iss >> proc >> res;
			node.addProcessWaiting(proc, res);
		}
	}

	if (!node.pst.empty() || !node.rst.empty()) {
		nodes.push_back(node);
	}

	if (nodes.size() != static_cast<size_t>(numSites)) {
		std::cerr << "Warning: parsed" << nodes.size()
			<< " site(s), expected " << numSites << "\n";
	}

	return nodes;
}

int main() {
	std::vector<Node> allSites = loadNodeFromFile("input-1.txt");

	WaitForGraph wfg = buildGlobalWFG(allSites);
	wfg.print();

	if (wfg.hasCycle()) {
		std::cout << "\nDeadlock detected!\n";
	}
	else {
		std::cout << "\nNo deadlock detected.\n";
	}

	return 0;
}