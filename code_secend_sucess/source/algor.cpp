#include "algor.h"


/**********************************************************************************************
�������Ż��㷨
**********************************************************************************************/
void Result::Algorithm(Data& data, int line_num) {
	int line = 0, start_wind_index = 0;
	queue<int> L1;
	vector<long> match_costs;
	vector<vector<int>> unhook_window; //��Ϊ�ɲ𻷵�window���Լ����򣬲�һ��Ҫ��unordered_map
	/*����ͷ�ڵ�Ͳ����һ����ʼ��ƥ��*/
	for (auto& f : data.linegraph.first_device)
		L1.push(f->index);
	installed_window.insert({ 0, vector<int>(data.device_num, 999) });
	installed_area.insert({ 0, vector<int>(data.device_num, 999) });

	/*����ȫ��չ���������е�ƥ��*/
	bool flag_res = Install_Match(data, L1, start_wind_index, line);
	bool flag_res2 = Install_Match_back(data, line);
	Output(data, 0);
	//cout<< Get_Cost(data)<<endl;
	//if (flag_res) {
	//	long one_cost = Get_Cost(data);
	//	match_costs.push_back(one_cost);
	//	/*�Ƚ�ÿ�����а�װ�豸�����ӹ�ʱ�����С�ӹ�ʱ���ֵ����ֵԽ��𻷺�ӹ�ʱ���СԽ�࣬����ԽС*/
	//	for (auto& wind : data.window_data) {
	//		if (wind.self_loop){
	//			vector<int> one;
	//			if (wind.process_time.size() > 0) {
	//				set<int>::iterator end = wind.process_time.end(); end--;
	//				one.push_back((*end) - (*wind.process_time.begin()));
	//				one.push_back(wind.index);
	//			}
	//			else {
	//				one.push_back(0);
	//				one.push_back(wind.index);
	//			}
	//			unhook_window.push_back(one);
	//		}
	//	}
	//	/*�ӹ�ʱ���ֵ�Ѿ�����С�����˳������map�����ˣ�keyΪ��ֵ��valueΪ���ں�*/
	//	/*��˳��
	//	1���Ƚ��Իػ��������͵�һ�໷�ط�Χ
	//	2������ػ�����С�ڵ�һ��ػ���Χ���Ȳ��Իػ��в�ֵ���ģ��Ӵ�С(>0,0û��Ҫ��)
	//	*/
	//	int unhook_self_circle = 1;
	//	if (data.max_loop_num <= data.first_loop_window_num) {
	//		/*���Իػ�*/
	//		for (auto& wind : unhook_window) {
	//			int best_line = Unhook(data, wind[1]);
	//			
	//		}
	//	}
	//	Output(data, line);
	//	cout << match_costs[0] << endl;
	//	//�������res_index�Ĵ��ۣ���ѡ�������С��ƥ����
	//}
}


/**********************************************************************************************
�𻷺���Ż�ƥ���㷨
�ҵ����۸��͵�ƥ��Ͱ�
**********************************************************************************************/
int Result::Unhook(Data& data, int wind) {
	int line = 1, start_wind_index = 0;
	queue<int> L1;
	vector<int> unhook_line = data.sqread_circle[0];
	/*�ȸ��ݲ𻷴��ں����ɴ�������*/
	for (int i = 1; i <= data.max_loop_num; i++) {
		for (vector<int>::iterator it = unhook_line.begin() + 1; it != unhook_line.end(); it++) {
			if (wind == (*it) && wind == (*(it-1))) {  //�������������������wind����ɾȥ����һ��
				unhook_line.erase(it);
				data.sqread_circle.insert({ line, unhook_line });
				break;
			}
			start_wind_index++; line++;
		}
		for (auto& dev : data.window_data[start_wind_index].already_installed_device)
			L1.push(dev);
		bool flag_res = Install_Match(data, L1, start_wind_index, line);
	}
	return line;
}


/**********************************************************************************************
��װƥ���㷨
**********************************************************************************************/
bool Result::Install_Match(Data& data, queue<int>& L1, int start_wind_index, int line) {
	bool valid_result = true;
	int cur_dev, cur_wind, cur_wind_index = start_wind_index;
	vector<int> done_lastnum(data.device_num, 0);     //���̱���������ڵ���ƥ����������
	queue<int> L2;

	while (1) {
		if (L1.empty()) {
			L1 = L2;
			while (!L2.empty())
				L2.pop();
			cur_wind_index++;
		}
		cur_dev = L1.front();

		/*�ж��ܷ���뵱ǰ����*/
		cur_wind = data.sqread_circle[line][cur_wind_index];
		bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, line);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue;
		}
		int cur_area = Area_Match(data, cur_dev, cur_wind);
		install_device(data, cur_dev, cur_area, cur_wind, cur_wind_index);  //����ֻȡ�˵�һ��������һ��ƥ�䷽��
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
		if (cur_wind_index == data.sqread_circle[line].size() - 1){
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
��װ�豸������״̬������
**********************************************************************************************/
void Result::install_device(Data& data, int device_index, int area_index, int wind, int wind_index) {
	int energy_type = data.area_data[area_index].energy_type;
	//����״̬����
	data.window_data[wind].already_installed_device.emplace_back(device_index);
	//����豸�Ǻ����豸�ż���ӹ�ʱ�䣬process_time�ĳ�set���Ǻ����豸������push
	if (data.device_data[device_index].is_core_device) {
		data.window_data[wind].process_time.insert(data.device_process_time[energy_type]);
		if(data.device_data[device_index].last_device.size() == 0) //����ͷ���Ĵ���Ҫ�ӽ�ȥ
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
	installed_window[0][device_index] = wind_index;
	installed_area[0][device_index] = area_index;
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
����ƥ���㷨
**********************************************************************************************/
int Result::Area_Match(Data& data, int dev_index, int wind) {
	int match_area;
	long dev_cost = LONG_MAX;
	/*��ÿ��������֧�ֵ������Ƿ��ܰ�װ���豸���ܰ�װ�ҷ���С����һ���͸��¸��豸�ķ���*/
	for (int j = 0; j < data.window_data[wind].support_area.size(); j++) {
		int area = data.window_data[wind].support_area[j];
		int energy_type = data.area_data[area].energy_type;
		if (data.device_data[dev_index].energy_install_cost[energy_type] > 0 &&
			data.device_data[dev_index].energy_install_cost[energy_type] < dev_cost)
		{
			dev_cost = data.device_data[dev_index].energy_install_cost[energy_type];
			match_area = area;
		}
	}
	return match_area;
}


/**********************************************************************************************
������
**********************************************************************************************/
void Result::Output(Data& data, int line) {
	/*����������*/
	cout << data.device_num << endl;

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
		cout << data.sqread_circle[line][installed_window[line][core]] << " ";
	}
	cout << endl;
}


/**********************************************************************************************
�жϵ�ǰ�豸�ʹ����ܲ���ƥ��
ע�⣺�����wind_index�Ǵ��ںţ�����չ���󴰿����б��
**********************************************************************************************/
bool Result::Check_Match(Data& data, int dev_index, int wind, int wind_index, int line) {
	bool result = true;
	/*����Ƿ���֧�ִ��ڣ�������Ƿ���ƥ������*/
	if (data.device_data[dev_index].surport_window.find(wind) ==
		data.device_data[dev_index].surport_window.end())
		return false;
	else {
		if(data.device_data[dev_index].surport_window[wind].size() == 0)
			return false;
	}

	/*��鵱ǰ�豸���������豸�Ĵ��������Ƿ�С�ڵ�ǰƥ�䴰��������Эͬ�豸���Ե���*/
	for (int i = 0; i < data.device_data[dev_index].last_device.size(); i++) {
		int last_dev = data.device_data[dev_index].last_device[i]->index;
		if (data.linegraph.adjacent_matrix[last_dev][dev_index] != 2 &&  //����Эͬ�豸
			installed_window[line][last_dev] >= wind_index)  //���������豸�İ�װ����
			return false;
	}
	return result;
}


///*���ص��ǻػ��������е�*/
//queue<int> Result::Choose_Window(Data& data, int dev_indev, int cur_wind_index) {
//	queue<int> results;
//	return results;
//}

/**********************************************************************************************
����ƥ���㷨 ���ڻ���
**********************************************************************************************/
bool Result::Install_Match_back(Data& data, int line) {
	bool valid_result = true;
	int cur_dev, cur_wind = 0;
	int cur_wind_index = data.sqread_circle[0].size() - 1;
	vector<int> done_lastnum(data.device_num, 0);     //���̱���������ڵ���ƥ����������
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
		if (cur_wind_index < 0)break;
		cur_dev = L1.front();

		/*�ж��ܷ���뵱ǰ����*/
		int cur_area;
		cur_wind = data.sqread_circle[line][cur_wind_index];
		bool match_cur_wind = Check_Match_back(data, cur_dev, cur_wind, cur_wind_index, 0, cur_area);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue;
		}
		int pre_wind = data.sqread_circle[line][installed_window[line][cur_dev]];
		int pre_area = installed_area[line][cur_dev];
		install_device_back(data, cur_dev, cur_area, cur_wind, cur_wind_index, pre_wind, pre_area);
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
			for (int next = 0; next < data.device_data[cur_dev].next_device.size(); next++) {
				if (data.linegraph.adjacent_matrix[cur_dev][data.device_data[cur_dev].next_device[next]->index != 2]) {
					valid_result = false;
					break;
				}
			}
		}
		if (!valid_result)//��Чƥ�䣬����ѭ��
			break;

		/*�ж��ӽڵ����Ƿ�������ڵ�*/
		for (int i = 0; i < data.device_data[cur_dev].last_device.size(); i++) {
			int son_dev = data.device_data[cur_dev].last_device[i]->index;
			/*���������豸�����ӽڵ����L1����*/
			if (data.device_data[son_dev].next_device.size() == 1)  //Ӧ�ò�����ֵ���0�����
				L1.push(son_dev);
			else {
				/*�������豸�����ж��ӽڵ����е������豸�Ƿ��Ѱ�װ*/
				if (done_lastnum[son_dev] == data.device_data[son_dev].next_device.size() - 1)
					L1.push(son_dev);
				else
					done_lastnum[son_dev]++;  //û��װ����ȵ��ţ���ȫ����װ�����ڷ�����н���ƥ��
			}
		}
	}
	return valid_result;
}


/**********************************************************************************************
�жϵ�ǰ�豸�ʹ����ܲ���ƥ�䣨�Ż��汾��
ע�⣺�����wind_index�Ǵ��ںţ�����չ���󴰿����б��
**********************************************************************************************/
bool Result::Check_Match_back(Data& data, int dev_index, int wind, int wind_index, int match_index, int& area_index) {
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
	for (int i = 0; i < data.device_data[dev_index].next_device.size(); i++) {
		int last_dev = data.device_data[dev_index].next_device[i]->index;
		if (data.linegraph.adjacent_matrix[last_dev][dev_index] != 2 &&  //����Эͬ�豸
			installed_window[match_index][last_dev] <= wind_index)  //���������豸�İ�װ����
			return false;
	}

	/*������������а�װ�ɱ���С���������ĸ�*/
	int min_cost = LONG_MAX;
	for (auto& area : data.device_data[dev_index].surport_window[wind])
	{
		int new_cost = data.device_data[dev_index].energy_install_cost[data.area_data[area].energy_type];
		if (new_cost < min_cost) {
			min_cost = new_cost;
			area_index = area;
		}
	}

	/*����Ƶ�������ڳɱ�����ˣ��Ͳ������������*/
	if (min_cost > data.device_data[dev_index].install_cost)
		return false;

	return result;
}



/**********************************************************************************************
��װ�豸������״̬�����ڻ���
**********************************************************************************************/
void Result::install_device_back(Data& data, int device_index, int area_index, int wind, int wind_index, int pre_wind_index = -1, int pre_area_index = -1) {
	//int window_index = data.area_data[area_index].window_index;
	int pre_energy_type = data.area_data[pre_area_index].energy_type;
	int energy_type = data.area_data[area_index].energy_type;

	/*ɾ��֮ǰ��װ��״̬*/
	if (pre_wind_index >= 0)
	{
		/*����״̬�ָ�*/
		auto it = find(data.window_data[pre_wind_index].already_installed_device.begin(), data.window_data[pre_wind_index].already_installed_device.end(), device_index);
		data.window_data[pre_wind_index].already_installed_device.erase(it);

		//�Ǻ����豸�Żָ�
		if (data.device_data[device_index].is_core_device)
		{
			data.window_data[pre_wind_index].process_time.clear();
			for (auto it_dev : data.window_data[pre_wind_index].already_installed_device)
			{
				if (data.device_data[it_dev].is_core_device)
					data.window_data[pre_wind_index].process_time.insert(data.device_process_time[pre_energy_type]);

			}
		}
		//����״̬�ָ�
		auto it_area = find(data.area_data[pre_area_index].already_installed_device.begin(), data.area_data[pre_area_index].already_installed_device.end(), device_index);
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
	installed_window[0][device_index] = wind_index;
	installed_area[0][device_index] = area_index;
	//ȫ��״̬����
	data.have_installed_device_num++;
}