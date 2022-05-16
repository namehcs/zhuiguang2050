#include "data.h"
#include "algor.h"


int main() {
    string in_path = "./case0.in";
    Data data;
    Result result;
    data.Read_file();
    data.Data_Choose();
    data.linegraph.Tree_Graph();
    //cout << data.device_data.size() << endl;
    result.Algorithm(data, 0);
    return 0;
}