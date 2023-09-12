#include <iostream>
#include <algorithm>
#include <limits>

#include <boost/property_map/property_map.hpp>

#include "cfg.h"
#include "utils.h"
#include "emm.h"

#define INT_MAX std::numeric_limits<int>::max()
#define INT_MIN int(0)


TENSOR_USAGE_RECORDS_S init_TENSOR_USAGE_RECORDS(DirectedGraqh graph) {
    TENSOR_USAGE_RECORDS_S TURS;
    int numVertex = graph.vertex_set().size() - 1;
    TURS.size = numVertex;
    TURS.TENSOR_USAGE_RECORDS = new TENSOR_USAGE_RECORD_S[TURS.size];

    for (int i = 0; i < numVertex; i++) {
//        DirectedGraqh::vertex_descriptor sourceNode = getVertex(graph, i);
        int first_op_t = i;
        int last_op_t = i;
        DirectedGraqh::vertex_descriptor sourceNode = boost::vertex(i, graph);
        int outDegree = boost::out_degree(sourceNode, graph);

//        std::cout << "sourceNode with Index: " << i << " has inDegree: " << boost::out_degree(sourceNode, graph) << std::endl;

//        DirectedGraqh::vertex_descriptor *targetNodes = new DirectedGraqh::vertex_descriptor[outDegree];
//        int *targetNodesIndex = new int[outDegree];
        std::vector<int> targetNodesVector;
        DirectedGraqh::out_edge_iterator edgeIt, edgeEnd;
        std::tie(edgeIt, edgeEnd) = boost::out_edges(sourceNode, graph);

        int out_i = 0;
        for (; edgeIt != edgeEnd; ++edgeIt) {
            DirectedGraqh::vertex_descriptor targetNode = boost::target(*edgeIt, graph);
            int targetNodeIndex = boost::get(boost::vertex_index, graph, targetNode);
//            targetNodesIndex[out_i] = targetNodeIndex;
            targetNodesVector.push_back(targetNodeIndex);
            if (targetNodeIndex >= last_op_t) {
                last_op_t = targetNodeIndex;
            }

            out_i++;
        }

        boost::property_map<DirectedGraqh, int NodeProperty::*>::type valueMap = boost::get(&NodeProperty::size, graph);
        int tensorSize = valueMap[sourceNode];

        TURS.TENSOR_USAGE_RECORDS[i].tensorUsageInterval.firstOptT = first_op_t;
        TURS.TENSOR_USAGE_RECORDS[i].tensorUsageInterval.lastOptT = last_op_t;
        TURS.TENSOR_USAGE_RECORDS[i].size = tensorSize;
        TURS.TENSOR_USAGE_RECORDS[i].id = i;
//        std::cout << "Tensor size of node with index " << sourceNode << ": "<< tensorSize<<std::endl;
//        std::cout << "Child node of source node with index " << sourceNode << ": ";
//        for(const auto& ele:targetNodesVector){
//            std::cout<< ele<< " ";
//        }
//        std::cout<<std::endl<<std::endl;
    }
    return TURS;
}

OPERATOR_PROFILES_S init_OPERATOR_PROFILES(TENSOR_USAGE_RECORDS_S TURS, DirectedGraqh graph) {
    OPERATOR_PROFILES_S OPS;
    int numVertex = graph.vertex_set().size();
    OPS.size = numVertex;
    OPS.OPERATOR_PROFILES = new OPERATOR_PROFILE_S[OPS.size];

    for (int i = 0; i < numVertex; i++) {
        OPS.OPERATOR_PROFILES[i].id = i;

        DirectedGraqh::vertex_descriptor sourceNode = boost::vertex(i, graph);
        int outDegree = boost::out_degree(sourceNode, graph);

//        std::cout << "sourceNode with Index: " << i << " has outDegree: " <<boost::out_degree(sourceNode, graph) << std::endl;
        if (outDegree + 1 > OPS.OPERATOR_BREADTH_DIM) {
            OPS.OPERATOR_BREADTH_DIM = outDegree + 1;
        }

//        if(i==8){
//            std::cout<<"Current 8";
//        }

//        OPS.OPERATOR_PROFILES[i].TURS.size = 0;
        std::vector<TENSOR_USAGE_RECORD_S> TENSOR_USAGE_RECORD_V;
        for (int j = 0; j <= i; j++) {
            if (TURS.TENSOR_USAGE_RECORDS[j].tensorUsageInterval.firstOptT <=
                TURS.TENSOR_USAGE_RECORDS[i].tensorUsageInterval.firstOptT &&
                TURS.TENSOR_USAGE_RECORDS[j].tensorUsageInterval.lastOptT >=
                TURS.TENSOR_USAGE_RECORDS[i].tensorUsageInterval.firstOptT) {
                TENSOR_USAGE_RECORD_V.push_back(TURS.TENSOR_USAGE_RECORDS[j]);
                OPS.OPERATOR_PROFILES[i].TURS.size++;
            }
        }
        OPS.OPERATOR_PROFILES[i].TURS.TENSOR_USAGE_RECORDS = new TENSOR_USAGE_RECORD_S[OPS.OPERATOR_PROFILES[i].TURS.size];
        for (int j = 0; j < OPS.OPERATOR_PROFILES[i].TURS.size; j++) {
            OPS.OPERATOR_PROFILES[i].TURS.TENSOR_USAGE_RECORDS[j] = TENSOR_USAGE_RECORD_V[j];
            OPS.OPERATOR_PROFILES[i].OPERATOR_BREADTH += OPS.OPERATOR_PROFILES[i].TURS.TENSOR_USAGE_RECORDS[j].size;
        }
    }
    OPS.OPERATOR_BREADTH_VALUES = new int[OPS.OPERATOR_BREADTH_DIM]{0};

    std::vector<std::vector<int>> OPValues;
    for (int i = 0; i < numVertex; i++) {
        int CurrentOPSize = OPS.OPERATOR_PROFILES[i].TURS.size;
        std::vector<int> CurrentOPValues;
        std::cout << "Operator with index " << i << " has OPSize: " << CurrentOPSize << std::endl;
        std::cout<<"Operator Breadth: "<<OPS.OPERATOR_PROFILES[i].OPERATOR_BREADTH << std::endl;
        for (int j = 0; j < CurrentOPSize; j++) {
            CurrentOPValues.push_back(OPS.OPERATOR_PROFILES[i].TURS.TENSOR_USAGE_RECORDS[j].size);
            std::cout << OPS.OPERATOR_PROFILES[i].TURS.TENSOR_USAGE_RECORDS[j].size << " ";
        }
        std::cout << std::endl;
        OPValues.push_back(CurrentOPValues);
    }
    for (int i = 0; i < numVertex; i++) {
        int CurrentOPSize = OPS.OPERATOR_PROFILES[i].TURS.size;
        std::vector<int> currentSortedValues = OPValues[i];
//        memcpy(&currentSortedValues, &OPValues[i], sizeof(std::vector<int>));
        std::sort(currentSortedValues.begin(), currentSortedValues.end(), [](int a, int b) {
            return a > b;
        });
//        std::sort(currentSortedValues.begin(), currentSortedValues.end(), [](int a, int b){
//            return a>b;
//        });
//        std::cout<<"OPSize with index "<<i<<" has OPSize: "<<CurrentOPSize<<std::endl;
        for (int j = 0; j < CurrentOPSize; j++) {
            if (currentSortedValues[j] > OPS.OPERATOR_BREADTH_VALUES[j]) {
                OPS.OPERATOR_BREADTH_VALUES[j] = currentSortedValues[j];
            }
        }
    }
    std::cout << "Operator Profile Dim: " << OPS.OPERATOR_BREADTH_DIM << std::endl;
    for (int i = 0; i < OPS.OPERATOR_BREADTH_DIM; i++) {
        std::cout << "The max value of Operator Profile with Dim " << i << " is " << OPS.OPERATOR_BREADTH_VALUES[i] << std::endl;
        OPS.TheoryBorderline = OPS.TheoryBorderline + OPS.OPERATOR_BREADTH_VALUES[i];
    }
    std::cout << "The Theory Borderline is " << OPS.TheoryBorderline << std::endl;

    return OPS;
}

bool compareTURSPointerByInt(TENSOR_USAGE_RECORD_S *a, TENSOR_USAGE_RECORD_S *b) {
    return (a->size < b->size);
}


void EfficientMemoryManagement::GreedyBySize4OffSetCalculation() {
    this->cleanCache();
    std::cout << "EfficientMemoryManagement: GreedyBySize4OffSetCalculation begins to run!" << std::endl;

    std::sort(this->TURS.TENSOR_USAGE_RECORDS, this->TURS.TENSOR_USAGE_RECORDS + this->TURS.size,
              [](TENSOR_USAGE_RECORD_S a, TENSOR_USAGE_RECORD_S b) {
                  return (a.size > b.size);
              });
    for (int i = 0; i < this->TURS.size; i++) {
        int prevOffset = 0;
        int bestOffset = -1;
        int smallestGap = INT_MAX;
        TENSOR_USAGE_RECORD_S TURt = this->TURS.TENSOR_USAGE_RECORDS[i];
        for (int j = 0; j < this->orderedAllocatedIds.size(); j++) {
            TENSOR_USAGE_RECORD_S TURx = this->orderedAllocatedIds[j];
            int maxFirstOp = std::max(TURt.tensorUsageInterval.firstOptT, TURx.tensorUsageInterval.firstOptT);
            int minLastOp = std::min(TURt.tensorUsageInterval.lastOptT, TURx.tensorUsageInterval.lastOptT);
            if (maxFirstOp <= minLastOp) {
                int gap = this->assignedOffset[TURx.id] - prevOffset;
                if (gap > TURx.size and gap < smallestGap) {
                    smallestGap = gap;
                    bestOffset = prevOffset;
                }
                prevOffset = std::max(prevOffset, assignedOffset[TURx.id] + TURx.size);
            }
        }
        if (bestOffset < 0) {
            bestOffset = prevOffset;
        }
        assignedOffset[TURt.id] = bestOffset;
        this->totalConsumption = std::max(this->totalConsumption, bestOffset + TURt.size);
        this->orderedAllocatedIds.push_back(TURt);
        std::cout<<"After Inserting Tensor Index with "<< TURt.id << ", this Total Consumption is: "<< this->totalConsumption<<std::endl;
    }

    std::cout << "EfficientMemoryManagement: GreedyBySize4OffSetCalculation finished Succesfully!" << std::endl;

//    std::sort(this->TURS.TENSOR_USAGE_RECORDS, this->TURS.TENSOR_USAGE_RECORDS + this->TURS.size,
//              compareTURSPointerByInt);
}


void EfficientMemoryManagement::GreedyByBreadth4OffSetCalculation() {
    this->cleanCache();
    std::cout << "EfficientMemoryManagement: GreedyByBreadth4OffSetCalculation begins to run!" << std::endl;

    std::sort(this->OPS.OPERATOR_PROFILES, this->OPS.OPERATOR_PROFILES + this->OPS.size,
              [](OPERATOR_PROFILE_S a, OPERATOR_PROFILE_S b) {
                  return (a.OPERATOR_BREADTH > b.OPERATOR_BREADTH);
              });
    for (int i = 0; i < this->OPS.size; i++) {
        std::cout<<"Operator Profile with Index: "<<i<<" has "<<this->OPS.OPERATOR_PROFILES[i].TURS.size<<" OPSize"<<std::endl;
        std::sort(this->OPS.OPERATOR_PROFILES[i].TURS.TENSOR_USAGE_RECORDS,
                  this->OPS.OPERATOR_PROFILES[i].TURS.TENSOR_USAGE_RECORDS +
                  this->OPS.OPERATOR_PROFILES[i].TURS.size,
                  [](TENSOR_USAGE_RECORD_S a, TENSOR_USAGE_RECORD_S b) {
                      return (a.size > b.size);
                  });
    }
    for (int i = 0; i < this->OPS.size; i++) {
        for (int j = 0; j < this->OPS.OPERATOR_PROFILES->TURS.size; j++) {
            TENSOR_USAGE_RECORD_S TURt = this->OPS.OPERATOR_PROFILES[i].TURS.TENSOR_USAGE_RECORDS[j];
            if (assignedOffset[TURt.id]>=0){
                continue;
            }
            int prevOffset = 0;
            int bestOffset = -1;
            int smallestGap = INT_MAX;
            for (int k = 0; k < this->orderedAllocatedIds.size(); k++) {
                TENSOR_USAGE_RECORD_S TURx = this->OPS.OPERATOR_PROFILES->TURS.TENSOR_USAGE_RECORDS[k];
                int maxFirstOp = std::max(TURt.tensorUsageInterval.firstOptT, TURx.tensorUsageInterval.firstOptT);
                int minLastOp = std::min(TURt.tensorUsageInterval.lastOptT, TURx.tensorUsageInterval.lastOptT);
                if (maxFirstOp <= minLastOp) {
                    int gap = this->assignedOffset[TURx.id] - prevOffset;
                    if (gap > TURx.size and gap < smallestGap) {
                        smallestGap = gap;
                        bestOffset = prevOffset;
                    }
                    prevOffset = std::max(prevOffset, assignedOffset[TURx.id] + TURx.size);
                }
            }
            if (bestOffset < 0) {
                bestOffset = prevOffset;
            }
            assignedOffset[TURt.id] = bestOffset;
            this->totalConsumption = std::max(this->totalConsumption, bestOffset + TURt.size);
            this->orderedAllocatedIds.push_back(TURt);
            std::cout << "After Inserting Tensor Index with " << TURt.id << ", this Total Consumption is: "
                      << this->totalConsumption << std::endl;
        }
    }
    std::cout << "EfficientMemoryManagement: GreedyByBreadth4OffSetCalculation finished Succesfully!" << std::endl;
}


void EfficientMemoryManagement::printResult() {
    std::cout << "TotalConsumption: " << this->totalConsumption << std::endl;
    std::cout<<"The offsets of the operators are as following: "<<std::endl;
    for(int i = 0; i< this->size; i++){
        std::cout<<"The offset of operator with index: "<<i<<" is "<<assignedOffset[i]<<std::endl;
    }
}

void EfficientMemoryManagement::cleanCache() {
    std::cout <<"Begins to clean cache"<<std::endl;
    this->totalConsumption = 0;
    for(int i = 0; i <this->size; i++){
        assignedOffset[i] = -1;
    }
    this->orderedAllocatedIds.clear();
}