#include "algor.h"

/**********************************************************************************************
判断当前设备和窗口能不能匹配
注意：这里的wind_index是窗口号，不是展开后窗口序列编号
**********************************************************************************************/
void Result::Algorithm(Data& data) {
	bool iter_end = false;
	int cur_dev, cur_wind, cur_wind_index = 0;
    //要检查一下这里是不是正常添加了一组空匹配，检查好后本句删除
	installed_window.push_back(vector<int>(data.device_num, 999)); 
	vector<int> done_lastnum(data.device_num, 0);     //过程变量：特殊节点已匹配的输入个数
	queue<int> L1, L2, surport_wind;
	L1.push(data.linegraph.first_device->index);  //插入头节点

	while (!iter_end) {
		if (L1.empty()) {
			L1 = L2;
			while (!L2.empty())
				L2.pop();
			cur_wind_index++;
		}
		cur_dev = L1.front();

		/*多个窗口选择*/
		//surport_wind = Choose_Window(data, cur_dev, cur_wind_index);
		//cur_wind_index = surport_wind.front();

		/*判断能否放入当前窗口*/
		cur_wind = data.sqread_circle[cur_wind_index];
		bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, 0);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue; //检查这里是不是不执行下面的，跳到下个循环了
		}
		installed_window[0][cur_dev] = cur_wind_index;  //这里只取了第一个，生成一种匹配方案
		L1.pop();
		//surport_wind.pop();

		/*判断是否是结束节点*/
		if (data.device_data[cur_dev].next_device.size() == 0) {
			if (L1.empty() && L2.empty())
				break;
			else
				continue;
		}

		/*判断子节点们是否是特殊节点*/
		for (int i = 0; i < data.device_data[cur_dev].next_device.size(); i++) {
			int son_dev = data.device_data[cur_dev].next_device[i]->index;
			/*不是特殊设备，则将子节点加入L1队列*/
			if (data.device_data[son_dev].last_device.size() == 1)  //应该不会出现等于0的情况
				L1.push(son_dev);
			else {
				/*是特殊设备，则判断子节点所有的输入设备是否都已安装*/
				if (done_lastnum[son_dev] == data.device_data[son_dev].last_device.size() - 1)
					L1.push(son_dev);
				else
					done_lastnum[son_dev]++;  //没安装完就先等着，等全部安装完了在放入队列进行匹配
			}
		}
	}
}

/*返回的是回环窗口序列的*/
queue<int> Result::Choose_Window(Data& data, int dev_indev, int cur_wind_index) {
	queue<int> results;
	return results;
}

/**********************************************************************************************
判断当前设备和窗口能不能匹配
注意：这里的wind_index是窗口号，不是展开后窗口序列编号
**********************************************************************************************/
bool Result::Check_Match(Data& data, int dev_index, int wind, int wind_index, int match_index) {
	bool result = true;
	/*检查是否核心设备，以及核心设备的支持窗口是否包含wind_index*/
	if (data.device_data[dev_index].is_core_device &&
		data.device_data[dev_index].surport_window.find(wind) ==
		data.device_data[dev_index].surport_window.end())  //指向空，没找到
		return false;

	/*检查 当前设备 与 窗口支持能源 匹配情况*/
	int flag = 0;
	for (int i = 0; i < data.device_data[dev_index].surport_energy.size(); i++) {
		if (data.window_data[wind].support_energy.find(data.device_data[dev_index].surport_energy[i]) ==
			data.window_data[wind].support_energy.end())  //指向空，没找到
			flag++;
	}
	if (flag == data.device_data[dev_index].surport_energy.size())
		return false;

	/*检查当前设备所有输入设备的窗口索引是否小于当前匹配窗口索引，协同设备可以等于*/
	for (int i = 0; i < data.device_data[dev_index].last_device.size(); i++) {
		int last_dev = data.device_data[dev_index].last_device[i]->index;
		if (data.linegraph.adjacent_matrix[last_dev][dev_index] != 2 &&  //不是协同设备
			installed_window[match_index][last_dev] >= wind_index)  //而且输入设备的安装窗口
			return false;
	}
	return result;
}