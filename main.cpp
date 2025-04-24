#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_map>

bool isGoodFirstNode(const std::vector<std::vector<int>>& graph, int firstNode) {
    int s = std::count(graph[firstNode].begin(), graph[firstNode].end(), 1);
    if(s == graph.size() - 1)
        return true;
    
    std::unordered_map<int, int> nodesCovered;
    for(size_t i=0; i<graph.size(); i++) {
        if(graph[firstNode][i] == 1) {
            nodesCovered[i] = 1;
            for(size_t j=0; j<graph.size(); j++) {
                if(j == firstNode || j == i)
                    continue;
                else if(nodesCovered[j] == 1)
                    continue;
                else if(graph[i][j] == 1)
                    nodesCovered[j] = 1;
            }
        }
        else if(i == firstNode)
            nodesCovered[i] = 1;
    }
    
    for(size_t i =0; i<graph.size(); i++)
        std::cout << nodesCovered[i] << std::endl;
    return false;
}

int main()
{
    int selectedNode;
    
    std::vector<std::vector<int>> graph = {
        {0, 0, 1, 1, 1},
        {0, 0, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 0, 0, 0},
        {1, 1, 0, 0, 0}
    };
    
    std::cout << "Type number of node: ";
    std::cin >> selectedNode;
    if(selectedNode >= graph.size() || selectedNode < 0) std::cerr << "Wrong node";
    isGoodFirstNode(graph, selectedNode);
    
    return 0;
}
