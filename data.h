#pragma once
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

/*
�豸���ݽṹ���豸����dev_class���豸��������dev_win���豸��������dev_workshop���豸��������dev_area
*/
class Dev_Match {
public:
    int dev_id;
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
    vector<int> core_devices;
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
    vector<Dev_Match> dev_match;

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
