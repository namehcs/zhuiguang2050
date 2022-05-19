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
�豸���ݽṹ���豸����dev_class���豸��������dev_win���豸��������dev_workshop���豸��������dev_area
*/

class Window {
public:
    int index;  //��ʼ��ʱ����
    bool self_loop;  //��ʼ��ʱ����
    int workershop_index;  //��ʼ��ʱ����
    int cost_coefficient;  //��ʼ��ʱ����
    set<int> process_time;
    int in_times;
    bool limit_in = false;

    vector<bool> preprocess_device;  //��ʼ��ʱ����
    set<int> support_energy;  //Data_Chooseʱ����
    vector<int> support_area;  //Data_Chooseʱ����
    vector<int> already_installed_device;

    Window(int index, int self_loop, int workershop_index, int cost_coefficient) :
        index(index), self_loop(self_loop), workershop_index(workershop_index), cost_coefficient(cost_coefficient) {}
    ~Window() {}
};

class Area {
public:
    int index;  //��ʼ��ʱ����
    int workershop_index;  //��ʼ��ʱ����
    int energy_type;  //��ʼ��ʱ����

    vector<int> already_installed_device;

    Area(int index, int workershop_index, int energy_type) :
        index(index), workershop_index(workershop_index), energy_type(energy_type) {}
    ~Area() {}
};

class Device {
public:
    int index;  //��ʼ��ʱ����
    int type;  //��ʼ��ʱ����
    int installed_area;
    int install_cost;
    bool is_core_device;  //��ʼ��ʱ����
    vector<long> energy_install_cost; //��ʼ��ʱ����
    vector<Device*> next_device; //graph��ʼ��ʱ����
    vector<Device*> last_device; //graph��ʼ��ʱ����

    set<int> surport_energy; //��ʼ��ʱ����
    //set<int> surport_window; //Data_Chooseʱ����
    unordered_map<int, vector<int>> surport_window;//����+����

    Device(int index, int type, bool is_core_device) :
        index(index), type(type), is_core_device(is_core_device) {}
    ~Device() {}
};

class CoreLine {
public:
    int edge_num;//��ˮ�ߵı���, ��ʼ��ʱ����
    int production_times; //��ʼ��ʱ����
    vector<int> edge_array;//��ˮ�ߵı��±�����,��ʼ��ʱ����
    vector<int> core_devices; //��ʼ��ʱ����
    vector<int> not_installed_device;
};

class LineGraph {
public:
    int edge_num;   //��ʼ��ʱ����
    vector<vector<int>> graph_data;  //��ʼ��ʱ����
    vector<vector<int>> adjacent_matrix;  //��ʼ��ʱ����
    void Tree_Graph();
    vector<Device*> first_device;
    vector<Device*> latest_device;
};

class Data {
public:
    vector<Window> window_data;  //��ʼ��ʱ����
    vector<Area> area_data;  //��ʼ��ʱ����
    vector<Device> device_data;  //��ʼ��ʱ����

    void Data_Choose();
    void Read_file();
    void Read_file_cin();
    LineGraph linegraph;
    CoreLine coreline;


public:
    string in_path = "./case0.in";
    int workershop_num;  //��ʼ��ʱ����
    int max_loop_num;  //��ʼ��ʱ����
    int first_loop_window_num;  //��ʼ��ʱ����
    int window_num;  //��ʼ��ʱ����
    int device_num;  //��ʼ��ʱ����
    int have_installed_device_num;  //�Ѿ���װ���豸����
    int area_num;    //��ʼ��ʱ����
    vector<int> device_process_time;//����ʹ��5����Դ�ļӹ�ʱ��  ��ʼ��ʱ����
    
    /*ÿ���ҵ���ʹ��۵�ƥ��·�����swap��key=0��λ��*/
    vector<int> sqread_circle; //Data_Chooseʱ����
};
