#pragma once
#include "data.h"
#include <queue>
#include <algorithm>

class Result:public Data {
public:
    vector<vector<int>> installed_window; //结果：设备安装窗口
    vector<vector<int>> installed_area; //结果：设备安装区域
    void Window_Algorithm(Data& data);
    void Area_Algorithm(Data& data);
    void Output(Data& data);
private:
    //long LONG_MAX = 2147483647;
    queue<int> Choose_Window(Data& data, int dev_indev, int cur_wind_index);
    bool Check_Match(Data& data, int dev_index, int wind, int wind_index, int match_index);

};

