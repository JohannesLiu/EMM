#include <iostream>
#include <string>
#include <vector>
#include <boost/graph/adjacency_list.hpp>

#include "cfg.h"
#include "emm.h"
#include "utils.h"

#define DELIMITER ' '

int main(){
    std::vector<std::string > graphInfoVec = readGraphData("/home/xkliu/CLionProjects/EMM/data/example1.txt");
    DirectedGraqh graph;
    constructGraph(graph, graphInfoVec, DELIMITER);
    printGraphInfo(graph);

    TENSOR_USAGE_RECORDS_S TURS = init_TENSOR_USAGE_RECORDS(graph);
    OPERATOR_PROFILES_S OPS = init_OPERATOR_PROFILES(TURS, graph);

    EfficientMemoryManagement GBS4SC(TURS, OPS);
    GBS4SC.GreedyBySize4OffSetCalculation();
    GBS4SC.printResult();
    GBS4SC.GreedyByBreadth4OffSetCalculation();
    GBS4SC.printResult();

    std::cout << "Finished!" << std::endl;
    return 0;
}
