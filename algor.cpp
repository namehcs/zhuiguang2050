#include "algor.h"

/**********************************************************************************************
�жϵ�ǰ�豸�ʹ����ܲ���ƥ��
ע�⣺�����wind_index�Ǵ��ںţ�����չ���󴰿����б��
**********************************************************************************************/
void Result::Algorithm(Data& data) {
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