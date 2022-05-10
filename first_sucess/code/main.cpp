#include "data.h"
#include "algor.h"


int main(int argc, char *argv[]) {
    Data data;
    Result result;
    data.Read_file();
    data.Data_Choose();
    data.linegraph.Tree_Graph();
    result.Window_Algorithm(data);
    result.Area_Algorithm(data);
    result.Output(data);

	// /*仪器的数量*/
	// cout << data.device_num << endl;

	// /*仪器安装的车间区域下标的数组*/
	// for (int i = 0; i < data.device_num-1; i++) {
	// 	cout << i << " ";
	// }
	// cout << data.device_num-1 << endl;

	// /*流水线的步骤数*/
	// cout << data.coreline.core_devices.size() << endl;

	// /*流水线的窗口下标的数组*/
	// int core, wind;
	// for (int i = 0; i < data.coreline.core_devices.size()-1; i++) {
	// 	cout << i << " ";
	// }
	// cout << data.coreline.core_devices.size()-1 << endl;

    return 0;
}