#include <iostream>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <vector>

// ------------------------
// Example Input File Format:
// ------------------------
// 3                       ← Number of sites
//
// SITE 1
// RESOURCE R1 P1         ← Resource R1 is held by process P1
// WAIT P2 R1             ← Process P2 is waiting for resource R1
//
// SITE 2
// RESOURCE R2 P2
// WAIT P1 R2
//
// SITE 3
// WAIT P3 R1
// WAIT P3 R2
// ------------------------

// Each Node represents a single site, holding its local PST and RST
class Node {
public:
	// Process Status Table: process → set of resources it's waiting for
	std::unordered_map<std::string, std::set<std::string>> pst;

	// Resource Status Table: resource → process currently holding it
	std::unordered_map<std::string, std::string> rst;

	// Add a wait dependency: process waits for a specific resource
	void addProcessWaiting(const std::string& process, const std::string& resource) {
		pst[process].insert(resource);
	}

	// Assign a resource to a process (lock)
	void assignResourceToProcess(const std::string& resource, const std::string& process) {
		rst[resource] = process;
	}
};

// Structure to hold and operate on the Wait-For Graph
struct WaitForGraph {
	std::unordered_map<std::string, std::set<std::string>> graph;

	// Add a directed edge: "from" process waits for "to" process
	void addEdge(const std::string& from, const std::string& to) {
		graph[from].insert(to);
	}

	// Print all wait-for edges
	void print() const {
		for (const auto& it : graph) {
			for (const auto& to : it.second) {
				std::cout << it.first << " -> " << to << "\n";
			}
		}
	}

	// Recursive DFS utility to detect cycles
	bool hasCycleUtil(const std::string& node, std::unordered_set<std::string>& visited, std::unordered_set<std::string>& recStack) const {
		visited.insert(node);
		recStack.insert(node);

		auto it = graph.find(node);
		if (it != graph.end()) {
			for (const auto& neighbor : it->second) {
				if (recStack.count(neighbor)) return true;
				if (!visited.count(neighbor) && hasCycleUtil(neighbor, visited, recStack)) return true;
			}
		}	

		recStack.erase(node);
		return false;
	}

	// Public method to check if a cycle exists in the graph
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

// Builds the global Wait-For Graph by analyzing PST and RST of all sites
WaitForGraph buildGlobalWFG(const std::vector<Node>& allNodes) {
	WaitForGraph wfg;

	// Merge all local RSTs into a global resource map
	std::unordered_map<std::string, std::string> globalRST;
	for (const auto& node : allNodes) {
		for (const auto& it : node.rst) {
			globalRST[it.first] = it.second;
		}
	}

	// For each local PST entry, find who holds the needed resource
	// and add an edge: process → holder
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

// Parses a single input file with data for multiple sites
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
				nodes.push_back(node);	// store previous node
				node = Node();			// start a new one
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

	// Add the last parsed site
	if (!node.pst.empty() || !node.rst.empty()) {
		nodes.push_back(node);
	}

	// Warn if declared number of sites doesn't match actual
	if (nodes.size() != static_cast<size_t>(numSites)) {
		std::cerr << "Warning: parsed" << nodes.size()
			<< " site(s), expected " << numSites << "\n";
	}

	return nodes;
}

// Entry point
int main() {
	std::string filename;
	std::cout << "Enter file name (must be in the same folder): ";
	std::cin >> filename;

	std::vector<Node> allSites = loadNodeFromFile(filename);

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