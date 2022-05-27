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
    const long longmax = 2147483647;
    vector<long> match_costs;
    vector<int> path_wind;
    vector<vector<int>> optimize_device;
    void back_trace(vector< vector<int>>& data, int nums, int sp);

    long Get_Cost(Data& data);
    void Output(Data& data, int line);
    void Sort_Device(Data& data, int line);
    void State_Clear(Data& data, bool clear_all);
    void Forward(Data& data, int& bestline);
    void Backward(Data& data, int& bestline);

    bool Install_Match(Data& data, int line);
    bool Small_Optim_Match(Data& data, int line, queue<int>& L3, int cur_wind_index);
    bool Check_Match(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index);
    void install_device(Data& data, int device_index, int area_index, int wind, int line, int wind_index);

    long Install_Cost(Data& data, int dev_index, int wind_index, int& area_index);
    bool Install_Match_back(Data& data, int line);
    bool Check_Match_back(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index);
    void install_device_back(Data& data, int device_index, int area_index, int wind, int wind_index, int line,  
        int pre_wind_index = -1, int pre_area_index = -1);

};





