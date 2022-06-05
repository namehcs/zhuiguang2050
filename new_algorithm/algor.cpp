#include "algor.h"
//#include <windows.h>



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
		forward_cost = Get_Cost(data, result_index);
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
}

void Result::Backward(Data& data, int& bestline) {
	long backward_cost;
	bool backward_valid;
	/*���ݴӺ���ǰ�������Ĵ���ƥ�䣬�����һ����ʼ��ƥ��*/
	//installed_window.push_back(installed_window[result_index]);
	//installed_area.push_back(installed_area[result_index]);
	installed_window.push_back(vector<int>(data.device_num, 999));
	installed_area.push_back(vector<int>(data.device_num, 999));
	result_index++;
	backward_valid = Install_Match_back(data, result_index);
	if (backward_valid) {
		backward_cost = Get_Cost(data, result_index);
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

void Result::fff(Data& data, int& bestline) {
	long fffward_cost;
	bool fffward_valid;

	/*install[2]Ϊ���ԣ�install[3]Ϊ����*/
	installed_window.push_back(installed_window[0]);
	installed_area.push_back(installed_area[0]);
	match_costs.push_back(match_costs[0]);

	installed_window.push_back(installed_window[0]);
	installed_area.push_back(installed_area[0]);
	match_costs.push_back(match_costs[0]);

	fffward_valid = Install_Match_fff(data);
	fffward_cost = Get_Cost(data, result_index);

	/*������ݴ���С����ʹ��ۣ����������ƥ��*/
	bestline = result_index;
}


/**********************************************************************************************
�������Ż��㷨
**********************************************************************************************/
void Result::Algorithm(Data& data) {
	int bestline = 0;
	long start_time, end_time, process_time;
	bool forward_result, backward_result, core_mode = false;

	Forward(data, bestline);
	forward = false;
	Backward(data, bestline);
	forward = true;
	fff(data, bestline);

	Output(data, 3);
}

long Result::find_area(Data& data, int dev_index, int wind, int& area_index) {
	/*������������а�װ�ɱ���С���������ĸ�*/
	long cost = longmax;
	for (auto& area : data.device_data[dev_index].surport_window[wind]) {
		int install_cost = data.device_data[dev_index].energy_install_cost[data.area_data[area].energy_type];
		if (install_cost < cost) {
			cost = install_cost;
			area_index = area;
		}
	}

	return cost;
}

/**********************************************************************************************
��װƥ���㷨
**********************************************************************************************/
bool Result::Small_Optim_Match(Data& data, queue<int> L1, queue<int> L2, vector<int> done_lastnum) {
	bool valid_result = true;
	int cur_dev = L1.front(), cur_wind, cur_area, cur_wind_index = installed_window[0][cur_dev] + 1, p;

	for (int i = installed_window[0][cur_dev] + 1; i <= installed_window[1][cur_dev]; i++) {
		//installed_window[2] = installed_window[0];
		//installed_area[2] = installed_area[0];
		p = 0;
		for (auto& next_dev : data.device_data[cur_dev].next_device) {
			if (installed_window[2][next_dev->index] > i)
				p++;
		}
		if (p == data.device_data[cur_dev].next_device.size()) {//ûӰ�������豸������Ҫƥ�䣬ֱ�Ӹ�
			installed_window[2][cur_dev] = i;
			int now_area;
			find_area(data, cur_dev, data.sqread_circle[i], now_area);
			installed_area[2][cur_dev] = now_area;
			int cost_1 = Get_Cost(data, 2); //С�Ĳ���
			match_costs[2] = cost_1;
			if (cost_1 < match_costs[3]) {
				installed_window[3] = installed_window[2];
				installed_area[3] = installed_area[2];
				match_costs[3] = cost_1;
			}
			continue;
		}
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
			bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, 2, cur_area);
			if (!match_cur_wind) {
				L2.push(cur_dev);
				L1.pop();
				continue;
			}
			install_device(data, cur_dev, cur_area, cur_wind, cur_wind_index, 2);
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
					if (done_lastnum[son_dev->index] == son_dev->last_device.size() - 1)
						L1.push(son_dev->index);
					else
						done_lastnum[son_dev->index]++;  //û��װ����ȵ��ţ���ȫ����װ�����ڷ�����н���ƥ��
				}
			}
		}
		int cost_2 = Get_Cost(data, 2); //С�Ĳ���
		if (cost_2 < match_costs[3]) {
			installed_window[3] = installed_window[2];
			installed_area[3] = installed_area[2];
			match_costs[3] = cost_2;
		}
	}

	return valid_result;
}

bool Result::Install_Match_fff(Data& data) {
	bool valid_result = true;
	queue<int> L1, L2;
	vector<int> done_lastnum(data.device_num, 0);
	int cur_wind, cur_area, cur_dev, start_optim_wind, cur_wind_index = 0;
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

		/*�ֲ�ǰ�����*/
		if (installed_window[1][cur_dev] - installed_window[0][cur_dev] > 0) {
			Small_Optim_Match(data, L1, L2, done_lastnum);
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
				if (done_lastnum[son_dev->index] == son_dev->last_device.size() - 1)
					L1.push(son_dev->index);
				else
					done_lastnum[son_dev->index]++;  //û��װ����ȵ��ţ���ȫ����װ�����ڷ�����н���ƥ��
			}
		}
	}

	return valid_result;
}


bool Result::Install_Match(Data& data, int line) {
	bool valid_result = true;
	queue<int> L1, L2;
	vector<int> done_lastnum(data.device_num, 0);
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
		install_device(data, cur_dev, cur_area, cur_wind, cur_wind_index, line);
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
				if (done_lastnum[son_dev->index] == son_dev->last_device.size() - 1)
					L1.push(son_dev->index);
				else
					done_lastnum[son_dev->index]++;  //û��װ����ȵ��ţ���ȫ����װ�����ڷ�����н���ƥ��
			}
		}
	}

	return valid_result;
}


/**********************************************************************************************
��װ�豸������״̬
**********************************************************************************************/
void Result::install_device(Data& data, int device_index, int area_index, int wind, int wind_index, int line) {

	//�豸״̬����
	data.device_data[device_index].installed_area = area_index;
	//����״̬����
	installed_window[line][device_index] = wind_index;
	installed_area[line][device_index] = area_index;
}

long Result::Get_Cost(Data& data, int line) {
	long install_cost = 0;
	long preprocess_cost = 0;
	long window_match_cost = 0;
	vector<vector<int>> winds(data.window_num, vector<int>(2, 0)); //{���ӹ�ʱ��, �������}
	for (int i = 0; i < data.device_num; i++) {
		install_cost += data.device_data[i].energy_install_cost[data.area_data[installed_area[line][i]].energy_type];
	}
	for (int i = 0; i < data.device_num; i++) {
		int w = data.sqread_circle[installed_window[line][i]];
		if(data.device_data[i].is_core_device){
			winds[w][0] = max(winds[w][0], data.device_process_time[
				data.area_data[installed_area[line][i]].energy_type]);//���ӹ�ʱ��
			if (data.device_data[i].last_coredev &&
				installed_window[line][data.device_data[i].last_coredev->index]
				== installed_window[line][i] &&  //���ﲻ���Ǵ��ںţ�ֻ���Ǵ�������
				data.linegraph.adjacent_matrix[data.device_data[i].
				last_coredev->index][i] == 2)
				continue;
			winds[w][1]++;
		}
	}
	for (auto& window : data.window_data) {  //�����������ֵ����ӹ�ʱ��
		preprocess_cost += window.cost_coefficient * winds[window.index][0];
		window_match_cost += winds[window.index][0] * winds[window.index][1] * data.coreline.production_times;
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
	if (forward) {
		for (auto& last_dev : data.device_data[dev_index].last_device) {
			if (data.linegraph.adjacent_matrix[last_dev->index][dev_index] != 2 &&  //����Эͬ�豸
				installed_window[line][last_dev->index] >= wind_index)  //���������豸�İ�װ����
				return false;
		}
	}
	if(!forward) {
		for (auto& next_dev : data.device_data[dev_index].next_device) {
			if (data.linegraph.adjacent_matrix[dev_index][next_dev->index] != 2 &&  //����Эͬ�豸  ����Эͬ�豸û����
				installed_window[line][next_dev->index] <= wind_index)  //���������豸�İ�װ����
				return false;
		}
	}

	/*������������а�װ�ɱ���С���������ĸ�*/
	long cost = find_area(data, dev_index, wind, area_index);
	data.device_data[dev_index].install_cost = cost;

	return result;
}

/**********************************************************************************************
����ƥ���㷨 ���ڻ���
**********************************************************************************************/
bool Result::Install_Match_back(Data& data, int line) {
	bool valid_result = true;
	queue<int> L1, L2;//�豸�ţ����Կ�ʼ�����Ĵ��ں�
	vector<int> done_lastnum(data.device_num, 0);
	int cur_dev, start_wind_index, cur_wind = 0;
	int cur_wind_index = data.sqread_circle.size() - 1;
	

	/*����β���ڵ�*/
	for (int f = 0; f < data.linegraph.latest_device.size(); f++)
		L1.push(data.linegraph.latest_device[f]->index);

	while (1) {
		if (L1.empty()) {
			L1 = L2;
			while (!L2.empty())
				L2.pop();
			cur_wind_index--;
			if (cur_wind_index < 0) {
				valid_result = false;
				break;
			}
		}
		cur_dev = L1.front();

		///*�ֲ�ǰ�����*/

		/*�ж��ܷ���뵱ǰ����*/
		int cur_area;
		cur_wind = data.sqread_circle[cur_wind_index];
		bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, line, cur_area);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue;
		}

		install_device(data, cur_dev, cur_area, cur_wind, cur_wind_index, line);
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
				if (done_lastnum[son_dev->index] == son_dev->next_device.size() - 1)
					L1.push(son_dev->index);
				else
					done_lastnum[son_dev->index]++;  //û��װ����ȵ��ţ���ȫ����װ�����ڷ�����н���ƥ��
			}
		}
	}
	return valid_result;
}

