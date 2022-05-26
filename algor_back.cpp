/**********************************************************************************************
����ƥ���㷨
**********************************************************************************************/
bool Result::Install_Match_back(Data& data, int line, int res) {
	bool valid_result = true;
	int cur_dev, cur_wind = 0;
	int cur_wind_index = data.sqread_circle.size() - 1;
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
		int pre_wind = installed_window[line][cur_dev];
		int pre_area = installed_area[line][cur_dev];
		install_device(data, cur_dev, cur_area, cur_wind_index, pre_wind, pre_area);
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
	int min_cost = INT_MAX;
	for (auto& area : data.device_data[dev_index].surport_window[wind_index])
	{
		if (data.device_data[dev_index].energy_install_cost[area[i]] < min_cost) {
			min_cost = data.device_data[dev_index].energy_install_cost[area[i]];
			area_index = area[i];
		}
	}

	/*����Ƶ�������ڳɱ�����ˣ��Ͳ������������*/
	if (min_cost > data.device_data[dev_index].install_cost)
		return false;

	return result;
}





/**********************************************************************************************
��װ�豸������״̬
**********************************************************************************************/
void Result::install_device(Data& data, int device_index, int area_index, int wind_index, int pre_wind_index = -1, int pre_area_index = -1) {
	int window_index = data.area_data[area_index].window_index;
	int energy_type = data.area_data[area_index].energy_type;

	/*ɾ��֮ǰ��װ��״̬*/
	if (pre_wind_index >= 0)
	{
		/*����״̬�ָ�*/
		auto& it = data.window_data[pre_wind_index].already_installed_device.find(device_index)
			data.window_data[pre_wind_index].already_installed_device.erase(it);
		data.window_data[pre_wind_index].process_time = 0;
		for (auto& area : data.window_data[pre_wind_index].support_area)
		{
			if (data.area_data[area].already_installed_device.size() > 0)
			{
				data.window_data[pre_wind_index].process_time = max(data.window_data[window_index].process_time, data.device_process_time[data.area_data[area].energy_type]);
			}
		}
		//����״̬�ָ�
		auto& it1 = data.area_data[pre_area_index].already_installed_device.find(device_index);
		data.area_data[pre_area_index].already_installed_device.erase(it1);
		//ȫ��״̬�ָ�
		data.have_installed_device_num--;
	}

	//����״̬����
	data.window_data[window_index].already_installed_device.emplace_back(device_index);
	data.window_data[window_index].process_time = max(data.window_data[window_index].process_time, data.device_process_time[energy_type]);
	data.window_data[window_index].in_times++;
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