#pragma once
#include "data.h"
#include <map>
#include <queue>
#include <algorithm>

class Result:public Data {
public:
    /*ÿ���ҵ���ʹ��۵�ƥ����swap��key=0��λ��*/
    unordered_map<int, vector<int>> installed_window; //������豸��װ����
    unordered_map<int, vector<int>> installed_area; //������豸��װ����

    void Algorithm(Data& data, int line_num);

private:
    long LONG_MAX = 2147483647;
    long Get_Cost(Data& data);
    void Output(Data& data, int line);
    int Unhook(Data& data, int unhook_type);
    int Area_Match(Data& data, int dev_index, int wind);
    void install_device(Data& data, int device_index, int area_index, int wind, int wind_index);
    bool Check_Match(Data& data, int dev_index, int wind, int wind_index, int line);
    bool Install_Match(Data& data, queue<int>& L1, int start_wind_index, int line);
    bool Install_Match_back(Data& data, int line);
    bool Check_Match_back(Data& data, int dev_index, int wind, int wind_index, int match_index, int& area_index);
    void install_device_back(Data& data, int device_index, int area_index, int wind, int wind_index, int pre_wind_index, int pre_area_index);
};





