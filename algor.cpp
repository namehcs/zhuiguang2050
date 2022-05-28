#include "algor.h"
//#include <windows.h>


void Result::State_Clear(Data& data, bool clear_all) {
	if (clear_all) {//ȫ������
		for (auto& wind : data.window_data) {
			//��
		}
		for (auto& area : data.area_data) {}
		for (auto& device : data.device_data) {
			device.done_lastnum = 0;
		}
		data.have_installed_device_num = 0;
	}
	else {//��������
		for (auto& wind : data.window_data) {

		}
	}
}


//void Result::Sort_Device(Data& data, int line) {
//	/*�Ƚ�ÿ�������豸�Ż����۵Ĵ�С���Ӵ�С����*/
//	vector<int> one;
//	for (auto& dev : data.coreline.core_devices) {
//		if (data.device_data[dev].optim_cost > 0) {
//			one.push_back(data.device_data[dev].index);
//			one.push_back(data.device_data[dev].optim_cost);
//			one.push_back(installed_window[line][dev]);
//			optimize_device.push_back(one);
//			one.clear();
//		}
//	}
//
//	sort(optimize_device.begin(), optimize_device.end(), [](vector<int>& x, vector<int>& y) { return x[1] > y[1]; });
//}

void Result::Forward(Data& data, int& bestline) {
	/*����ǰ��ƥ�䣬ע��������Ҫ�����зŽ�data.sqread_circle���ܽ��к�������*/
	/*�����һ����ʼ��ƥ��*/
	long forward_cost;
	bool forward_valid;
	installed_window.push_back(vector<int>(data.device_num, 999));
	installed_area.push_back(vector<int>(data.device_num, 999));
	result_index++;
	forward_valid = Install_Match(data, result_index);
	if (forward_valid) {
		//Sort_Device(data, result_index);
		forward_cost = Get_Cost(data);
		match_costs.push_back(forward_cost);
		if (forward_cost < match_costs[bestline])
			bestline = result_index;
	}
	if (!forward_valid) {
		result_index--;
		installed_window.pop_back();
		installed_area.pop_back();
		match_costs.pop_back();
	}
	State_Clear(data, true);
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
		/*������ݴ���С����ʹ��ۣ����������ƥ��*/
		if (backward_cost < match_costs[bestline])
			bestline = result_index;
	}
	else if (!backward_valid) {
		result_index--;
		installed_window.pop_back();
		installed_area.pop_back();
		match_costs.pop_back();
	}
}

/**********************************************************************************************
�������Ż��㷨
**********************************************************************************************/
void Result::Algorithm(Data& data) {
	int bestline = 0;
	long start_time, end_time, process_time;
	bool forward_result, backward_result, core_mode = false;
	start_time = clock();

	Forward(data, bestline);
	Backward(data, bestline);

	end_time = clock();
	process_time = (end_time - start_time) / 1000;

	Output(data, bestline);
}


/**********************************************************************************************
��װƥ���㷨
**********************************************************************************************/
bool Result::Small_Optim_Match(Data& data, int line, int father_dev, int start_wind_index) {
	bool valid_result = true;
	int cur_wind, cur_area, cur_wind_index;
	for (auto& son_dev : data.device_data[father_dev].next_device) {
		/*ȷ��������Χ���ӿ�ʼ�豸��������ţ��������豸���������*/
		int start_dev = son_dev->index;
		int cur_dev = start_dev, end_dev, end_wind_index;
		while (1) {
			if (data.device_data[cur_dev].next_device.size() != 1 ||
				data.device_data[cur_dev].next_device[0]->last_device.size() > 1)
				break;
			else
				cur_dev = data.device_data[cur_dev].next_device[0]->index;
		}
		end_dev = cur_dev;
		end_wind_index = installed_window[line][end_dev];
		if (end_wind_index <= start_wind_index)
			continue;

		/*��ʼǰ�����*/
		cur_dev = start_dev;
		cur_wind_index = start_wind_index;
		while (1) {
			valid_result = true;
			/*�ж��ܷ���뵱ǰ����*/
			cur_wind = data.sqread_circle[cur_wind_index];
			bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, line, cur_area);
			if (!match_cur_wind) {
				cur_wind_index++;
				if (cur_wind_index == end_wind_index) {
					valid_result = false;
					break;
				}
				continue;
			}
			install_device(data, cur_dev, cur_area, cur_wind, line, cur_wind_index);

			/*�ж��Ƿ��ǽ����ڵ�*/
			if (cur_dev == end_dev)
				break;

			/*�ж��Ƿ�����Ч�������У����ܷ��������豸*/
			/*���滹�нڵ㣬�����Ѿ��������һ�����ڣ����ǰ����Эͬ��ϵ�Ļ�������Чƥ��*/
			if (cur_wind_index == end_wind_index) {
				if (data.linegraph.adjacent_matrix[cur_dev][data.device_data[cur_dev].next_device[0]->index != 2]) {
					valid_result = false;
					break;
				}
			}
			if (!valid_result)//��Чƥ�䣬����ѭ��
				break;
			cur_dev = data.device_data[cur_dev].next_device[0]->index;
		}
	}

	return valid_result;
}


bool Result::Install_Match(Data& data, int line) {
	bool valid_result = true;
	queue<int> L1, L2;
	int cur_wind, cur_area, cur_dev, cur_wind_index = 0;
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
		cur_wind = data.sqread_circle[cur_wind_index];
		bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, line, cur_area);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue;
		}
		install_device(data, cur_dev, cur_area, cur_wind, line, cur_wind_index);
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

	return valid_result;
}


/**********************************************************************************************
��װ�豸������״̬
**********************************************************************************************/
void Result::install_device(Data& data, int device_index, int area_index, int wind, int line, int wind_index) {
	int energy_type = data.area_data[area_index].energy_type;

	//����״̬����
	if (data.device_data[device_index].is_core_device) {
		data.window_data[wind].already_installed_core.insert({ device_index,
			{area_index, data.device_process_time[energy_type]} });  //�����豸�ţ�����źͼӹ�ʱ��

		//�������������֮ǰ��ĳ�������豸������Эͬ�豸������������е�Эͬ����һ
		for (auto& dev_last : data.device_data[device_index].last_device) {
			if (data.window_data[wind].already_installed_core.find(dev_last->index) !=
				data.window_data[wind].already_installed_core.end()) {
				if (data.linegraph.adjacent_matrix[dev_last->index][device_index] == 2)
					data.window_data[wind].in_times++;//Эͬ�豸����1
			}
		}
	}
	else
		data.window_data[wind].already_installed_normall.insert({ device_index, area_index });

	//����״̬����
	data.area_data[area_index].already_installed_device.push_back(device_index);
	//�豸״̬����
	data.device_data[device_index].installed_area = area_index;
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
		int max_time = 0, core_dev_num = window.already_installed_core.size();
		if (core_dev_num > 0) {
			for (auto& dev : window.already_installed_core)
				max_time = max(max_time, dev.second.second);
			preprocess_cost += window.cost_coefficient * max_time;
			window_match_cost += max_time * (core_dev_num - window.in_times) * data.coreline.production_times;
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


long Result::Install_Cost(Data& data, int dev_index, int wind_index, int& area_index)
{
	long cost = longmax;
	int wind = data.sqread_circle[wind_index];

	//�ǷǺ����豸ֻ�а�װ����
	if (!data.device_data[dev_index].is_core_device)
	{
		for (auto& area : data.device_data[dev_index].surport_window[wind]) {
			int now_cost = data.device_data[dev_index].energy_install_cost[data.area_data[area].energy_type];
			if (now_cost < cost) {
				cost = now_cost;
				area_index = area;
			}
		}
	}
	else
	{
		//��ȡ��ǰ���ڵļӹ�ʱ��
		int last_process_time = 0, cur_process_time = 0;
		int last_intimes = data.window_data[wind].already_installed_core.size() - data.window_data[wind].in_times;
		for (auto& last_dev : data.window_data[wind].already_installed_core) {
			last_process_time = max(last_process_time, data.device_process_time[data.area_data
				[data.device_data[last_dev.first].installed_area].energy_type]);
		}

		for (auto& area : data.device_data[dev_index].surport_window[wind])
		{
			int now_cost = data.device_data[dev_index].energy_install_cost[data.area_data[area].energy_type];
			cur_process_time = data.device_process_time[data.area_data[area].energy_type];

			//���д���
			if (last_process_time > cur_process_time) {
				now_cost += data.coreline.production_times * last_process_time;
			}
			if (last_process_time <= cur_process_time) {
				now_cost += data.coreline.production_times * last_intimes * (cur_process_time - last_process_time);
				now_cost += data.coreline.production_times * cur_process_time;
			}

			if (now_cost < cost) {
				cost = now_cost;
				area_index = area;
			}
		}
	}
	return cost;
}

/**********************************************************************************************
�жϵ�ǰ�豸�ʹ����ܲ���ƥ��
ע�⣺�����wind_index�Ǵ��ںţ�����չ���󴰿����б��
**********************************************************************************************/
bool Result::Check_Match(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index) {
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
	for (auto& last_dev : data.device_data[dev_index].last_device) {
		if (data.linegraph.adjacent_matrix[last_dev->index][dev_index] != 2 &&  //����Эͬ�豸
			installed_window[line][last_dev->index] >= wind_index)  //���������豸�İ�װ����
			return false;
	}

	long dev_cost = longmax;
	/*������������а�װ�ɱ���С���������ĸ�*/
	int area = -1;
	int cost = Install_Cost(data, dev_index, wind_index, area);

	/*����Ƶ�������ڳɱ�����ˣ��Ͳ�����������ڣ�����������������һ��ǰ��ͺ������ŵ�ǰ��*/
	if (cost >= data.device_data[dev_index].install_cost)
		return false;

	data.device_data[dev_index].install_cost = cost;
	area_index = area;

	return result;
}

/**********************************************************************************************
����ƥ���㷨 ���ڻ���
**********************************************************************************************/
bool Result::Install_Match_back(Data& data, int line) {
	bool valid_result = true;
	int cur_dev, start_wind_index, cur_wind = 0;
	int cur_wind_index = data.sqread_circle.size() - 1;
	queue<int> L1, L2;//�豸�ţ����Կ�ʼ�����Ĵ��ں�
	unordered_set<int> L3; //���Ե������豸�ţ�ͬһʱ�ڻ���Ӱ��
	int start_optim_wind;

	/*����β���ڵ�*/
	for (int f = 0; f < data.linegraph.latest_device.size(); f++)
		L1.push(data.linegraph.latest_device[f]->index);

	while (1) {
		if (L1.empty() && L2.empty())
			break;
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

		/*�ֲ�ǰ�����*/
		for (auto& son_dev : data.device_data[cur_dev].last_device) {
			if (son_dev->next_device.size() > 1 && L3.find(son_dev->index) == L3.end()) {
				L3.insert(son_dev->index);
				start_optim_wind = installed_window[0][son_dev->index];
				Small_Optim_Match(data, line, son_dev->index, start_optim_wind);
			}
		}

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

	/*������������а�װ�ɱ���С���������ĸ�*/
	int area = -1;
	int cost = Install_Cost(data, dev_index, wind_index, area);

	/*����Ƶ�������ڳɱ�����ˣ��Ͳ������������*/
	if (cost > data.device_data[dev_index].install_cost)
		return false;

	data.device_data[dev_index].install_cost = cost;
	area_index = area;
	//cout << dev_index << ": " << cost << endl;
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
		if (data.device_data[device_index].is_core_device) {
			//ɾ����װ�豸
			data.window_data[pre_wind_index].already_installed_core.erase(device_index);
			//ɾ��Эͬ��
			for (auto& it_dev : data.window_data[pre_wind_index].already_installed_core) {
				if (data.linegraph.adjacent_matrix[it_dev.first][device_index] == 2 ||
					data.linegraph.adjacent_matrix[device_index][it_dev.first] == 2)
					data.window_data[pre_wind_index].in_times--;
			}
		}
		else
		{
			data.window_data[pre_wind_index].already_installed_normall.erase(device_index);
		}

		//����״̬�ָ�
		auto it_area = find(data.area_data[pre_area_index].already_installed_device.begin(),
			data.area_data[pre_area_index].already_installed_device.end(), device_index);
		data.area_data[pre_area_index].already_installed_device.erase(it_area);
	}

	//����״̬����
	if (data.device_data[device_index].is_core_device) {
		data.window_data[wind].already_installed_core.insert({ device_index , {wind_index, area_index} });
	}
	else
	{
		data.window_data[wind].already_installed_normall.insert({ device_index , area_index });
	}

	//����Эͬ��
	if (data.device_data[device_index].is_core_device) {
		for (auto& it_dev : data.window_data[wind].already_installed_core) {
			if (data.linegraph.adjacent_matrix[it_dev.first][device_index] == 2 ||
				data.linegraph.adjacent_matrix[device_index][it_dev.first] == 2)
				data.window_data[wind].in_times++;
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
}
