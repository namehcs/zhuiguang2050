#pragma once
#include "data.h"
#include <time.h>
#include <map>
#include <queue>
#include <algorithm>

class Result:public Data {
public:
    /*每次找到最低代价的匹配后就swap到key=0的位置*/
    vector<vector<int>> installed_window; //结果：设备安装窗口
    vector<vector<int>> installed_area; //结果：设备安装区域
    void Algorithm(Data& data);

private:
    int result_index = -1;
    bool forward = true;
    const long longmax = 2147483647;
    vector<long> match_costs;
    vector<int> path_wind;
    vector<vector<int>> optimize_device;

    long find_area(Data& data, int dev_index, int wind, int& area_index);
    long Get_Cost(Data& data, int line);
    void Output(Data& data, int line);
    void Forward(Data& data, int& bestline);
    void Backward(Data& data, int& bestline);
    void fff(Data& data, int& bestline);

    bool Install_Match(Data& data, int line);
    bool Install_Match_fff(Data& data);
    bool Small_Optim_Match(Data& data, queue<int> L1, queue<int> L2, vector<int> done_lastnum);
    bool Check_Match(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index);
    void install_device(Data& data, int device_index, int area_index, int wind, int wind_index, int line);
    bool Install_Match_back(Data& data, int line);

};





//for (auto& son_dev : data.device_data[cur_dev].last_device) {
//	if (son_dev->next_device.size() > 1) {
//		if (L3.count(son_dev->index) == 1) {
//			start_optim_wind = installed_window[0][son_dev->index];
//			Small_Optim_Match(data, line, son_dev->index, start_optim_wind);
//		}
//		L3.insert(son_dev->index);
//	}
//}