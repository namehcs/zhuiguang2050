#include "algor.h"
//#include <windows.h>


void Result::State_Clear(Data& data, bool clear_all) {
	if (clear_all) {
		for (auto& wind : data.window_data) {
			wind.already_installed_device.clear();
			wind.process_time.clear();
			wind.forward_intimes = 0;
			wind.in_times = 0;
			wind.limit_in = false;
		}
		for (auto& area : data.area_data)
			area.already_installed_device.clear();
		for (auto& device : data.device_data) {
			device.install_cost = 0;
		}
		data.have_installed_device_num = 0;
	}
	else {
		for (auto& wind : data.window_data) {
			wind.process_time.clear();
			wind.forward_intimes = wind.in_times;
			wind.in_times = 0;
		}
	}
}


void Result::Sort_Device(Data& data, int line) {
	/*�Ƚ�ÿ�������豸�Ż����۵Ĵ�С���Ӵ�С����*/
	vector<int> one;
	for (auto& dev : data.coreline.core_devices) {
		if (data.device_data[dev].optim_cost > 0) {
			one.push_back(data.device_data[dev].index);
			one.push_back(data.device_data[dev].optim_cost);
			one.push_back(installed_window[line][dev]);
			optimize_device.push_back(one);
			one.clear();
		}
	}

	sort(optimize_device.begin(), optimize_device.end(), [](vector<int>& x, vector<int>& y) { return x[1] > y[1]; });
}

void Result::Backward(Data& data, int& bestline) {
	long backward_cost;
	bool backward_valid;
	/*���ݴӺ���ǰ�������Ĵ���ƥ�䣬�����һ����ʼ��ƥ��*/
	installed_window.push_back(installed_window[result_index]);
	installed_area.push_back(installed_area[result_index]);
	result_index++;
	State_Clear(data, false);
	backward_valid = Install_Match_back(data, result_index);
	if (backward_valid) {
		backward_cost = Get_Cost(data);
		match_costs.push_back(backward_cost);
	}
	/*������ݴ���С����ʹ��ۣ����������ƥ��*/
	if (backward_valid && backward_cost < match_costs[bestline]) {
		bestline = result_index;
	}
	else if (!backward_valid || backward_cost >= match_costs[bestline]) {
		result_index--;
		installed_window.pop_back();
		installed_area.pop_back();
		match_costs.pop_back();
	}
}


bool Result::Forward(Data& data, int& bestline, bool core_mode) {
	bool result = true;
	/*����ǰ��ƥ�䣬ע��������Ҫ�����зŽ�data.sqread_circle���ܽ��к�������*/
	/*�����һ����ʼ��ƥ��*/
	long forward_cost, backward_cost;
	bool forward_valid, backward_valid;
	if (result_index == -1) {
		installed_window.push_back(vector<int>(data.device_num, 999));
		installed_area.push_back(vector<int>(data.device_num, 999));
	}
	else if (result_index >= 0) {
		installed_window.push_back(installed_window[result_index]);
		installed_area.push_back(installed_area[result_index]);
	}

	result_index++;
	forward_valid = Install_Match(data, result_index, core_start_device, core_start_window_index, core_mode);
	if (forward_valid) {
		Sort_Device(data, result_index);
		forward_cost = Get_Cost(data);
		match_costs.push_back(forward_cost);
		if (forward_cost < match_costs[bestline])
			bestline = result_index;
		//Backward(data, bestline);
	}
	if (!forward_valid) {
		result_index--;
		result = false;
		installed_window.pop_back();
		installed_area.pop_back();
	}
	State_Clear(data, true);
	return result;
}

/**********************************************************************************************
�������Ż��㷨
**********************************************************************************************/
void Result::Algorithm(Data& data) {
	int bestline = 0;
	long start_time, end_time, process_time;
	bool forward_result, core_mode = false;
	start_time = clock();
	core_start_window_index = 0;
	for (auto& dev : data.linegraph.first_device) {
		if (dev->is_core_device) {
			core_mode = true;
			core_start_device = dev->index;
			stage = 2;
		}
	}
	forward_result = Forward(data, bestline, core_mode);
	if (forward_result && optimize_device.size() > 0) {
		while(optimize_device.size() > 0) {
			vector<int> dev = optimize_device[0];
			core_start_device = dev[0];
			core_start_window_index = dev[2]+1;
			optimize_device.clear();
			forward_result = Forward(data, bestline, core_mode);
			end_time = clock();
			process_time = (end_time - start_time) / 1000;
			//if (process_time >= 10)
			//	break;
		}
	}
	Forward(data, bestline, false);
	bestline = installed_area.size() - 1;
	Backward(data, bestline);
	Output(data, bestline);
}


/**********************************************************************************************
��װƥ���㷨
**********************************************************************************************/
bool Result::Install_Match(Data& data, int line, int cur_dev, int cur_wind_index, bool core_mode) {
	bool valid_result = true;
	int cur_wind, cur_area;
	if (!core_mode) {
		/*����ͷ�ڵ�*/
		for (auto& f : data.linegraph.first_device)
			L1.push(f->index);
		while (1) {
			if (L1.empty()) {
				L1 = L2;
				while (!L2.empty())
					L2.pop();
				cur_wind_index++;
				if (cur_wind_index >= data.sqread_circle.size()) {
					valid_result = false;
					break;
				}
			}
			cur_dev = L1.front();
			if(data.device_data[cur_dev].is_core_device && stage == 0){
				stage = 1;
				core_start_device = cur_dev;
				core_start_window_index = cur_wind_index;
				break;
			}
			/*�жϷǺ����豸�ܷ���뵱ǰ����*/
			if (!data.device_data[cur_dev].is_core_device) {
				cur_wind = data.sqread_circle[cur_wind_index];
				bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, line, cur_area, core_mode);
				if (!match_cur_wind) {
					L2.push(cur_dev);
					L1.pop();
					continue;
				}
				install_device(data, cur_dev, cur_area, cur_wind, line, cur_wind_index, core_mode);  //����ֻȡ�˵�һ��������һ��ƥ�䷽��
			}
			L1.pop();

			/*�ж��Ƿ��ǽ����ڵ�*/
			if (data.device_data[cur_dev].next_device.size() == 0) {
				if (L1.empty() && L2.empty())
					break;
				else
					continue;
			}

			/*�ж��Ƿ�����Ч�������У����ܷ��������豸*/
			/*���滹�нڵ㣬�����Ѿ��������һ�����ڣ����ǰ����Эͬ��ϵ�Ļ�������Чƥ��*/
			if (cur_wind_index == data.sqread_circle.size() - 1) {
				for (auto& next : data.device_data[cur_dev].next_device) {
					if (data.linegraph.adjacent_matrix[cur_dev][next->index != 2]) {
						valid_result = false;
						break;
					}
				}
			}
			if (!valid_result)//��Чƥ�䣬����ѭ��
				break;

			/*�ж��ӽڵ����Ƿ�������ڵ�*/
			for (auto& son_dev : data.device_data[cur_dev].next_device) {
				/*���������豸�����ӽڵ����L1����*/
				if (son_dev->last_device.size() == 1)  //Ӧ�ò�����ֵ���0�����
					L1.push(son_dev->index);
				else {
					/*�������豸�����ж��ӽڵ����е������豸�Ƿ��Ѱ�װ*/
					if (son_dev->done_lastnum == son_dev->last_device.size() - 1)
						L1.push(son_dev->index);
					else
						son_dev->done_lastnum++;  //û��װ����ȵ��ţ���ȫ����װ�����ڷ�����н���ƥ��
				}
			}
		}
	}
	if (core_mode) {
		while (1) {
			/*�ж��ܷ���뵱ǰ����*/
			cur_wind = data.sqread_circle[cur_wind_index];
			bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, line, cur_area, core_mode);
			if (!match_cur_wind) {
				cur_wind_index++;
				if (cur_wind_index >= data.sqread_circle.size()) {
					valid_result = false;
					break;
				}
				continue;
			}
			install_device(data, cur_dev, cur_area, cur_wind, line, cur_wind_index, core_mode);

			/*�ж��Ƿ��ǽ����ڵ�*/
			if (cur_dev == data.coreline.core_devices[data.coreline.edge_num])
				break;

			/*�ж��Ƿ�����Ч�������У����ܷ��������豸*/
			/*���滹�нڵ㣬�����Ѿ��������һ�����ڣ����ǰ����Эͬ��ϵ�Ļ�������Чƥ��*/
			if (cur_wind_index == data.sqread_circle.size() - 1) {
				if (data.linegraph.adjacent_matrix[cur_dev][data.device_data[cur_dev].next_coredev->index != 2]) {
					valid_result = false;
					break;
				}
			}
			if (!valid_result)//��Чƥ�䣬����ѭ��
				break;
			cur_dev = data.device_data[cur_dev].next_coredev->index;
		}
	}

	return valid_result;
}


/**********************************************************************************************
��װ�豸������״̬
**********************************************************************************************/
void Result::install_device(Data& data, int device_index, int area_index, int wind, int line, int wind_index, bool core_mode) {
	int energy_type = data.area_data[area_index].energy_type;

	if (core_mode) {
		//��װ�Ż����ۼ���
		long cur_cost = data.device_data[device_index].energy_install_cost[data.area_data[area_index].energy_type];
		for (auto& cost : data.device_data[device_index].energy_install_cost) {
			if (cost < cur_cost && cost != 0)
				data.device_data[device_index].optim_cost = cost - cur_cost;
		}

		//Ԥ�ӹ��������Ż����ۼ���-ֻ�к����豸��Ҫ����
		if (data.device_data[device_index].is_core_device && data.window_data[wind].in_times > 0) {
			int last_process_time = 0, cur_process_time = 0, last_intimes = data.window_data[wind].in_times;
			for (auto& last_dev : data.window_data[wind].already_installed_device) {
				last_process_time = max(last_process_time, data.device_process_time[data.area_data
					[data.device_data[last_dev].installed_area].energy_type]);
			}
			cur_process_time = data.device_process_time[energy_type];

			//���п��Ż�����
			if (last_process_time > cur_process_time) {
				data.device_data[device_index].optim_cost += data.coreline.production_times *
					(last_process_time - cur_process_time);
			}
			if (last_process_time < cur_process_time) {
				data.device_data[device_index].optim_cost += data.coreline.production_times *
					last_intimes * (cur_process_time - last_process_time);
			}
		}
	}

	//����״̬����
	data.window_data[wind].already_installed_device.push_back(device_index);
	//����豸�Ǻ����豸�ż���ӹ�ʱ�䣬process_time�ĳ�set���Ǻ����豸������push
	if (data.device_data[device_index].is_core_device) {
		data.window_data[wind].process_time.insert(data.device_process_time[energy_type]);
		if (data.device_data[device_index].last_device.size() == 0) //����ͷ���Ĵ���Ҫ�ӽ�ȥ
			data.window_data[wind].in_times++;
		for (auto& dev_last : data.device_data[device_index].last_device) {
			if (dev_last->is_core_device && data.linegraph.adjacent_matrix[dev_last->index][device_index] != 2)
				data.window_data[wind].in_times++;//����豸���ϸ��豸�Ǻ����豸���������豸����Эͬ�豸
		}
	}

	//����״̬����
	data.area_data[area_index].already_installed_device.push_back(device_index);
	//�豸״̬����
	data.device_data[device_index].installed_area = area_index;
	data.device_data[device_index].install_cost = data.device_data[device_index].energy_install_cost[energy_type];
	//����״̬����
	installed_window[line][device_index] = wind_index;
	installed_area[line][device_index] = area_index;
	//ȫ��״̬����
	data.have_installed_device_num++;
}


long Result::Get_Cost(Data& data) {
	long install_cost = 0;
	long preprocess_cost = 0;
	long window_match_cost = 0;
	for (auto& device : data.device_data) {
		install_cost += device.install_cost;
	}
	for (auto& window : data.window_data) {  //�����������ֵ����ӹ�ʱ��
		if (window.process_time.size() > 0) {
			set<int>::iterator end = window.process_time.end();
			end--;
			preprocess_cost += window.cost_coefficient * (*end);
			window_match_cost += (*end) * window.in_times * data.coreline.production_times;
		}
	}
	return install_cost + preprocess_cost + window_match_cost;
}


/**********************************************************************************************
������
**********************************************************************************************/
void Result::Output(Data& data, int line) {
	/*����������*/
	cout << data.device_num << endl;
	line = installed_area.size() - 1;
	/*������װ�ĳ��������±������*/
	for (int i = 0; i < data.device_num; i++) {
		cout << installed_area[line][i] << " ";
	}
	cout << endl;

	/*��ˮ�ߵĲ�����*/
	cout << data.coreline.core_devices.size() << endl;

	/*��ˮ�ߵĴ����±������*/
	int core, wind;
	for (int i = 0; i < data.coreline.core_devices.size(); i++) {
		core = data.coreline.core_devices[i];
		cout << data.sqread_circle[installed_window[line][core]] << " ";
	}
	cout << endl;
}


/**********************************************************************************************
�жϵ�ǰ�豸�ʹ����ܲ���ƥ��
ע�⣺�����wind_index�Ǵ��ںţ�����չ���󴰿����б��
**********************************************************************************************/
bool Result::Check_Match(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index, bool core_mode) {
	bool result = true;
	/*����Ƿ���֧�ִ��ڣ�������Ƿ���ƥ������*/
	if (data.device_data[dev_index].surport_window.find(wind) ==
		data.device_data[dev_index].surport_window.end())
		return false;
	else {
		if (data.device_data[dev_index].surport_window[wind].size() == 0)
			return false;
	}

	if (!core_mode) {
		/*��鵱ǰ�豸���������豸�Ĵ��������Ƿ�С�ڵ�ǰƥ�䴰��������Эͬ�豸���Ե���*/
		for (auto& last_dev : data.device_data[dev_index].last_device) {
			if (data.linegraph.adjacent_matrix[last_dev->index][dev_index] != 2 &&  //����Эͬ�豸
				installed_window[line][last_dev->index] >= wind_index)  //���������豸�İ�װ����
				return false;
		}
	}
	else{
		/*�����������豸�Ĵ��������Ƿ�С�ڵ�ǰƥ�䴰��������Эͬ�豸���Ե���*/
		if (data.device_data[dev_index].last_coredev &&
			data.linegraph.adjacent_matrix[data.device_data[dev_index].last_coredev->index][dev_index] != 2 &&  //����Эͬ�豸
			installed_window[line][data.device_data[dev_index].last_coredev->index] >= wind_index)  //���������豸�İ�װ����
			return false;
	}

	/*����Ѱ�װ�����豸�Ĵ��ڽ����������ƣ������ٷ�������豸*/
	if (data.window_data[wind].limit_in && data.device_data[dev_index].is_core_device &&
		data.window_data[wind].in_times > 0)
		return false;

	long dev_cost = longmax;
	/*��ÿ��������֧�ֵ������Ƿ��ܰ�װ���豸���ܰ�װ�ҷ���С����һ���͸��¸��豸�ķ���*/
	for (auto& area : data.window_data[wind].support_area) {
		int energy_type = data.area_data[area].energy_type;
		if (data.device_data[dev_index].energy_install_cost[energy_type] > 0 &&
			data.device_data[dev_index].energy_install_cost[energy_type] < dev_cost) {
			dev_cost = data.device_data[dev_index].energy_install_cost[energy_type];
			area_index = area;
		}
	}

	return result;
}


/**********************************************************************************************
����ƥ���㷨 ���ڻ���
**********************************************************************************************/
bool Result::Install_Match_back(Data& data, int line) {
	bool valid_result = true;
	int cur_dev, cur_wind = 0;
	int cur_wind_index = data.sqread_circle.size() - 1;
	queue<int> L1, L2;

	/*����β���ڵ�*/
	for (int f = 0; f < data.linegraph.latest_device.size(); f++)
		L1.push(data.linegraph.latest_device[f]->index);

	while (1) {
		if (L1.empty()) {
			L1 = L2;
			while (!L2.empty())
				L2.pop();
			cur_wind_index--;
		}
		if (cur_wind_index < 0) {
			break;
		}
		cur_dev = L1.front();

		/*�ж��ܷ���뵱ǰ����*/
		int cur_area;
		cur_wind = data.sqread_circle[cur_wind_index];
		bool match_cur_wind = Check_Match_back(data, cur_dev, cur_wind, cur_wind_index, line, cur_area);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue;
		}
		int pre_wind = data.sqread_circle[installed_window[line][cur_dev]];
		int pre_area = installed_area[line][cur_dev];
		install_device_back(data, cur_dev, cur_area, cur_wind, cur_wind_index, line, pre_wind, pre_area);
		L1.pop();

		/*�ж��Ƿ��ǽ����ڵ�*/
		if (data.device_data[cur_dev].last_device.size() == 0) {
			if (L1.empty() && L2.empty())
				break;
			else
				continue;
		}

		/*�ж��Ƿ�����Ч�������У����ܷ��������豸*/
		/*���滹�нڵ㣬�����Ѿ��������һ�����ڣ����ǰ����Эͬ��ϵ�Ļ�������Чƥ��*/
		if (cur_wind_index == 0) {
			for (auto& last : data.device_data[cur_dev].last_device) {
				if (data.linegraph.adjacent_matrix[last->index][cur_dev] != 2) {
					valid_result = false;
					break;
				}
			}
		}
		if (!valid_result)//��Чƥ�䣬����ѭ��
			break;

		/*�ж��ӽڵ����Ƿ�������ڵ�*/
		for (auto& son_dev : data.device_data[cur_dev].last_device) {
			/*���������豸�����ӽڵ����L1����*/
			if (son_dev->next_device.size() == 1)  //Ӧ�ò�����ֵ���0�����
				L1.push(son_dev->index);
			else {
				/*�������豸�����ж��ӽڵ����е������豸�Ƿ��Ѱ�װ*/
				if (son_dev->done_lastnum == son_dev->next_device.size() - 1)
					L1.push(son_dev->index);
				else
					son_dev->done_lastnum++;  //û��װ����ȵ��ţ���ȫ����װ�����ڷ�����н���ƥ��
			}
		}
	}
	return valid_result;
}

/**********************************************************************************************
�жϵ�ǰ�豸�ʹ����ܲ���ƥ�䣨�Ż��汾��
ע�⣺�����wind_index�Ǵ��ںţ�����չ���󴰿����б��
**********************************************************************************************/
bool Result::Check_Match_back(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index) {
	bool result = true;
	/*����Ƿ���֧�ִ��ڣ�������Ƿ���ƥ������*/
	if (data.device_data[dev_index].surport_window.find(wind) ==
		data.device_data[dev_index].surport_window.end())
		return false;
	else {
		if (data.device_data[dev_index].surport_window[wind].size() == 0)
			return false;
	}

	/*��鵱ǰ�豸���������豸�Ĵ��������Ƿ�С�ڵ�ǰƥ�䴰��������Эͬ�豸���Ե���*/
	for (auto& next_dev : data.device_data[dev_index].next_device) {
		if (data.linegraph.adjacent_matrix[next_dev->index][dev_index] != 2 &&  //����Эͬ�豸
			installed_window[line][next_dev->index] <= wind_index)  //���������豸�İ�װ����
			return false;
	}

	/*����Ѱ�װ�����豸�Ĵ��ڽ����������ƣ������ٷ�������豸*/
	if (data.window_data[wind].limit_in && data.device_data[dev_index].is_core_device &&
		data.window_data[wind].in_times > 0)
		return false;

	/*������������а�װ�ɱ���С���������ĸ�*/
	int min_cost = longmax, area_;
	for (auto& area : data.device_data[dev_index].surport_window[wind])
	{
		int new_cost = data.device_data[dev_index].energy_install_cost[data.area_data[area].energy_type];
		if (new_cost < min_cost) {
			min_cost = new_cost;
			area_ = area;
		}
	}

	/*����Ƶ�������ڳɱ�����ˣ��Ͳ������������*/
	if (min_cost > data.device_data[dev_index].install_cost)
		return false;
	area_index = area_;

	return result;
}

/**********************************************************************************************
��װ�豸������״̬�����ڻ���
**********************************************************************************************/
void Result::install_device_back(Data& data, int device_index, int area_index, int wind,
	int wind_index, int line, int pre_wind_index, int pre_area_index) {

	//int window_index = data.area_data[area_index].window_index;
	int pre_energy_type = data.area_data[pre_area_index].energy_type;
	int energy_type = data.area_data[area_index].energy_type;

	/*ɾ��֮ǰ��װ��״̬*/
	if (pre_wind_index >= 0)
	{
		/*����״̬�ָ�*/
		auto it = find(data.window_data[pre_wind_index].already_installed_device.begin(),
			data.window_data[pre_wind_index].already_installed_device.end(), device_index);
		data.window_data[pre_wind_index].already_installed_device.erase(it);

		//�Ǻ����豸�Żָ�
		if (data.device_data[device_index].is_core_device) {
			data.window_data[pre_wind_index].process_time.clear();
			for (auto it_dev : data.window_data[pre_wind_index].already_installed_device) {
				if (data.device_data[it_dev].is_core_device)
					data.window_data[pre_wind_index].process_time.insert(data.device_process_time[pre_energy_type]);
			}
		}
		//����״̬�ָ�
		auto it_area = find(data.area_data[pre_area_index].already_installed_device.begin(),
			data.area_data[pre_area_index].already_installed_device.end(), device_index);
		data.area_data[pre_area_index].already_installed_device.erase(it_area);
		//ȫ��״̬�ָ�
		data.have_installed_device_num--;
	}

	//����״̬����
	data.window_data[wind].already_installed_device.emplace_back(device_index);
	//����豸�Ǻ����豸�ż���ӹ�ʱ�䣬process_time�ĳ�set���Ǻ����豸������push
	if (data.device_data[device_index].is_core_device) {
		data.window_data[wind].process_time.insert(data.device_process_time[energy_type]);
		if (data.device_data[device_index].last_device.size() == 0) //����ͷ���Ĵ���Ҫ�ӽ�ȥ
			data.window_data[wind].in_times++;
		for (auto& dev_last : data.device_data[device_index].last_device) {
			if (dev_last->is_core_device && data.linegraph.adjacent_matrix[dev_last->index][device_index] != 2)
				data.window_data[wind].in_times++;//����豸���ϸ��豸�Ǻ����豸���������豸����Эͬ�豸
		}
	}
	//����״̬����
	data.area_data[area_index].already_installed_device.emplace_back(device_index);
	//�豸״̬����
	data.device_data[device_index].installed_area = area_index;
	data.device_data[device_index].install_cost = data.device_data[device_index].energy_install_cost[energy_type];
	//����״̬����
	installed_window[line][device_index] = wind_index;
	installed_area[line][device_index] = area_index;
	//ȫ��״̬����
	data.have_installed_device_num++;
}