#include <boost/graph/adjacency_list.hpp>


typedef struct NodeProperty_S{
    int id;
    int size;
}NodeProperty;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, NodeProperty> DirectedGraqh;
void printGraphInfo(DirectedGraqh graph);
void constructGraph(DirectedGraqh &graph, std::vector<std::string > strVec, char delimiter );
DirectedGraqh::vertex_descriptor getVertex(DirectedGraqh graph, int targetNodeIndex);