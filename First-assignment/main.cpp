#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

// Loads graph from file (file format similar to graph-x.txt)
static std::vector<std::vector<int>> loadGraph(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    int n;
    file >> n;
    std::vector<std::vector<int>> graph(n, std::vector<int>(n));
    
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (!(file >> graph[i][j])) {
                throw std::runtime_error("Invalid file format");
            }
        }
    }

    return graph;
}

// Recursive DFS function
void dfs(const std::vector<std::vector<int>>& graph, int node, std::vector<bool>& visited) {
    visited[node] = true;
    for (size_t i = 0; i < graph.size(); ++i) {
        if (graph[node][i] == 1 && !visited[i]) {
            dfs(graph, i, visited);
        }
    }
}

// Checks if node can be initializator
bool isGoodFirstNode(const std::vector<std::vector<int>>& graph, int firstNode) {
    int n = graph.size();
    std::vector<bool> nodesCovered(n, false);

    dfs(graph, firstNode, nodesCovered);

    for (bool it : nodesCovered) {
        if (it == false)
            return false;
    }
    return true;
}

int main()
{
    try {
        std::string filename;

        std::cout << "Enter the graph filename (must be in the same directory): ";
        std::cin >> filename;

        std::vector<std::vector<int>> graph = loadGraph(filename);

        /*
        * This section allows user to select single starting node. It's commented because now loop goes through every node and checks it.
        *
        * int selectedNode;
        *
        * std::cout << "Type number of node: ";
        * std::cin >> selectedNode;
        * if (selectedNode >= graph.size() || selectedNode < 0) { std::cerr << "Wrong node selected. Exiting."; return -1; }
        *
        * if (isGoodFirstNode(graph, selectedNode))
        *        std::cout << "Node " << selectedNode << " is good" << std::endl;
        *   else
        *       std::cout << "Node " << selectedNode << " is bad" << std::endl;
        */

        for (size_t i = 0; i < graph.size(); ++i) {
            if (isGoodFirstNode(graph, i))
                std::cout << "Node " << i << " can reach all other nodes" << std::endl;
            else
                std::cout << "Node " << i << " cannot reach all other nodes" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}