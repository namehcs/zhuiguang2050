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

void Data::Read_file_cin()
{
    string ins;
    vector<string> outs;
    for (int block = 0; block < 7; block++) {
        outs.clear();
        /*�ӹ�������������Դ�ӹ�ʱ��*/
        if (block == 0) {
            getline(cin, ins);
            coreline.production_times = atoi(ins.c_str());

            getline(cin, ins);
            SplitString(ins, outs, " ");
            for (string p : outs)
                device_process_time.push_back(atoi(p.c_str()));
        }
        /*������������*/
        else if (block == 1) {
            getline(cin, ins);
            workershop_num = atoi(ins.c_str());

            getline(cin, ins);
            area_num = atoi(ins.c_str());
            for (int i = 0; i < area_num; i++) {
                getline(cin, ins);
                SplitString(ins, outs, " ");
                Area area(i, atoi(outs[0].c_str()), atoi(outs[1].c_str()));
                area_data.push_back(area);
                outs.clear();
            }
        }
        /*����*/
        else if (block == 2) {
            getline(cin, ins);
            max_loop_num = atoi(ins.c_str());
            getline(cin, ins);
            first_loop_window_num = atoi(ins.c_str());
        }
        /*������������*/
        else if (block == 3) {
            getline(cin, ins);
            window_num = atoi(ins.c_str());
            for (int i = 0; i < window_num; i++) {
                getline(cin, ins);
                SplitString(ins, outs, " ");
                Window wind(i, atoi(outs[0].c_str()), atoi(outs[1].c_str()), atoi(outs[2].c_str()));
                for (int j = 3; j < 6; j++) {
                    wind.preprocess_device.push_back(atoi(outs[j].c_str()));
                }
                window_data.push_back(wind);
                outs.clear();
            }
        }
        /*�豸��������*/
        else if (block == 4) {
            getline(cin, ins);
            device_num = atoi(ins.c_str());
            linegraph.adjacent_matrix.resize(device_num);
            for (int i = 0; i < device_num; i++) {
                linegraph.adjacent_matrix[i].resize(device_num);
                getline(cin, ins);
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
        /*��ˮͼ�������뼰�豸�ڵ���������*/
        else if (block == 5) {
            getline(cin, ins);
            linegraph.edge_num = atoi(ins.c_str());
            for (int i = 0; i < linegraph.edge_num; i++) {
                getline(cin, ins);
                SplitString(ins, outs, " ");
                vector<int> oneline;
                for (int j = 0; j < 3; j++)
                    oneline.push_back(atoi(outs[j].c_str()));
                int from = atoi(outs[1].c_str());
                int to = atoi(outs[2].c_str());
                device_data[from].next_device.push_back(&device_data[to]);//�����һ���ڵ��next_device
                device_data[to].last_device.push_back(&device_data[from]);//����ڶ����ڵ��last_device
                linegraph.graph_data.push_back(oneline);
                outs.clear();
            }
        }
        /*������ˮ�ߺ��ڽӾ���*/
        else if (block == 6) {
            getline(cin, ins);
            coreline.edge_num = atoi(ins.c_str());
            getline(cin, ins);
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
}


void Data::Read_file()
{
    ifstream infile(in_path);
    if (!infile.is_open()) cout << "can't open file " << in_path << endl;
    string ins;
    vector<string> outs;
    int block = 0;
    for (int block = 0; block < 7; block++) {
        outs.clear();
        /*�ӹ�������������Դ�ӹ�ʱ��*/
        if (block == 0) {
            getline(infile, ins);
            coreline.production_times = atoi(ins.c_str());

            getline(infile, ins);
            SplitString(ins, outs, " ");
            for (string p : outs)
                device_process_time.push_back(atoi(p.c_str()));
        }
        /*������������*/
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
        /*����*/
        else if (block == 2) {
            getline(infile, ins);
            max_loop_num = atoi(ins.c_str());
            getline(infile, ins);
            first_loop_window_num = atoi(ins.c_str());
        }
        /*������������*/
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
        /*�豸��������*/
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
        /*��ˮͼ�������뼰�豸�ڵ���������*/
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
                device_data[from].next_device.push_back(&device_data[to]);//�����һ���ڵ��next_device
                device_data[to].last_device.push_back(&device_data[from]);//����ڶ����ڵ��last_device
                linegraph.graph_data.push_back(oneline);
                outs.clear();
            }
        }
        /*������ˮ�ߺ��ڽӾ���*/
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

    /*����֧����Դ�������ɸѡ*/
    for (auto& window : window_data) {
        for (auto& area : area_data) {
            if (window.workershop_index == area.workershop_index) {
                //�����п��ܻ������set���ظ�������ͬ����Դ���ͣ��������Բ�����ͬ�Ĳ������ͻ������ֻ�����һ��
                window.support_energy.insert(area.energy_type);
                window.support_area.push_back(area.index);
            }
        }
    }

    /*�豸֧�ֵĴ��ڼ�����ɸѡ*/
    for (int i = 0; i < device_data.size(); i++) {
        for (int w = 0; w < window_num; w++) {
            //������Ҫ֧��Ԥ�ӹ�//������Ǻ����豸�Ͳ���
            if (window_data[w].preprocess_device[device_data[i].type] || !device_data[i].is_core_device) {
                //������û���������֧��
                for (auto& area : window_data[w].support_area) {
                    //�ж���Դ�ܷ�ƥ��
                    if (device_data[i].surport_energy.find(area_data[area].energy_type) !=
                        device_data[i].surport_energy.end())
                        device_data[i].surport_window[w].emplace_back(area);
                }
            }
        }
    }
    /*���Ľڵ����ӹ�ϵ����*/
    for (int i = 0; i < coreline.core_devices.size(); i++) {
        if (i > 0)
            device_data[coreline.core_devices[i]].last_coredev = &device_data[coreline.core_devices[i - 1]];
        if (i < coreline.edge_num)
            device_data[coreline.core_devices[i]].next_coredev = &device_data[coreline.core_devices[i + 1]];
    }

    /*Ѱ��ͷβ���*/
    for (int i = 0; i < device_num; i++) {
        if (device_data[i].last_device.size() == 0)
            linegraph.first_device.push_back(&device_data[i]);
        if (device_data[i].next_device.size() == 0)
            linegraph.latest_device.push_back(&device_data[i]);
    }

    /*չ���ػ����ڣ�������Ĵ�������*/
    for (int out_slp = 0; out_slp < max_loop_num + 1; out_slp++){
        for (int loop = 0; loop < first_loop_window_num; loop++) {
            if (window_data[loop].self_loop) {
                for (int slp = 0; slp < max_loop_num; slp++)
                    sqread_circle.push_back(loop);
            }
            sqread_circle.push_back(loop);
        }
    }
    for (int others = first_loop_window_num; others < window_num; others++) {
        if (window_data[others].self_loop) {
            for (int slp = 0; slp < max_loop_num; slp++)
                sqread_circle.push_back(others);
        }
        sqread_circle.push_back(others);
    }
    return;
}







