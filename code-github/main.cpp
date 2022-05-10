#include "data.h"
#include "algor.h"


int main() {
    Data data;
    Result result;
    data.Read_file();
    data.Data_Choose();
    data.linegraph.Tree_Graph();
    result.Window_Algorithm(data);
    result.Area_Algorithm(data);
    result.Output(data);
    return 0;
}