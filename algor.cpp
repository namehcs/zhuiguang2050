#include "algor.h"


/**********************************************************************************************
迭代及优化算法
**********************************************************************************************/
void Result::Algorithm(Data& data, int line_num) {
	int line = 0, start_wind_index = 0;
	queue<int> L1;
	vector<long> match_costs;
	vector<vector<int>> unhook_window; //改为可拆环的window，自己排序，不一定要用unordered_map
	/*插入头节点和插入第一个初始化匹配*/
	for (auto& f : data.linegraph.first_device)
		L1.push(f->index);
	installed_window.insert({ 0, vector<int>(data.device_num, 999) });
	installed_area.insert({ 0, vector<int>(data.device_num, 999) });

	/*生成全部展开窗口序列的匹配*/
	bool flag_res = Install_Match(data, L1, start_wind_index, line);
	if (flag_res) {
		long one_cost = Get_Cost(data);
		match_costs.push_back(one_cost);
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
				int best_line = Unhook(data, wind[1]);
				
			}
		}
		Output(data, line);
		cout << match_costs[0] << endl;
		//计算这个res_index的代价，并选择代价最小的匹配结果
	}
}


/**********************************************************************************************
拆环后的优化匹配算法
找到代价更低的匹配就把
**********************************************************************************************/
int Result::Unhook(Data& data, int wind) {
	int line = 1, start_wind_index = 0;
	queue<int> L1;
	vector<int> unhook_line = data.sqread_circle[0];
	/*先根据拆环窗口号生成窗口序列*/
	for (int i = 1; i <= data.max_loop_num; i++) {
		for (vector<int>::iterator it = unhook_line.begin() + 1; it != unhook_line.end(); it++) {
			if (wind == (*it) && wind == (*(it-1))) {  //如果有连续两个都等于wind，就删去后面一个
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
安装匹配算法
**********************************************************************************************/
bool Result::Install_Match(Data& data, queue<int>& L1, int start_wind_index, int line) {
	bool valid_result = true;
	int cur_dev, cur_wind, cur_wind_index = start_wind_index;
	vector<int> done_lastnum(data.device_num, 0);     //过程变量：特殊节点已匹配的输入个数
	queue<int> L2;

	while (1) {
		if (L1.empty()) {
			L1 = L2;
			while (!L2.empty())
				L2.pop();
			cur_wind_index++;
		}
		cur_dev = L1.front();

		/*判断能否放入当前窗口*/
		cur_wind = data.sqread_circle[line][cur_wind_index];
		bool match_cur_wind = Check_Match(data, cur_dev, cur_wind, cur_wind_index, line);
		if (!match_cur_wind) {
			L2.push(cur_dev);
			L1.pop();
			continue;
		}
		int cur_area = Area_Match(data, cur_dev, cur_wind);
		install_device(data, cur_dev, cur_area, cur_wind, cur_wind_index);  //这里只取了第一个，生成一种匹配方案
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
void Result::install_device(Data& data, int device_index, int area_index, int wind, int wind_index) {
	int energy_type = data.area_data[area_index].energy_type;
	//窗口状态更新
	data.window_data[wind].already_installed_device.emplace_back(device_index);
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
bool Result::Check_Match(Data& data, int dev_index, int wind, int wind_index, int line) {
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
			installed_window[line][last_dev] >= wind_index)  //而且输入设备的安装窗口
			return false;
	}
	return result;
}


///*返回的是回环窗口序列的*/
//queue<int> Result::Choose_Window(Data& data, int dev_indev, int cur_wind_index) {
//	queue<int> results;
//	return results;
//}
