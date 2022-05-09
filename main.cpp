#include "data.h"
#include "algor.h"


int main() {
    string in_path = "./case0.in";
    Data data;
    Result result;
    data.Read_file(in_path);
    data.Data_Choose();
    data.linegraph.Tree_Graph();
    result.Window_Algorithm(data);
    result.Area_Algorithm(data);
    result.Output(data);
    return 0;
}