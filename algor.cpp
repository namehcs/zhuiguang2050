#include "algor.h"

/**********************************************************************************************
����ƥ���㷨
**********************************************************************************************/
void Result::Window_Algorithm(Data& data) {
	bool iter_end = false;
	int cur_dev, cur_wind, cur_wind_index = 0;
    //Ҫ���һ�������ǲ������������һ���ƥ�䣬���ú󱾾�ɾ��
	installed_window.push_back(vector<int>(data.device_num, 999)); 
	vector<int> done_lastnum(data.device_num, 0);     //���̱���������ڵ���ƥ����������
	queue<int> L1, L2, surport_wind;
	L1.push(data.linegraph.first_device->index);  //����ͷ�ڵ�

	while (!iter_end) {
		if (L1.empty()) {
			L1 = L2;
			while (!L2.empty())
				L2.pop();
			cur_wind_index++;
		}
		cur_dev = L1.front();

		/*�������ѡ��*/
		//surport_wind = Choose_Window(data, cur_dev, cur_wind_index);
		//cur_wind_index = surport_wind.front();

		/*�ж��ܷ���뵱ǰ����*/
		cur_wind = data.sqread_circle[cur_wind_index];
		bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, 0);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue; //��������ǲ��ǲ�ִ������ģ������¸�ѭ����
		}
		installed_window[0][cur_dev] = cur_wind_index;  //����ֻȡ�˵�һ��������һ��ƥ�䷽��
		L1.pop();
		//surport_wind.pop();

		/*�ж��Ƿ��ǽ����ڵ�*/
		if (data.device_data[cur_dev].next_device.size() == 0) {
			if (L1.empty() && L2.empty())
				break;
			else
				continue;
		}

		/*�ж��ӽڵ����Ƿ�������ڵ�*/
		for (int i = 0; i < data.device_data[cur_dev].next_device.size(); i++) {
			int son_dev = data.device_data[cur_dev].next_device[i]->index;
			/*���������豸�����ӽڵ����L1����*/
			if (data.device_data[son_dev].last_device.size() == 1)  //Ӧ�ò�����ֵ���0�����
				L1.push(son_dev);
			else {
				/*�������豸�����ж��ӽڵ����е������豸�Ƿ��Ѱ�װ*/
				if (done_lastnum[son_dev] == data.device_data[son_dev].last_device.size() - 1)
					L1.push(son_dev);
				else
					done_lastnum[son_dev]++;  //û��װ����ȵ��ţ���ȫ����װ�����ڷ�����н���ƥ��
			}
		}
	}
}

/**********************************************************************************************
����ƥ���㷨
**********************************************************************************************/
void Result::Area_Algorithm(Data& data) {
	int dev_wind, area, energy_type;
	vector<long> dev_install_cost(data.device_num, LONG_MAX);
	for (int i = 0; i < installed_window.size(); i++) {
		installed_area.push_back(vector<int>(data.device_num, 999));
		for (int dev = 0; dev < installed_window[i].size(); dev++) {
			dev_wind = data.sqread_circle[installed_window[i][dev]];
			/*��ÿ��������֧�ֵ������Ƿ��ܰ�װ���豸���ܰ�װ�ҷ���С����һ���͸��¸��豸�ķ���*/
			for (int j = 0; j < data.window_data[dev_wind].support_area.size(); j++) {
				area = data.window_data[dev_wind].support_area[j];
				energy_type = data.area_data[area].energy_type;
				if (data.device_data[dev].energy_install_cost[energy_type] > 0 && 
					data.device_data[dev].energy_install_cost[energy_type] < dev_install_cost[dev])
				{
					dev_install_cost[dev] = data.device_data[dev].energy_install_cost[energy_type];
					installed_area[i][dev] = area;
				}
			}
		}
		/*����һ������ƥ��İ�װ�ɱ�*/
		long dev_cost = 0;
		for (int c = 0; c < data.device_num; c++) {
			dev_cost += dev_install_cost[c];
		}
	}
}

/**********************************************************************************************
������
**********************************************************************************************/
void Result::Output(Data& data) {
	/*����������*/
	cout << data.device_num << endl;

	/*������װ�ĳ��������±������*/
	for (int i = 0; i < data.device_num - 1; i++) {
		cout << installed_area[0][i] << " ";
	}
	cout << installed_area[0][data.device_num - 1] << endl;

	/*��ˮ�ߵĲ�����*/
	cout << data.coreline.core_devices.size() << endl;

	/*��ˮ�ߵĴ����±������*/
	int core, wind;
	for (int i = 0; i < data.coreline.core_devices.size() - 1; i++) {
		core = data.coreline.core_devices[i];
		cout << data.sqread_circle[installed_window[0][core]] << " ";
	}
	core = data.coreline.core_devices[data.coreline.core_devices.size() - 1];
	cout << data.sqread_circle[installed_window[0][core]] << endl;
}

/*���ص��ǻػ��������е�*/
queue<int> Result::Choose_Window(Data& data, int dev_indev, int cur_wind_index) {
	queue<int> results;
	return results;
}

/**********************************************************************************************
�жϵ�ǰ�豸�ʹ����ܲ���ƥ��
ע�⣺�����wind_index�Ǵ��ںţ�����չ���󴰿����б��
**********************************************************************************************/
bool Result::Check_Match(Data& data, int dev_index, int wind, int wind_index, int match_index) {
	bool result = true;
	/*����Ƿ�����豸���Լ������豸��֧�ִ����Ƿ����wind_index*/
	if (data.device_data[dev_index].is_core_device &&
		data.device_data[dev_index].surport_window.find(wind) ==
		data.device_data[dev_index].surport_window.end())  //ָ��գ�û�ҵ�
		return false;

	/*��� ��ǰ�豸 �� ����֧����Դ ƥ�����*/
	int flag = 0;
	for (int i = 0; i < data.device_data[dev_index].surport_energy.size(); i++) {
		if (data.window_data[wind].support_energy.find(data.device_data[dev_index].surport_energy[i]) ==
			data.window_data[wind].support_energy.end())  //ָ��գ�û�ҵ�
			flag++;
	}
	if (flag == data.device_data[dev_index].surport_energy.size())
		return false;

	/*��鵱ǰ�豸���������豸�Ĵ��������Ƿ�С�ڵ�ǰƥ�䴰��������Эͬ�豸���Ե���*/
	for (int i = 0; i < data.device_data[dev_index].last_device.size(); i++) {
		int last_dev = data.device_data[dev_index].last_device[i]->index;
		if (data.linegraph.adjacent_matrix[last_dev][dev_index] != 2 &&  //����Эͬ�豸
			installed_window[match_index][last_dev] >= wind_index)  //���������豸�İ�װ����
			return false;
	}
	return result;
}