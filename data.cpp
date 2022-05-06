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
        if (block == 0) {
            getline(infile, ins);
            coreline.production_times = atoi(ins.c_str());

            getline(infile, ins);
            SplitString(ins, outs, " ");
            for (string p : outs)
                device_process_time.push_back(atoi(p.c_str()));
        }
        else if (block == 1) {
            getline(infile, ins);
            workershop_num = atoi(ins.c_str());

            getline(infile, ins);
            area_num = atoi(ins.c_str());
            for (int i = 0; i < area_num; i++) {
                getline(infile, ins);
                SplitString(ins, outs, " ");
                Area area(i, atoi(outs[0].c_str()), 0, atoi(outs[1].c_str()));
                area_data.push_back(area);
                outs.clear();
            }
        }
        else if (block == 2) {
            getline(infile, ins);
            max_loop_num = atoi(ins.c_str());
            getline(infile, ins);
            first_loop_window_num = atoi(ins.c_str());
        }
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
        else if (block == 4) {
            getline(infile, ins);
            device_num = atoi(ins.c_str());
            linegraph.adjacent_matrix.resize(device_num);
            for (int i = 0; i < device_num; i++) {
                linegraph.adjacent_matrix[i].resize(device_num);
                getline(infile, ins);
                SplitString(ins, outs, " ");
                Device device(i, atoi(outs[0].c_str()), false);
                for (int j = 1; j < 6; j++)
                    device.energy_install_cost.push_back(atoi(outs[j].c_str()));
                device_data.push_back(device);
                outs.clear();
            }
        }
        else if (block == 5) {
            getline(infile, ins);
            linegraph.edge_num = atoi(ins.c_str());
            for (int i = 0; i < linegraph.edge_num; i++) {
                getline(infile, ins);
                SplitString(ins, outs, " ");
                vector<int> oneline;
                for (int j = 0; j < 3; j++)
                    oneline.push_back(atoi(outs[j].c_str()));
                linegraph.graph_data.push_back(oneline);
                outs.clear();
            }
        }
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
    for (int i = 0; i < device_num; i++) {
        Dev_Match dev;
        dev.dev_id = i;
        //核心流水线设备类型与窗口支持预处理的筛选
        for (int w = 0; w < window_num; w++) {
            dev.dev_win.push_back(w);
            if (device_data[i].is_core_device && window_data[w].preprocess_device[device_data[i].type] == false)
                dev.dev_win.pop_back();
        }
        //设备支持能源与区域能源的筛选
        for (int r = 0; r < area_num; r++) {
            if (device_data[i].energy_install_cost[area_data[r].energy_type] != 0)
                dev.dev_area.push_back(r);
        }
        dev_match.push_back(dev);
    }
    return;
}


int main() {
    string in_path = "./case0.in";
    Data data;
    data.Read_file(in_path);
    data.Data_Choose();
    data.linegraph.Tree_Graph();
    cout << data.dev_match.size() << endl;
    return 0;
}






