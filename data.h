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
�豸���ݽṹ���豸����dev_class���豸��������dev_win���豸��������dev_workshop���豸��������dev_area
*/
class Dev_Match {
public:
    int dev_class;
    vector<int> dev_win;
    vector<int> dev_area;
};

class Window {
public:
    int index;  //��ʼ��ʱ����
    bool self_loop;  //��ʼ��ʱ����
    int workershop_index;  //��ʼ��ʱ����
    int cost_coefficient;  //��ʼ��ʱ����
    vector<bool> preprocess_device;  //��ʼ��ʱ����
    vector<int> support_device;
    vector<int> already_installed_device;

    Window(int index, int self_loop, int workershop_index, int cost_coefficient) :
        index(index), self_loop(self_loop), workershop_index(workershop_index), cost_coefficient(cost_coefficient) {}
    ~Window() {}
};

class Area {
public:
    int index;  //��ʼ��ʱ����
    int workershop_index;  //��ʼ��ʱ����
    int window_index;
    int energy_type;  //��ʼ��ʱ����
    vector<int> support_device;
    vector<int> already_installed_device;

    Area(int index, int workershop_index, int window_index, int energy_type) :
        index(index), workershop_index(workershop_index), window_index(window_index), energy_type(energy_type) {}
    ~Area() {}
};

class Device {
public:
    int index;  //��ʼ��ʱ����
    int type;  //��ʼ��ʱ����
    bool is_core_device;  //��ʼ��ʱ����
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
    int edge_num;//��ˮ�ߵı���, ��ʼ��ʱ����
    int production_times; //��ʼ��ʱ����
    vector<int> edge_array;//��ˮ�ߵı��±�����,��ʼ��ʱ����
    unordered_set<int> core_devices = { 0, 1, 6, 2, 7, 5, 9 };
    vector<int> not_installed_device;
};

class LineGraph {
public:
    int edge_num;   //��ʼ��ʱ����
    vector<vector<int>> graph_data;  //��ʼ��ʱ����
    vector<vector<int>> adjacent_matrix;
    void Tree_Graph();
    Device* first_device;
};

class Data {
public:
    vector<Window> window_data;  //��ʼ��ʱ����
    vector<Area> area_data;  //��ʼ��ʱ����
    vector<Device> device_data;  //��ʼ��ʱ����
    
    void Data_Choose();
    void Read_file(string& path);
    LineGraph linegraph;
    CoreLine coreline;

public:
    string input_path = "case0.in";
    int workershop_num;  //��ʼ��ʱ����
    int max_loop_num;  //��ʼ��ʱ����
    int first_loop_window_num;  //��ʼ��ʱ����
    int window_num;  //��ʼ��ʱ����
    int device_num;  //��ʼ��ʱ����
    int area_num;    //��ʼ��ʱ����
    unordered_map<int, int> workershop_window_table;//����ʹ��ڵĶ�Ӧ��
    vector<int> device_process_time;//����ʹ��5����Դ�ļӹ�ʱ��  

};

/*input:
����������K
������Դ�ӹ�ʱ�䣺T[5]
����������N ����������R ������Դ���ͣ�Energy[R][2]
���ػ�Ȧ����L
��һ�ֻ��صĴ���������2
*/
//int N = 4, K = 100000, Cl = 2;
int E_Time[5] = { 62, 218, 242, 398, 242 };

const int devices = 13, R = 8, winds = 5, key_nums = 7;



//:0�����ڳ��䣬 1����Դ����
int R_Energy[8][2] = { {0, 0}, {0, 1}, {0, 2}, {1, 1},
                      {1, 3}, {2, 0}, {3, 1}, {3, 4} };

//:0���Ƿ�֧���Ի��أ� 1�����ӵĳ����±꣬ 2�����ڵķ���ϵ���� 3-5�����ڶ� 3 ��������Ԥ�ӹ�֧������ 
int Windows[5][6] = { {0, 0, 1024, 1, 1, 0},
                    {0, 0, 1480, 1, 1, 0},
                    {0, 1, 1600, 0, 0, 1},
                    {1, 2, 2048, 1, 1, 0},
                    {1, 3, 2048, 1, 0, 1} };

//0���������ͣ� 1-5��ʹ�� 5 ����Դ�İ�װ���� 
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


