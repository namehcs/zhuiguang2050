#pragma once
#include "data.h"
#include <time.h>
#include <map>
#include <queue>
#include <algorithm>

class Result:public Data {
public:
    /*ÿ���ҵ���ʹ��۵�ƥ����swap��key=0��λ��*/
    vector<vector<int>> installed_window; //������豸��װ����
    vector<vector<int>> installed_area; //������豸��װ����
    queue<int> L1, L2;   //�Ǻ����豸���õ���
    void Algorithm(Data& data);

private:
    int result_index = -1;
    int stage = 0;
    const long longmax = 2147483647;
    int core_start_device;
    int core_start_window_index;
    vector<long> match_costs;
    vector<int> path_wind;
    vector<vector<int>> optimize_device;
    void back_trace(vector< vector<int>>& data, int nums, int sp);

    long Get_Cost(Data& data);
    void Sort_Device(Data& data, int line);
    void Output(Data& data, int line);
    void State_Clear(Data& data, bool clear_all);
    bool Install_Match(Data& data, int line, int cur_dev, int cur_wind_index, bool core_mode);
    bool Forward(Data& data, int& bestline, bool core_mode);
    void Backward(Data& data, int& bestline);
    bool Check_Match(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index, bool core_mode);
    void install_device(Data& data, int device_index, int area_index, int wind, int line, int wind_index, bool core_mode);
    bool Install_Match_back(Data& data, int line);
    bool Check_Match_back(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index);
    void install_device_back(Data& data, int device_index, int area_index, int wind, int wind_index, int line,  
        int pre_wind_index = -1, int pre_area_index = -1);

};




