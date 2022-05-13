#include "data.h"

using namespace std;


void SplitString(const string& instr, vector<string>& outstr, const string& split_by)
{
    string::size_type pos1, pos2;
    pos1 = 0;
    pos2 = instr.find(split_by);
    while (pos2 != string::npos)
    {
        outstr.push_back(instr.substr(pos1, pos2 - pos1));
        pos1 = pos2 + split_by.size();
        pos2 = instr.find(split_by, pos1);
    }
    if (pos1 != instr.length())
        outstr.push_back(instr.substr(pos1));
}

void LineGraph::Tree_Graph() {
    int type, from, to;
    for (int i = 0; i < edge_num; i++) {
        type = graph_data[i][0];
        from = graph_data[i][1];
        to = graph_data[i][2];
        if (type == 0)
            adjacent_matrix[from][to] = 1;
        if (type == 1)
            adjacent_matrix[from][to] = 2;
    }
}

void Data::Read_file(string& path)
{
    ifstream infile(path);
    if (!infile.is_open()) cout << "can't open file " << path << endl;
    string ins;
    vector<string> outs;
    int block = 0;
    for (int block = 0; block < 7; block++) {
        outs.clear();
        /*加工迭代次数和能源加工时长*/
        if (block == 0) {
            getline(infile, ins);
            coreline.production_times = atoi(ins.c_str());

            getline(infile, ins);
            SplitString(ins, outs, " ");
            for (string p : outs)
                device_process_time.push_back(atoi(p.c_str()));
        }
        /*区域数据载入*/
        else if (block == 1) {
            getline(infile, ins);
            workershop_num = atoi(ins.c_str());

            getline(infile, ins);
            area_num = atoi(ins.c_str());
            for (int i = 0; i < area_num; i++) {
                getline(infile, ins);
                SplitString(ins, outs, " ");
                Area area(i, atoi(outs[0].c_str()), atoi(outs[1].c_str()));
                area_data.push_back(area);
                outs.clear();
            }
        }
        /*环回*/
        else if (block == 2) {
            getline(infile, ins);
            max_loop_num = atoi(ins.c_str());
            getline(infile, ins);
            first_loop_window_num = atoi(ins.c_str());
        }
        /*窗口数据载入*/
        else if (block == 3) {
            getline(infile, ins);
            window_num = atoi(ins.c_str());
            for (int i = 0; i < window_num; i++) {
                getline(infile, ins);
                SplitString(ins, outs, " ");
                Window wind(i, atoi(outs[0].c_str()), atoi(outs[1].c_str()), atoi(outs[2].c_str()));
                for (int j = 3; j < 6; j++) {
                    wind.preprocess_device.push_back(atoi(outs[j].c_str()));
                }
                window_data.push_back(wind);
                outs.clear();
            }
        }
        /*设备数据载入*/
        else if (block == 4) {
            getline(infile, ins);
            device_num = atoi(ins.c_str());
            linegraph.adjacent_matrix.resize(device_num);
            for (int i = 0; i < device_num; i++) {
                linegraph.adjacent_matrix[i].resize(device_num);
                getline(infile, ins);
                SplitString(ins, outs, " ");
                Device device(i, atoi(outs[0].c_str()), false);
                for (int j = 1; j < 6; j++) {
                    device.energy_install_cost.push_back(atoi(outs[j].c_str()));
                    if (atoi(outs[j].c_str()) != 0)
                        device.surport_energy.insert(j - 1);
                }
                device_data.push_back(device);
                outs.clear();
            }
        }
        /*流水图数据载入及设备节点的输入输出*/
        else if (block == 5) {
            getline(infile, ins);
            linegraph.edge_num = atoi(ins.c_str());
            for (int i = 0; i < linegraph.edge_num; i++) {
                getline(infile, ins);
                SplitString(ins, outs, " ");
                vector<int> oneline;
                for (int j = 0; j < 3; j++)
                    oneline.push_back(atoi(outs[j].c_str()));
                int from = atoi(outs[1].c_str());
                int to = atoi(outs[2].c_str());
                device_data[from].next_device.push_back(&device_data[to]);//插入第一个节点的next_device
                device_data[to].last_device.push_back(&device_data[from]);//插入第二个节点的last_device
                linegraph.graph_data.push_back(oneline);
                outs.clear();
            }
        }
        /*核心流水线和邻接矩阵*/
        else if (block == 6) {
            getline(infile, ins);
            coreline.edge_num = atoi(ins.c_str());
            getline(infile, ins);
            SplitString(ins, outs, " ");
            int core_line = atoi(outs[0].c_str());
            int core_device = linegraph.graph_data[core_line][1];
            coreline.core_devices.push_back(core_device);
            device_data[core_device].is_core_device = true;
            for (int i = 0; i < coreline.edge_num; i++) {
                core_line = atoi(outs[i].c_str());
                coreline.edge_array.push_back(core_line);
                int core_device = linegraph.graph_data[core_line][2];
                coreline.core_devices.push_back(core_device);
                device_data[core_device].is_core_device = true;
            }
        }
    }
    infile.close();
}


void Data::Data_Choose() {

    /*窗口支持能源及区域的筛选*/
    for (auto& window : window_data) {
        for (auto& area : area_data) {
            if (window.workershop_index == area.workershop_index) {
                //这里有可能会出现往set里重复插入相同的能源类型，不过测试插入相同的并不会冲突，而且只会插入一次
                window.support_energy.insert(area.energy_type);
                window.support_area.push_back(area.index);
            }
        }
    }

    /*设备支持的窗口及区域筛选*/
    for (int i = 0; i < device_data.size(); i++) {
        for (int w = 0; w < window_num; w++) {
            //窗口需要支持预加工//如果不是核心设备就不用
            if (window_data[w].preprocess_device[device_data[i].type] || !device_data[i].is_core_device) {
                //看看有没有区域可以支持
                for (int a = 0; a < window_data[w].support_area.size(); a++) {
                    //判断能源能否匹配
                    int area_index = window_data[w].support_area[a];
                    if (device_data[i].surport_energy.find(area_data[area_index].energy_type) != 
                        device_data[i].surport_energy.end())
                        device_data[i].surport_window[w].emplace_back(a); 
                }
            }
        }
    }

    /*寻找头结点*/
    for (int i = 0; i < device_num; i++) {
        if (device_data[i].last_device.size() == 0) {
            linegraph.first_device.push_back(&device_data[i]);
        }
    }

    /*展开回环窗口，生成最长的窗口序列*/
    vector<int> longest;
    for (int out_slp = 0; out_slp < max_loop_num + 1; out_slp++){
        for (int loop = 0; loop < first_loop_window_num; loop++) {
            if (window_data[loop].self_loop) {
                for (int slp = 0; slp < max_loop_num; slp++)
                    longest.push_back(loop);
            }
            longest.push_back(loop);
        }
    }
    for (int others = first_loop_window_num; others < window_num; others++) {
        if (window_data[others].self_loop) {
            for (int slp = 0; slp < max_loop_num; slp++)
                longest.push_back(others);
        }
        longest.push_back(others);
    }
    sqread_circle.insert({ 0, longest });
    return;
}








