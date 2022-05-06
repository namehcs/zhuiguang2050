#pragma once
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

//extern vector<Dev_Match> dev_match;
//const extern int devices;
//extern unordered_set<int> Key_devices;

/*
设备数据结构：设备类型dev_class，设备所属窗口dev_win，设备所属车间dev_workshop，设备所属区域dev_area
*/
class Dev_Match {
public:
    int dev_class;
    vector<int> dev_win;
    vector<int> dev_area;
};

class Window {
public:
    int index;  //初始化时读入
    bool self_loop;  //初始化时读入
    int workershop_index;  //初始化时读入
    int cost_coefficient;  //初始化时读入
    vector<bool> preprocess_device;  //初始化时读入
    vector<int> support_device;
    vector<int> already_installed_device;

    Window(int index, int self_loop, int workershop_index, int cost_coefficient) :
        index(index), self_loop(self_loop), workershop_index(workershop_index), cost_coefficient(cost_coefficient) {}
    ~Window() {}
};

class Area {
public:
    int index;  //初始化时读入
    int workershop_index;  //初始化时读入
    int window_index;
    int energy_type;  //初始化时读入
    vector<int> support_device;
    vector<int> already_installed_device;

    Area(int index, int workershop_index, int window_index, int energy_type) :
        index(index), workershop_index(workershop_index), window_index(window_index), energy_type(energy_type) {}
    ~Area() {}
};

class Device {
public:
    int index;  //初始化时读入
    int type;  //初始化时读入
    bool is_core_device;  //初始化时读入
    int distance_from_first;
    int installed_area;
    //int distance_from_last;
    vector<int> energy_install_cost;
    vector<Device*> next_device;
    vector<Device*> last_device;

    Device(int index, int type, bool is_core_device) :
        index(index), type(type), is_core_device(is_core_device) {}
    ~Device() {}
};

class CoreLine {
public:
    int edge_num;//流水线的边数, 初始化时读入
    int production_times; //初始化时读入
    vector<int> edge_array;//流水线的边下标数组,初始化时读入
    unordered_set<int> core_devices = { 0, 1, 6, 2, 7, 5, 9 };
    vector<int> not_installed_device;
};

class LineGraph {
public:
    int edge_num;   //初始化时读入
    vector<vector<int>> graph_data;  //初始化时读入
    vector<vector<int>> adjacent_matrix;
    void Tree_Graph();
    Device* first_device;
};

class Data {
public:
    vector<Window> window_data;  //初始化时读入
    vector<Area> area_data;  //初始化时读入
    vector<Device> device_data;  //初始化时读入
    
    void Data_Choose();
    void Read_file(string& path);
    LineGraph linegraph;
    CoreLine coreline;

public:
    string input_path = "case0.in";
    int workershop_num;  //初始化时读入
    int max_loop_num;  //初始化时读入
    int first_loop_window_num;  //初始化时读入
    int window_num;  //初始化时读入
    int device_num;  //初始化时读入
    int area_num;    //初始化时读入
    unordered_map<int, int> workershop_window_table;//车间和窗口的对应表
    vector<int> device_process_time;//仪器使用5种能源的加工时间  

};

/*input:
生产次数：K
五种能源加工时间：T[5]
车间数量：N 区域数量：R 区域能源类型：Energy[R][2]
最大回环圈数：L
第一种环回的窗口数量：2
*/
//int N = 4, K = 100000, Cl = 2;
int E_Time[5] = { 62, 218, 242, 398, 242 };

const int devices = 13, R = 8, winds = 5, key_nums = 7;



//:0：所在车间， 1：能源类型
int R_Energy[8][2] = { {0, 0}, {0, 1}, {0, 2}, {1, 1},
                      {1, 3}, {2, 0}, {3, 1}, {3, 4} };

//:0：是否支持自环回， 1：连接的车间下标， 2：窗口的费用系数， 3-5：窗口对 3 种仪器的预加工支持能力 
int Windows[5][6] = { {0, 0, 1024, 1, 1, 0},
                    {0, 0, 1480, 1, 1, 0},
                    {0, 1, 1600, 0, 0, 1},
                    {1, 2, 2048, 1, 1, 0},
                    {1, 3, 2048, 1, 0, 1} };

//0：仪器类型， 1-5：使用 5 种能源的安装费用 
int Devices[13][6] = { {0, 262144,   262144, 0,       0,      0},
                      {1, 3014656,  0,      1146880, 0,      0},
                      {0, 67108864, 524288, 0,       0,      0},
                      {2, 0,        0,      0,       851968, 184680448},
                      {0, 262144,   262144, 0,       0,      0},
                      {0, 65536,    65536,  0,       0,      0},
                      {0, 49152,    49152,  0,       0,      0},
                      {0, 67108864, 524288, 0,       0,      0},
                      {2, 0,        0,      0,       983040, 218234880},
                      {2, 0,        0,      0,       983040, 218234880},
                      {0, 524288,   524288, 0,       0,      0},
                      {0, 262144,   262144, 0,       0,      0},
                      {0, 2097152,  524288, 0,       0,      0} };


