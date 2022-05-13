#pragma once
#include "data.h"
#include <map>
#include <queue>
#include <algorithm>

class Result:public Data {
public:
    /*每次找到最低代价的匹配后就swap到key=0的位置*/
    unordered_map<int, vector<int>> installed_window; //结果：设备安装窗口
    unordered_map<int, vector<int>> installed_area; //结果：设备安装区域

    void Algorithm(Data& data, int line_num);

private:
    long Get_Cost(Data& data);
    void Output(Data& data, int line);
    int Unhook(Data& data, int unhook_type);
    int Area_Match(Data& data, int dev_index, int wind);
    void install_device(Data& data, int device_index, int area_index, int wind, int wind_index);
    bool Check_Match(Data& data, int dev_index, int wind, int wind_index, int line);
    bool Install_Match(Data& data, queue<int>& L1, int start_wind_index, int line);
};





