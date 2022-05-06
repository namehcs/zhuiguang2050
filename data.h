#pragma once
#include <iostream>
#include <unordered_set>
#include <vector>
#include <unordered_map>

//extern vector<Dev_Match> dev_match;
//const extern int devices;

class OtherIn {
public:
    int workershop_num;
    int max_loop_num;
    int first_loop_window_num;
    int window_num;
    int device_num;
    unordered_map<int, int> workershop_window_table;//����ʹ��ڵĶ�Ӧ��
    vector<int> device_process_time;//����ʹ��5����Դ�ļӹ�ʱ��
    

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
    int edge_num;//��ˮ�ߵı���
    int production_times;
    vector<int> edge_array;//��ˮ�ߵı��±�����
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
�豸���ݽṹ���豸����dev_class���豸��������dev_win���豸��������dev_workshop���豸��������dev_area
*/
class Dev_Match {
public:
    int dev_class;
    std::vector<int> dev_win;
    //std::vector<int> dev_workshop;
    std::vector<int> dev_area;
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






