#include "algor.h"
//#include <windows.h>


void Result::State_Clear(Data& data, bool clear_all) {
	if (clear_all) {//全部清理
		for (auto& wind : data.window_data) {
			//后补
		}
		for (auto& area : data.area_data) {}
		for (auto& device : data.device_data) {
			device.done_lastnum = 0;
		}
		data.have_installed_device_num = 0;
	}
	else {//部分清理
		for (auto& wind : data.window_data) {

		}
	}
}


//void Result::Sort_Device(Data& data, int line) {
//	/*比较每个核心设备优化代价的大小，从大到小排序*/
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
	/*生成前向匹配，注意这里先要把序列放进data.sqread_circle才能进行后续操作*/
	/*插入第一个初始化匹配*/
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
	/*回溯从后往前调整核心窗口匹配，插入第一个初始化匹配*/
	installed_window.push_back(installed_window[result_index]);
	installed_area.push_back(installed_area[result_index]);
	result_index++;
	State_Clear(data, false);
	backward_valid = Install_Match_back(data, result_index);
	if (backward_valid) {
		backward_cost = Get_Cost(data);
		match_costs.push_back(backward_cost);
		/*如果回溯代价小于最低代价，则更新最优匹配*/
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
迭代及优化算法
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
安装匹配算法
**********************************************************************************************/
bool Result::Small_Optim_Match(Data& data, int line, int father_dev, int start_wind_index) {
	bool valid_result = true;
	int cur_wind, cur_area, cur_wind_index;
	for (auto& son_dev : data.device_data[father_dev].next_device) {
		/*确定调整范围，从开始设备及窗口序号，到结束设备及窗口序号*/
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

		/*开始前向调整*/
		cur_dev = start_dev;
		cur_wind_index = start_wind_index;
		while (1) {
			valid_result = true;
			/*判断能否放入当前窗口*/
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

			/*判断是否是结束节点*/
			if (cur_dev == end_dev)
				break;

			/*判断是否是无效窗口序列，不能放下所有设备*/
			/*后面还有节点，但是已经到了最后一个窗口，如果前后不是协同关系的话就是无效匹配*/
			if (cur_wind_index == end_wind_index) {
				if (data.linegraph.adjacent_matrix[cur_dev][data.device_data[cur_dev].next_device[0]->index != 2]) {
					valid_result = false;
					break;
				}
			}
			if (!valid_result)//无效匹配，跳出循环
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
	/*插入头节点*/
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

		/*判断是否是结束节点*/
		if (data.device_data[cur_dev].next_device.size() == 0) {
			if (L1.empty() && L2.empty())
				break;
			else
				continue;
		}

		/*判断是否是无效窗口序列，不能放下所有设备*/
		/*后面还有节点，但是已经到了最后一个窗口，如果前后不是协同关系的话就是无效匹配*/
		if (cur_wind_index == data.sqread_circle.size() - 1) {
			for (auto& next : data.device_data[cur_dev].next_device) {
				if (data.linegraph.adjacent_matrix[cur_dev][next->index != 2]) {
					valid_result = false;
					break;
				}
			}
		}
		if (!valid_result)//无效匹配，跳出循环
			break;

		/*判断子节点们是否是特殊节点*/
		for (auto& son_dev : data.device_data[cur_dev].next_device) {
			/*不是特殊设备，则将子节点加入L1队列*/
			if (son_dev->last_device.size() == 1)  //应该不会出现等于0的情况
				L1.push(son_dev->index);
			else {
				/*是特殊设备，则判断子节点所有的输入设备是否都已安装*/
				if (son_dev->done_lastnum == son_dev->last_device.size() - 1)
					L1.push(son_dev->index);
				else
					son_dev->done_lastnum++;  //没安装完就先等着，等全部安装完了在放入队列进行匹配
			}
		}
	}

	return valid_result;
}


/**********************************************************************************************
安装设备，更新状态
**********************************************************************************************/
void Result::install_device(Data& data, int device_index, int area_index, int wind, int line, int wind_index) {
	int energy_type = data.area_data[area_index].energy_type;

	//窗口状态更新
	if (data.device_data[device_index].is_core_device) {
		data.window_data[wind].already_installed_core.insert({ device_index,
			{area_index, data.device_process_time[energy_type]} });  //插入设备号，区域号和加工时间

		//如果和他相连的之前的某个核心设备和他是协同设备，那这个窗口中的协同数加一
		for (auto& dev_last : data.device_data[device_index].last_device) {
			if (data.window_data[wind].already_installed_core.find(dev_last->index) !=
				data.window_data[wind].already_installed_core.end()) {
				if (data.linegraph.adjacent_matrix[dev_last->index][device_index] == 2)
					data.window_data[wind].in_times++;//协同设备数加1
			}
		}
	}
	else
		data.window_data[wind].already_installed_normall.insert({ device_index, area_index });

	//区域状态更新
	data.area_data[area_index].already_installed_device.push_back(device_index);
	//设备状态更新
	data.device_data[device_index].installed_area = area_index;
	//方案状态更新
	installed_window[line][device_index] = wind_index;
	installed_area[line][device_index] = area_index;
	//全局状态更新
	data.have_installed_device_num++;
}


long Result::Get_Cost(Data& data) {
	long install_cost = 0;
	long preprocess_cost = 0;
	long window_match_cost = 0;
	for (auto& device : data.device_data) {
		install_cost += device.install_cost;
	}
	for (auto& window : data.window_data) {  //按最后面的最大值计算加工时间
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
结果输出
**********************************************************************************************/
void Result::Output(Data& data, int line) {
	/*仪器的数量*/
	cout << data.device_num << endl;
	line = installed_area.size() - 1;
	/*仪器安装的车间区域下标的数组*/
	for (int i = 0; i < data.device_num; i++) {
		cout << installed_area[line][i] << " ";
	}
	cout << endl;

	/*流水线的步骤数*/
	cout << data.coreline.core_devices.size() << endl;

	/*流水线的窗口下标的数组*/
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

	//是非核心设备只有安装费用
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
		//获取当前窗口的加工时间
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

			//运行代价
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
判断当前设备和窗口能不能匹配
注意：这里的wind_index是窗口号，不是展开后窗口序列编号
**********************************************************************************************/
bool Result::Check_Match(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index) {
	bool result = true;
	/*检查是否有支持窗口，如果有是否有匹配区域*/
	if (data.device_data[dev_index].surport_window.find(wind) ==
		data.device_data[dev_index].surport_window.end())
		return false;
	else {
		if (data.device_data[dev_index].surport_window[wind].size() == 0)
			return false;
	}

	/*检查当前设备所有输入设备的窗口索引是否小于当前匹配窗口索引，协同设备可以等于*/
	for (auto& last_dev : data.device_data[dev_index].last_device) {
		if (data.linegraph.adjacent_matrix[last_dev->index][dev_index] != 2 &&  //不是协同设备
			installed_window[line][last_dev->index] >= wind_index)  //而且输入设备的安装窗口
			return false;
	}

	long dev_cost = longmax;
	/*遍历这个窗口中安装成本最小的区域是哪个*/
	int area = -1;
	int cost = Install_Cost(data, dev_index, wind_index, area);

	/*如果移到这个窗口成本变高了，就不放在这个窗口，这里分两种情况：第一个前向和后续调优的前向*/
	if (cost >= data.device_data[dev_index].install_cost)
		return false;

	data.device_data[dev_index].install_cost = cost;
	area_index = area;

	return result;
}

/**********************************************************************************************
窗口匹配算法 基于回溯
**********************************************************************************************/
bool Result::Install_Match_back(Data& data, int line) {
	bool valid_result = true;
	int cur_dev, start_wind_index, cur_wind = 0;
	int cur_wind_index = data.sqread_circle.size() - 1;
	queue<int> L1, L2;//设备号，可以开始搜索的窗口号
	unordered_set<int> L3; //可以调整的设备号，同一时期互不影响
	int start_optim_wind;

	/*插入尾部节点*/
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

		/*局部前向调整*/
		for (auto& son_dev : data.device_data[cur_dev].last_device) {
			if (son_dev->next_device.size() > 1 && L3.find(son_dev->index) == L3.end()) {
				L3.insert(son_dev->index);
				start_optim_wind = installed_window[0][son_dev->index];
				Small_Optim_Match(data, line, son_dev->index, start_optim_wind);
			}
		}

		/*判断能否放入当前窗口*/
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

		/*判断是否是结束节点*/
		if (data.device_data[cur_dev].last_device.size() == 0) {
			if (L1.empty() && L2.empty())
				break;
			else
				continue;
		}

		/*判断是否是无效窗口序列，不能放下所有设备*/
		/*后面还有节点，但是已经到了最后一个窗口，如果前后不是协同关系的话就是无效匹配*/
		if (cur_wind_index == 0) {
			for (auto& last : data.device_data[cur_dev].last_device) {
				if (data.linegraph.adjacent_matrix[last->index][cur_dev] != 2) {
					valid_result = false;
					break;
				}
			}
		}
		if (!valid_result)//无效匹配，跳出循环
			break;

		/*判断子节点们是否是特殊节点*/
		for (auto& son_dev : data.device_data[cur_dev].last_device) {
			/*不是特殊设备，则将子节点加入L1队列*/
			if (son_dev->next_device.size() == 1)  //应该不会出现等于0的情况
				L1.push(son_dev->index);
			else {
				/*是特殊设备，则判断子节点所有的输入设备是否都已安装*/
				if (son_dev->done_lastnum == son_dev->next_device.size() - 1)
					L1.push(son_dev->index);
				else
					son_dev->done_lastnum++;  //没安装完就先等着，等全部安装完了在放入队列进行匹配
			}
		}
	}
	return valid_result;
}

/**********************************************************************************************
判断当前设备和窗口能不能匹配（优化版本）
注意：这里的wind_index是窗口号，不是展开后窗口序列编号
**********************************************************************************************/
bool Result::Check_Match_back(Data& data, int dev_index, int wind, int wind_index, int line, int& area_index) {
	bool result = true;
	/*检查是否有支持窗口，如果有是否有匹配区域*/
	if (data.device_data[dev_index].surport_window.find(wind) ==
		data.device_data[dev_index].surport_window.end())
		return false;
	else {
		if (data.device_data[dev_index].surport_window[wind].size() == 0)
			return false;
	}

	/*检查当前设备所有输入设备的窗口索引是否小于当前匹配窗口索引，协同设备可以等于*/
	for (auto& next_dev : data.device_data[dev_index].next_device) {
		if (data.linegraph.adjacent_matrix[next_dev->index][dev_index] != 2 &&  //不是协同设备
			installed_window[line][next_dev->index] <= wind_index)  //而且输入设备的安装窗口
			return false;
	}

	/*遍历这个窗口中安装成本最小的区域是哪个*/
	int area = -1;
	int cost = Install_Cost(data, dev_index, wind_index, area);

	/*如果移到这个窗口成本变高了，就不放在这个窗口*/
	if (cost > data.device_data[dev_index].install_cost)
		return false;

	data.device_data[dev_index].install_cost = cost;
	area_index = area;
	//cout << dev_index << ": " << cost << endl;
	return result;
}

/**********************************************************************************************
安装设备，更新状态，基于回溯
**********************************************************************************************/
void Result::install_device_back(Data& data, int device_index, int area_index, int wind,
	int wind_index, int line, int pre_wind_index, int pre_area_index) {

	//int window_index = data.area_data[area_index].window_index;
	int pre_energy_type = data.area_data[pre_area_index].energy_type;
	int energy_type = data.area_data[area_index].energy_type;

	/*删除之前安装的状态*/
	if (pre_wind_index >= 0)
	{
		if (data.device_data[device_index].is_core_device) {
			//删除安装设备
			data.window_data[pre_wind_index].already_installed_core.erase(device_index);
			//删除协同数
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

		//区域状态恢复
		auto it_area = find(data.area_data[pre_area_index].already_installed_device.begin(),
			data.area_data[pre_area_index].already_installed_device.end(), device_index);
		data.area_data[pre_area_index].already_installed_device.erase(it_area);
	}

	//窗口状态更新
	if (data.device_data[device_index].is_core_device) {
		data.window_data[wind].already_installed_core.insert({ device_index , {wind_index, area_index} });
	}
	else
	{
		data.window_data[wind].already_installed_normall.insert({ device_index , area_index });
	}

	//更新协同数
	if (data.device_data[device_index].is_core_device) {
		for (auto& it_dev : data.window_data[wind].already_installed_core) {
			if (data.linegraph.adjacent_matrix[it_dev.first][device_index] == 2 ||
				data.linegraph.adjacent_matrix[device_index][it_dev.first] == 2)
				data.window_data[wind].in_times++;
		}
	}

	//区域状态更新
	data.area_data[area_index].already_installed_device.emplace_back(device_index);
	//设备状态更新
	data.device_data[device_index].installed_area = area_index;
	data.device_data[device_index].install_cost = data.device_data[device_index].energy_install_cost[energy_type];
	//方案状态更新
	installed_window[line][device_index] = wind_index;
	installed_area[line][device_index] = area_index;
}
