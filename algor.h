#pragma once
#include "data.h"
#include <queue>
#include <algorithm>

class Result:public Data {
public:
    vector<vector<int>> installed_window; //结果：设备安装窗口
    vector<vector<int>> installed_area; //结果：设备安装区域

    void Algorithm(Data& data, int line_num);
    bool Install_Match(Data& data, int line, int res);
    int Area_Match(Data& data, int dev_index, int wind);
    void install_device(Data& data, int device_index, int area_index, int wind_index);
    void Output(Data& data);
private:
    //queue<int> Choose_Window(Data& data, int dev_indev, int cur_wind_index);
    bool Check_Match(Data& data, int dev_index, int wind, int wind_index, int match_index);

};





