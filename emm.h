#include <vector>
#include <unordered_map>

struct TENSOR_USAGE_INTERVAL_S{
    int firstOptT;
    int lastOptT;
};

struct TENSOR_USAGE_RECORD_S{
    int id = -1;
    TENSOR_USAGE_INTERVAL_S tensorUsageInterval;
    int size;
};

//TENSOR_USAGE_RECORD_S* TENSOR_USAGE_RECORDS;

struct TENSOR_USAGE_RECORDS_S{
    int size = 0;
    TENSOR_USAGE_RECORD_S* TENSOR_USAGE_RECORDS;
};

struct OPERATOR_PROFILE_S{
    int id = -1;
    TENSOR_USAGE_RECORDS_S TURS;
    int OPERATOR_BREADTH = 0;
//    TENSOR_USAGE_RECORDS_S TENSOR_USAGE_RECORDS;
};

struct OPERATOR_PROFILES_S{
    int size=0;
    OPERATOR_PROFILE_S *OPERATOR_PROFILES;
    int *OPERATOR_BREADTH_VALUES;
    int TheoryBorderline = 0;
    int OPERATOR_BREADTH_DIM = -1;
};


TENSOR_USAGE_RECORDS_S init_TENSOR_USAGE_RECORDS(DirectedGraqh graph);
OPERATOR_PROFILES_S init_OPERATOR_PROFILES(TENSOR_USAGE_RECORDS_S TURS, DirectedGraqh graph);

class EfficientMemoryManagement{
public:
    void GreedyBySize4OffSetCalculation();
    void GreedyByBreadth4OffSetCalculation();

    void cleanCache();
    void printResult();

    EfficientMemoryManagement(TENSOR_USAGE_RECORDS_S TURS, OPERATOR_PROFILES_S OPS): TURS(TURS), OPS(OPS){
        this->size = TURS.size;
        for(int i = 0; i <this->size; i++){
            assignedOffset[i] = -1;
        }
    };
    ~EfficientMemoryManagement(){};

private:
    int size;
    TENSOR_USAGE_RECORDS_S TURS;
    OPERATOR_PROFILES_S OPS;
    int totalConsumption = 0;
    std::vector<TENSOR_USAGE_RECORD_S> orderedAllocatedIds;
    std::unordered_map<int, int> assignedOffset; // id -> offset
};