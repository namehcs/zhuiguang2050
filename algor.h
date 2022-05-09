#pragma once
#include "data.h"
#include <queue>

class Result:public Data {
public:
    vector<vector<int>> installed_window; //������豸��װ����
    vector<vector<int>> installed_area; //������豸��װ����
    void Algorithm(Data& data);
private:
    queue<int> Choose_Window(Data& data, int dev_indev, int cur_wind_index);
    bool Check_Match(Data& data, int dev_index, int wind, int wind_index, int match_index);

};





