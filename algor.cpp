#include "algor.h"


void State_Clear(Data& data) {
	for (auto& wind : data.window_data){
		wind.already_installed_device.clear();
		wind.process_time.clear();
		wind.in_times = 0;
	}
	for (auto& area : data.area_data)
		area.already_installed_device.clear();
	for (auto& device : data.device_data) {
		device.install_cost = 0;
	}
	data.have_installed_device_num = 0;
}

/**********************************************************************************************
迭代及优化算法
**********************************************************************************************/
void Result::Algorithm(Data& data) {
	int bestline = 0, line = 0;
	vector<vector<int>> unhook_window; //改为可拆环的window，自己排序，不一定要用unordered_map

	/*生成全部展开窗口序列的匹配*/
	/*插入第一个初始化匹配*/
	installed_window.push_back(vector<int>(data.device_num, 999));
	installed_area.push_back(vector<int>(data.device_num, 999));
	bool flag_res1 = Install_Match(data, line);
	long forward_cost = Get_Cost(data);
	match_costs.push_back(forward_cost);

	/*回溯从后往前调整核心窗口匹配*/
	/*插入第一个初始化匹配*/
	line++;
	data.sqread_circle.push_back(data.sqread_circle[line - 1]);
	installed_window.push_back(installed_window[line - 1]);
	installed_area.push_back(installed_area[line - 1]);
	bool flag_res2 = Install_Match_back(data, line);
	long backtrace_cost = Get_Cost(data);
	match_costs.push_back(backtrace_cost);

	/*如果回溯代价小于前向代价，则更新最优匹配*/
	if (flag_res1 && flag_res2 && backtrace_cost < forward_cost) {
		bestline = line;
	}


	State_Clear(data);
	if (flag_res1) {
		/*比较每窗口中安装设备的最大加工时间和最小加工时间差值，该值越大拆环后加工时间减小越多，代价越小*/
		for (auto& wind : data.window_data) {
			if (wind.self_loop){
				vector<int> one;
				if (wind.process_time.size() > 0) {
					set<int>::iterator end = wind.process_time.end(); end--;
					one.push_back((*end) - (*wind.process_time.begin()));
					one.push_back(wind.index);
				}
				else {
					one.push_back(0);
					one.push_back(wind.index);
				}
				unhook_window.push_back(one);
			}
		}
		/*加工时间差值已经按从小到大的顺序存放在map里面了，key为差值，value为窗口号*/
		/*拆环顺序：
		1、比较自回环最大次数和第一类环回范围
		2、如果回环次数小于第一类回环范围：先拆自回环中差值最大的，从大到小(>0,0没必要拆)
		*/
		int unhook_self_circle = 1;
		if (data.max_loop_num <= data.first_loop_window_num) {
			/*拆自回环*/
			for (auto& wind : unhook_window) {
				Unhook(data, wind[1], bestline);
				
			}
		}
	}
	Output(data, bestline);
	//cout << match_costs[0] << endl;
	//计算这个res_index的代价，并选择代价最小的匹配结果
}


/**********************************************************************************************
拆环后的优化匹配算法
找到代价更低的匹配就把
**********************************************************************************************/
void Result::Unhook(Data& data, int wind, int& best_line) {
	int startline = data.sqread_circle.size() - 1, start_wind_index = 0;
	vector<int> unhook_line = data.sqread_circle[0];
	/*先根据拆环窗口号生成窗口序列*/
	for (int i = 1; i <= data.max_loop_num; i++) {
		for (vector<int>::iterator it = unhook_line.begin() + 1; it != unhook_line.end(); it++) {
			if (wind == (*it) && wind == (*(it-1))) {  //如果有连续两个都等于wind，就删去后面一个
				unhook_line.erase(it);
				data.sqread_circle.push_back(unhook_line);
				startline++;
				break;
			}
		}
		if (data.sqread_circle.size()-1 == installed_window.size() &&
			data.sqread_circle.size()-1 == installed_area.size()) {
			installed_window.push_back(vector<int>(data.device_num, 999));
			installed_area.push_back(vector<int>(data.device_num, 999));
			bool flag_res = Install_Match(data, startline);
			
			if (flag_res) {
				long one_cost = Get_Cost(data);
				if (one_cost < match_costs[best_line]) {
					match_costs.push_back(one_cost);
					State_Clear(data);
					best_line = startline;
				}
				else {
					installed_window.pop_back();
					installed_area.pop_back();
					data.sqread_circle.pop_back();
					startline--;
					State_Clear(data);
				}
			}
			else {
				installed_window.pop_back();
				installed_area.pop_back();
				data.sqread_circle.pop_back();
				startline--;
				State_Clear(data);
			}
		}

	}
}


/**********************************************************************************************
安装匹配算法
**********************************************************************************************/
bool Result::Install_Match(Data& data, int line) {
	bool valid_result = true;
	int cur_dev, cur_wind, cur_wind_index = 0;
	vector<int> done_lastnum(data.device_num, 0);     //过程变量：特殊节点已匹配的输入个数
	queue<int> L1, L2;
	/*插入头节点*/
	for (auto& f : data.linegraph.first_device)
		L1.push(f->index);
	while (1) {
		if (L1.empty()) {
			L1 = L2;
			while (!L2.empty())
				L2.pop();
			cur_wind_index++;
		}
		cur_dev = L1.front();

		/*判断能否放入当前窗口*/
		int cur_area;
		cur_wind = data.sqread_circle[line][cur_wind_index];
		bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, line, cur_area);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue;
		}
		install_device(data, cur_dev, cur_area, cur_wind, line, cur_wind_index);  //这里只取了第一个，生成一种匹配方案
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
		if (cur_wind_index == data.sqread_circle[line].size() - 1){
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
void Result::install_device(Data& data, int device_index, int area_index, int wind, int line, int wind_index) {

	int energy_type = data.area_data[area_index].energy_type;
	//窗口状态更新
	data.window_data[wind].already_installed_device.push_back(device_index);
	//这个设备是核心设备才计算加工时间，process_time改成set，是核心设备就往里push
	if (data.device_data[device_index].is_core_device) {
		data.window_data[wind].process_time.insert(data.device_process_time[energy_type]);
		if(data.device_data[device_index].last_device.size() == 0) //核心头结点的次数要加进去
			data.window_data[wind].in_times++;
		for (auto& dev_last : data.device_data[device_index].last_device) {
			if (dev_last->is_core_device && data.linegraph.adjacent_matrix[dev_last->index][device_index] != 2)
				data.window_data[wind].in_times++;//这个设备和上个设备是核心设备，且两个设备不是协同设备
		}
	}
	//区域状态更新
	data.area_data[area_index].already_installed_device.push_back(device_index);
	//设备状态更新
	data.device_data[device_index].installed_area = area_index;
	data.device_data[device_index].install_cost = data.device_data[device_index].energy_install_cost[energy_type];
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
结果输出
**********************************************************************************************/
void Result::Output(Data& data, int line) {
	/*仪器的数量*/
	cout << data.device_num << endl;

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
		cout << data.sqread_circle[line][installed_window[line][core]] << " ";
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
		if(data.device_data[dev_index].surport_window[wind].size() == 0)
			return false;
	}

	/*检查当前设备所有输入设备的窗口索引是否小于当前匹配窗口索引，协同设备可以等于*/
	for (auto& last_dev : data.device_data[dev_index].last_device) {
		if (data.linegraph.adjacent_matrix[last_dev->index][dev_index] != 2 &&  //不是协同设备
			installed_window[line][last_dev->index] >= wind_index)  //而且输入设备的安装窗口
			return false;
	}

	long dev_cost = longmax;
	/*看每个窗口里支持的区域是否能安装该设备，能安装且费用小于上一个就更新该设备的费用*/
	for (auto& area : data.window_data[wind].support_area) {
		int energy_type = data.area_data[area].energy_type;
		if (data.device_data[dev_index].energy_install_cost[energy_type] > 0 &&
			data.device_data[dev_index].energy_install_cost[energy_type] < dev_cost) {
			dev_cost = data.device_data[dev_index].energy_install_cost[energy_type];
			area_index = area;
		}
	}
	return result;
}


///*返回的是回环窗口序列的*/
//queue<int> Result::Choose_Window(Data& data, int dev_indev, int cur_wind_index) {
//	queue<int> results;
//	return results;
//}


/**********************************************************************************************
窗口匹配算法 基于回溯
**********************************************************************************************/
bool Result::Install_Match_back(Data& data, int line) {
	bool valid_result = true;
	int cur_dev, cur_wind = 0;
	int cur_wind_index = data.sqread_circle[line].size() - 1;
	vector<int> done_lastnum(data.device_num, 0);     //过程变量：特殊节点已匹配的输入个数
	queue<int> L1, L2;

	/*插入尾部节点*/
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

		/*判断能否放入当前窗口*/
		int cur_area;
		cur_wind = data.sqread_circle[line][cur_wind_index];
		bool match_cur_wind = Check_Match_back(data, cur_dev, cur_wind, cur_wind_index, line, cur_area);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue;
		}
		int pre_wind = data.sqread_circle[line][installed_window[line][cur_dev]];
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
		for (int i = 0; i < data.device_data[cur_dev].last_device.size(); i++) {
			int son_dev = data.device_data[cur_dev].last_device[i]->index;
			/*不是特殊设备，则将子节点加入L1队列*/
			if (data.device_data[son_dev].next_device.size() == 1)  //应该不会出现等于0的情况
				L1.push(son_dev);
			else {
				/*是特殊设备，则判断子节点所有的输入设备是否都已安装*/
				if (done_lastnum[son_dev] == data.device_data[son_dev].next_device.size() - 1)
					L1.push(son_dev);
				else
					done_lastnum[son_dev]++;  //没安装完就先等着，等全部安装完了在放入队列进行匹配
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
	int min_cost = longmax;
	for (auto& area : data.device_data[dev_index].surport_window[wind])
	{
		int new_cost = data.device_data[dev_index].energy_install_cost[data.area_data[area].energy_type];
		if (new_cost < min_cost) {
			min_cost = new_cost;
			area_index = area;
		}
	}

	/*如果移到这个窗口成本变高了，就不放在这个窗口*/
	if (min_cost > data.device_data[dev_index].install_cost)
		return false;

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
		/*窗口状态恢复*/
		auto it = find(data.window_data[pre_wind_index].already_installed_device.begin(), 
			data.window_data[pre_wind_index].already_installed_device.end(), device_index);
		data.window_data[pre_wind_index].already_installed_device.erase(it);

		//是核心设备才恢复
		if (data.device_data[device_index].is_core_device) {
			data.window_data[pre_wind_index].process_time.clear();
			for (auto it_dev : data.window_data[pre_wind_index].already_installed_device) {
				if (data.device_data[it_dev].is_core_device)
					data.window_data[pre_wind_index].process_time.insert(data.device_process_time[pre_energy_type]);
			}
		}
		//区域状态恢复
		auto it_area = find(data.area_data[pre_area_index].already_installed_device.begin(), 
			data.area_data[pre_area_index].already_installed_device.end(), device_index);
		data.area_data[pre_area_index].already_installed_device.erase(it_area);
		//全局状态恢复
		data.have_installed_device_num--;
	}

	//窗口状态更新
	data.window_data[wind].already_installed_device.emplace_back(device_index);
	//这个设备是核心设备才计算加工时间，process_time改成set，是核心设备就往里push
	if (data.device_data[device_index].is_core_device) {
		data.window_data[wind].process_time.insert(data.device_process_time[energy_type]);
		if (data.device_data[device_index].last_device.size() == 0) //核心头结点的次数要加进去
			data.window_data[wind].in_times++;
		for (auto& dev_last : data.device_data[device_index].last_device) {
			if (dev_last->is_core_device && data.linegraph.adjacent_matrix[dev_last->index][device_index] != 2)
				data.window_data[wind].in_times++;//这个设备和上个设备是核心设备，且两个设备不是协同设备
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
	//全局状态更新
	data.have_installed_device_num++;
}