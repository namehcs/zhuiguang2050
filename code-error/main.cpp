#include "data.h"
#include "algor.h"


int main() {
    
    Data data;
    Result result;
    // data.Read_file();
    data.Read_file_cin();
    data.Data_Choose();
    data.linegraph.Tree_Graph();
    result.Algorithm(data);
    return 0;
}