#pragma once
#include "data.h"
#include <map>
#include <queue>
#include <algorithm>

class Result:public Data {
public:
    /*ÿ���ҵ���ʹ��۵�ƥ����swap��key=0��λ��*/
    vector<vector<int>> installed_window; //������豸��װ����
    vector<vector<int>> installed_area; //������豸��װ����

    void Algorithm(Data& data);

private:
    long longmax = 2147483647;
    vector<long> match_costs;
    long Get_Cost(Data& data);
    void Output(Data& data, int line);
    void Unhook(Data& data, int unhook_type, int& best_line);
    bool Install_Match(Data& data, int line);
    bool Check_Match(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index);
    void install_device(Data& data, int device_index, int area_index, int wind, int line, int wind_index);

    bool Install_Match_back(Data& data, int line);
    bool Check_Match_back(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index);
    void install_device_back(Data& data, int device_index, int area_index, int wind, int wind_index, int line, int pre_wind_index = -1, int pre_area_index = -1);

};





