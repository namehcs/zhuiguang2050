#include "algor.h"


/**********************************************************************************************
迭代及优化算法
**********************************************************************************************/
void Result::Algorithm(Data& data, int line_num) {
	int res_index = 0;
	int size_sqreadline = data.sqread_circle.size();
	line_num = std::min(line_num, size_sqreadline);
	for (int line = 0; line < line_num; line++) {
		installed_window.push_back(vector<int>(data.device_num, 999)); 
		installed_area.push_back(vector<int>(data.device_num, 999));
		bool flag_res = Install_Match(data, line, res_index);
		if (flag_res) {
			Output(data);
			//计算这个res_index的代价，并选择代价最小的匹配结果
		}
	}

}


/**********************************************************************************************
窗口匹配算法
**********************************************************************************************/
bool Result::Install_Match(Data& data, int line, int res) {
	bool valid_result = true;
	int cur_dev, cur_wind, cur_wind_index = 0;
	vector<int> done_lastnum(data.device_num, 0);     //过程变量：特殊节点已匹配的输入个数
	queue<int> L1, L2;

    /*插入头节点*/
	for(int f = 0; f < data.linegraph.first_device.size(); f++)
		L1.push(data.linegraph.first_device[f]->index); 

	while (1) {
		if (L1.empty()) {
			L1 = L2;
			while (!L2.empty())
				L2.pop();
			cur_wind_index++;
		}
		cur_dev = L1.front();

		/*判断能否放入当前窗口*/
		cur_wind = data.sqread_circle[cur_wind_index];
		bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, 0);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue;
		}
		int cur_area = Area_Match(data, cur_dev, cur_wind);
		install_device(data, cur_dev, cur_area, cur_wind_index);  //这里只取了第一个，生成一种匹配方案
		L1.pop();

		/*判断是否是结束节点*/
		if (data.device_data[cur_dev].next_device.size() == 0) {
			if (L1.empty() && L2.empty())
				break;
			else
				continue;
		}

		/*判断是否是无效窗口序列，不能放下所有设备*/
		/*后面还有节点，但是已经到了最后一个窗口，如果前后不是协同关系的话就是无效匹配*/
		if (cur_wind_index == data.sqread_circle.size() - 1){
			for (int next = 0; next < data.device_data[cur_dev].next_device.size(); next++) {
				if (data.linegraph.adjacent_matrix[cur_dev][data.device_data[cur_dev].next_device[next]->index != 2]) {
					valid_result = false;
					break;
				}
			}
		}
		if (!valid_result)//无效匹配，跳出循环
			break;

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
	return valid_result;
}


/**********************************************************************************************
区域匹配算法
**********************************************************************************************/
int Result::Area_Match(Data& data, int dev_index, int wind) {
	int match_area;
	long dev_cost = LONG_MAX;
	/*看每个窗口里支持的区域是否能安装该设备，能安装且费用小于上一个就更新该设备的费用*/
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
安装设备，更新状态
**********************************************************************************************/
void Result::install_device(Data& data, int device_index, int area_index, int wind_index) {
	int window_index = data.area_data[area_index].window_index;
	int energy_type = data.area_data[area_index].energy_type;
	//窗口状态更新
	data.window_data[window_index].already_installed_device.emplace_back(device_index);
	data.window_data[window_index].process_time = max(data.window_data[window_index].process_time, data.device_process_time[energy_type]);
	data.window_data[window_index].in_times++;
	//区域状态更新
	data.area_data[area_index].already_installed_device.emplace_back(device_index);
	//设备状态更新
	data.device_data[device_index].installed_area = area_index;
	data.device_data[device_index].install_cost = data.device_data[device_index].energy_install_cost[energy_type];
	//方案状态更新
	installed_window[0][device_index] = wind_index;
	installed_area[0][device_index] = area_index;
	//全局状态更新
	data.have_installed_device_num--;
}


/**********************************************************************************************
结果输出
**********************************************************************************************/
void Result::Output(Data& data) {
	/*仪器的数量*/
	cout << data.device_num << endl;

	/*仪器安装的车间区域下标的数组*/
	for (int i = 0; i < data.device_num; i++) {
		cout << installed_area[0][i] << " ";
	}
	cout << endl;

	/*流水线的步骤数*/
	cout << data.coreline.core_devices.size() << endl;

	/*流水线的窗口下标的数组*/
	int core, wind;
	for (int i = 0; i < data.coreline.core_devices.size(); i++) {
		core = data.coreline.core_devices[i];
		cout << data.sqread_circle[installed_window[0][core]] << " ";
	}
	cout << endl;
}


/**********************************************************************************************
判断当前设备和窗口能不能匹配
注意：这里的wind_index是窗口号，不是展开后窗口序列编号
**********************************************************************************************/
bool Result::Check_Match(Data& data, int dev_index, int wind, int wind_index, int match_index) {
	bool result = true;
	/*检查是否有支持窗口，如果有是否有匹配区域*/
	if (data.device_data[dev_index].surport_window.find(wind) ==
		data.device_data[dev_index].surport_window.end())
		return false;
	else {
		if(data.device_data[dev_index].surport_window[wind].size() == 0)
			return false;
	}

	/*检查当前设备所有输入设备的窗口索引是否小于当前匹配窗口索引，协同设备可以等于*/
	for (int i = 0; i < data.device_data[dev_index].last_device.size(); i++) {
		int last_dev = data.device_data[dev_index].last_device[i]->index;
		if (data.linegraph.adjacent_matrix[last_dev][dev_index] != 2 &&  //不是协同设备
			installed_window[match_index][last_dev] >= wind_index)  //而且输入设备的安装窗口
			return false;
	}
	return result;
}


///*返回的是回环窗口序列的*/
//queue<int> Result::Choose_Window(Data& data, int dev_indev, int cur_wind_index) {
//	queue<int> results;
//	return results;
//}
