#pragma once
#include <vector>
#include <unordered_map>

extern vector<Dev_Match> dev_match;
const extern int devices;

const int devices = 13, R = 8, winds = 5;
//int N = 4, K = 100000, Cl = 2;
int E_Time[5] = { 62, 218, 242, 398, 242 };

class OtherIn {
public:
    int workershop_num;
    int max_loop_num;
    int first_loop_window_num;
    int window_num;
    int device_num;
    unordered_map<int, int> workershop_window_table;//车间和窗口的对应表
    vector<int> device_process_time;//仪器使用5种能源的加工时间
    

};

class Window {
public:
    int index;
    bool self_loop;
    int workershop_index;
    int cost_coefficient;
    vector<bool> preprocess_device;
    vector<int> support_device;
    vector<int> already_installed_device;
};

class Area {
public:
    int index;
    int workershop_index;
    int window_index;
    int energy_type;
    vector<int> support_device;
    vector<int> already_installed_device;
};

class CoreLine {
public:
    int edge_num;//流水线的边数
    int production_times;
    vector<int> edge_array;//流水线的边下标数组
    vector<int> core_devices;
    vector<int> not_installed_device;
};

class Device {
public:
    int index;
    int type;
    bool is_core_device;
    int distance_from_first;
    int installed_area;
    //int distance_from_last;
    vector<int> energy_install_cost;
    vector<Device*> next_device;
    vector<Device*> last_device;
};

class LineGraph {
public:
    int edge_num;
    vector<vector<int>> adjacent_matrix;
    Device* first_device;
};



/*
设备数据结构：设备类型dev_class，设备所属窗口dev_win，设备所属车间dev_workshop，设备所属区域dev_area
*/
class Dev_Match {
public:
    int dev_class;
    std::vector<int> dev_win;
    std::vector<int> dev_workshop;
    std::vector<int> dev_area;
};


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






