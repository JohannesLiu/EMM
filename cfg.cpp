#include <iostream>
#include "cfg.h"
#include "utils.h"

void printGraphInfo(DirectedGraqh graph){
    DirectedGraqh::vertex_iterator  vertexIt, vertexEnd;
    DirectedGraqh::out_edge_iterator  outEdgeIt, outEdgeEnd;



    std::cout << "Nodes: ";
    for(boost::tie(vertexIt, vertexEnd) = boost::vertices(graph); vertexIt!=vertexEnd; ++vertexIt){
        std::cout<<*vertexIt<<" ";
    }

    std::cout<<"Edges: ";
    for(boost::tie(vertexIt, vertexEnd) = boost::vertices(graph); vertexIt!=vertexEnd; ++vertexIt){
        std::cout<<"Out edges of node "<< *vertexIt<< ": ";
        for(boost::tie(outEdgeIt, outEdgeEnd) = boost::out_edges(*vertexIt, graph); outEdgeIt != outEdgeEnd; ++outEdgeIt){
            std::cout<< *outEdgeIt<< " ";
        }
        std::cout<<std::endl;
    }
}

void constructGraph(DirectedGraqh &graph, std::vector<std::string > strVec, char delimiter ){
    int nodeC = std::stoi(strVec[0]);
    for (int i = 1; i <=nodeC; ++i) {
        std::vector<std::string> subStrVec = splitString(strVec[i], delimiter);
        int cost = std::stoi(subStrVec[1]);

        NodeProperty NP = {i, cost};
        boost::add_vertex(NP, graph);
    }
    for (size_t i = 1; i <strVec.size(); ++i){
        std::vector<std::string> subStrVec = splitString(strVec[i], delimiter);
        int inVertex = std::stoi(subStrVec[0]);

        for(size_t j =2; j<subStrVec.size(); ++j){
            int outVertex = std::stoi(subStrVec[j]);
            boost::add_edge(inVertex, outVertex, graph);
        }
    }
//    DirectedGraqh::vertex_descriptor vDesc = boost::vertex(0, graph);
    std::cout<<"Graph Constructed!";
}

DirectedGraqh::vertex_descriptor getVertex(DirectedGraqh graph, int targetNodeIndex){
    DirectedGraqh::vertex_descriptor targetNode = boost::vertex(targetNodeIndex, graph);

    if (targetNode!= boost::graph_traits<DirectedGraqh>::null_vertex()){
        std::cout << "Found target node with index" <<targetNodeIndex << std::endl;
    }else{
        std::cout <<"target node with index " << targetNodeIndex <<" not found"<< std::endl;
    }
    return targetNode;
}

//int getInDegree(DirectedGraqh graph, DirectedGraqh::vertex_descriptor targetNode){
//    return boost::in_degree(targetNode, graph);
//}