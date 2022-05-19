#pragma once
#include <iostream>
#include <set>
#include <vector>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

/*
设备数据结构：设备类型dev_class，设备所属窗口dev_win，设备所属车间dev_workshop，设备所属区域dev_area
*/

class Window {
public:
    int index;  //初始化时读入
    bool self_loop;  //初始化时读入
    int workershop_index;  //初始化时读入
    int cost_coefficient;  //初始化时读入
    set<int> process_time;
    int in_times;
    bool limit_in = false;

    vector<bool> preprocess_device;  //初始化时读入
    set<int> support_energy;  //Data_Choose时读入
    vector<int> support_area;  //Data_Choose时读入
    vector<int> already_installed_device;

    Window(int index, int self_loop, int workershop_index, int cost_coefficient) :
        index(index), self_loop(self_loop), workershop_index(workershop_index), cost_coefficient(cost_coefficient) {}
    ~Window() {}
};

class Area {
public:
    int index;  //初始化时读入
    int workershop_index;  //初始化时读入
    int energy_type;  //初始化时读入

    vector<int> already_installed_device;

    Area(int index, int workershop_index, int energy_type) :
        index(index), workershop_index(workershop_index), energy_type(energy_type) {}
    ~Area() {}
};

class Device {
public:
    int index;  //初始化时读入
    int type;  //初始化时读入
    int installed_area;
    int install_cost;
    bool is_core_device;  //初始化时读入
    vector<long> energy_install_cost; //初始化时读入
    vector<Device*> next_device; //graph初始化时读入
    vector<Device*> last_device; //graph初始化时读入

    set<int> surport_energy; //初始化时读入
    //set<int> surport_window; //Data_Choose时读入
    unordered_map<int, vector<int>> surport_window;//窗口+区域

    Device(int index, int type, bool is_core_device) :
        index(index), type(type), is_core_device(is_core_device) {}
    ~Device() {}
};

class CoreLine {
public:
    int edge_num;//流水线的边数, 初始化时读入
    int production_times; //初始化时读入
    vector<int> edge_array;//流水线的边下标数组,初始化时读入
    vector<int> core_devices; //初始化时读入
    vector<int> not_installed_device;
};

class LineGraph {
public:
    int edge_num;   //初始化时读入
    vector<vector<int>> graph_data;  //初始化时读入
    vector<vector<int>> adjacent_matrix;  //初始化时读入
    void Tree_Graph();
    vector<Device*> first_device;
    vector<Device*> latest_device;
};

class Data {
public:
    vector<Window> window_data;  //初始化时读入
    vector<Area> area_data;  //初始化时读入
    vector<Device> device_data;  //初始化时读入

    void Data_Choose();
    void Read_file();
    void Read_file_cin();
    LineGraph linegraph;
    CoreLine coreline;


public:
    string in_path = "./case0.in";
    int workershop_num;  //初始化时读入
    int max_loop_num;  //初始化时读入
    int first_loop_window_num;  //初始化时读入
    int window_num;  //初始化时读入
    int device_num;  //初始化时读入
    int have_installed_device_num;  //已经安装的设备数量
    int area_num;    //初始化时读入
    vector<int> device_process_time;//仪器使用5种能源的加工时间  初始化时读入
    
    /*每次找到最低代价的匹配路径后就swap到key=0的位置*/
    vector<int> sqread_circle; //Data_Choose时读入
};
