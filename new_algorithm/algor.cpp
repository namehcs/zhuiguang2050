#include "algor.h"
//#include <windows.h>



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
	/*回溯从后往前调整核心窗口匹配，插入第一个初始化匹配*/
	//installed_window.push_back(installed_window[result_index]);
	//installed_area.push_back(installed_area[result_index]);
	installed_window.push_back(vector<int>(data.device_num, 999));
	installed_area.push_back(vector<int>(data.device_num, 999));
	result_index++;
	backward_valid = Install_Match_back(data, result_index);
	if (backward_valid) {
		backward_cost = Get_Cost(data, result_index);
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

void Result::fff(Data& data, int& bestline) {
	long fffward_cost;
	bool fffward_valid;

	/*install[2]为测试，install[3]为最优*/
	installed_window.push_back(installed_window[0]);
	installed_area.push_back(installed_area[0]);
	match_costs.push_back(match_costs[0]);

	installed_window.push_back(installed_window[0]);
	installed_area.push_back(installed_area[0]);
	match_costs.push_back(match_costs[0]);

	fffward_valid = Install_Match_fff(data);
	fffward_cost = Get_Cost(data, result_index);

	/*如果回溯代价小于最低代价，则更新最优匹配*/
	bestline = result_index;
}


/**********************************************************************************************
迭代及优化算法
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
	/*遍历这个窗口中安装成本最小的区域是哪个*/
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
安装匹配算法
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
		if (p == data.device_data[cur_dev].next_device.size()) {//没影响后面的设备，不需要匹配，直接改
			installed_window[2][cur_dev] = i;
			int now_area;
			find_area(data, cur_dev, data.sqread_circle[i], now_area);
			installed_area[2][cur_dev] = now_area;
			int cost_1 = Get_Cost(data, 2); //小改测试
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
					if (done_lastnum[son_dev->index] == son_dev->last_device.size() - 1)
						L1.push(son_dev->index);
					else
						done_lastnum[son_dev->index]++;  //没安装完就先等着，等全部安装完了在放入队列进行匹配
				}
			}
		}
		int cost_2 = Get_Cost(data, 2); //小改测试
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

		/*局部前向调整*/
		if (installed_window[1][cur_dev] - installed_window[0][cur_dev] > 0) {
			Small_Optim_Match(data, L1, L2, done_lastnum);
		}

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
				if (done_lastnum[son_dev->index] == son_dev->last_device.size() - 1)
					L1.push(son_dev->index);
				else
					done_lastnum[son_dev->index]++;  //没安装完就先等着，等全部安装完了在放入队列进行匹配
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
		install_device(data, cur_dev, cur_area, cur_wind, cur_wind_index, line);
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
				if (done_lastnum[son_dev->index] == son_dev->last_device.size() - 1)
					L1.push(son_dev->index);
				else
					done_lastnum[son_dev->index]++;  //没安装完就先等着，等全部安装完了在放入队列进行匹配
			}
		}
	}

	return valid_result;
}


/**********************************************************************************************
安装设备，更新状态
**********************************************************************************************/
void Result::install_device(Data& data, int device_index, int area_index, int wind, int wind_index, int line) {

	//设备状态更新
	data.device_data[device_index].installed_area = area_index;
	//方案状态更新
	installed_window[line][device_index] = wind_index;
	installed_area[line][device_index] = area_index;
}

long Result::Get_Cost(Data& data, int line) {
	long install_cost = 0;
	long preprocess_cost = 0;
	long window_match_cost = 0;
	vector<vector<int>> winds(data.window_num, vector<int>(2, 0)); //{最大加工时间, 进入次数}
	for (int i = 0; i < data.device_num; i++) {
		install_cost += data.device_data[i].energy_install_cost[data.area_data[installed_area[line][i]].energy_type];
	}
	for (int i = 0; i < data.device_num; i++) {
		int w = data.sqread_circle[installed_window[line][i]];
		if(data.device_data[i].is_core_device){
			winds[w][0] = max(winds[w][0], data.device_process_time[
				data.area_data[installed_area[line][i]].energy_type]);//最大加工时间
			if (data.device_data[i].last_coredev &&
				installed_window[line][data.device_data[i].last_coredev->index]
				== installed_window[line][i] &&  //这里不能是窗口号，只能是窗口序列
				data.linegraph.adjacent_matrix[data.device_data[i].
				last_coredev->index][i] == 2)
				continue;
			winds[w][1]++;
		}
	}
	for (auto& window : data.window_data) {  //按最后面的最大值计算加工时间
		preprocess_cost += window.cost_coefficient * winds[window.index][0];
		window_match_cost += winds[window.index][0] * winds[window.index][1] * data.coreline.production_times;
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
	if (forward) {
		for (auto& last_dev : data.device_data[dev_index].last_device) {
			if (data.linegraph.adjacent_matrix[last_dev->index][dev_index] != 2 &&  //不是协同设备
				installed_window[line][last_dev->index] >= wind_index)  //而且输入设备的安装窗口
				return false;
		}
	}
	if(!forward) {
		for (auto& next_dev : data.device_data[dev_index].next_device) {
			if (data.linegraph.adjacent_matrix[dev_index][next_dev->index] != 2 &&  //不是协同设备  这里协同设备没用上
				installed_window[line][next_dev->index] <= wind_index)  //而且输入设备的安装窗口
				return false;
		}
	}

	/*遍历这个窗口中安装成本最小的区域是哪个*/
	long cost = find_area(data, dev_index, wind, area_index);
	data.device_data[dev_index].install_cost = cost;

	return result;
}

/**********************************************************************************************
窗口匹配算法 基于回溯
**********************************************************************************************/
bool Result::Install_Match_back(Data& data, int line) {
	bool valid_result = true;
	queue<int> L1, L2;//设备号，可以开始搜索的窗口号
	vector<int> done_lastnum(data.device_num, 0);
	int cur_dev, start_wind_index, cur_wind = 0;
	int cur_wind_index = data.sqread_circle.size() - 1;
	

	/*插入尾部节点*/
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

		///*局部前向调整*/

		/*判断能否放入当前窗口*/
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
				if (done_lastnum[son_dev->index] == son_dev->next_device.size() - 1)
					L1.push(son_dev->index);
				else
					done_lastnum[son_dev->index]++;  //没安装完就先等着，等全部安装完了在放入队列进行匹配
			}
		}
	}
	return valid_result;
}

