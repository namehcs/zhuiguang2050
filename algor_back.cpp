/**********************************************************************************************
窗口匹配算法
**********************************************************************************************/
bool Result::Install_Match_back(Data& data, int line, int res) {
	bool valid_result = true;
	int cur_dev, cur_wind = 0;
	int cur_wind_index = data.sqread_circle.size() - 1;
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
		cur_dev = L1.front();

		/*判断能否放入当前窗口*/
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
bool Result::Check_Match_back(Data& data, int dev_index, int wind, int wind_index, int match_index, int& area_index) {
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
	for (int i = 0; i < data.device_data[dev_index].next_device.size(); i++) {
		int last_dev = data.device_data[dev_index].next_device[i]->index;
		if (data.linegraph.adjacent_matrix[last_dev][dev_index] != 2 &&  //不是协同设备
			installed_window[match_index][last_dev] <= wind_index)  //而且输入设备的安装窗口
			return false;
	}

	/*遍历这个窗口中安装成本最小的区域是哪个*/
	int min_cost = INT_MAX;
	for (auto& area : data.device_data[dev_index].surport_window[wind_index])
	{
		if (data.device_data[dev_index].energy_install_cost[area[i]] < min_cost) {
			min_cost = data.device_data[dev_index].energy_install_cost[area[i]];
			area_index = area[i];
		}
	}

	/*如果移到这个窗口成本变高了，就不放在这个窗口*/
	if (min_cost > data.device_data[dev_index].install_cost)
		return false;

	return result;
}





/**********************************************************************************************
安装设备，更新状态
**********************************************************************************************/
void Result::install_device(Data& data, int device_index, int area_index, int wind_index, int pre_wind_index = -1, int pre_area_index = -1) {
	int window_index = data.area_data[area_index].window_index;
	int energy_type = data.area_data[area_index].energy_type;

	/*删除之前安装的状态*/
	if (pre_wind_index >= 0)
	{
		/*窗口状态恢复*/
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
		//区域状态恢复
		auto& it1 = data.area_data[pre_area_index].already_installed_device.find(device_index);
		data.area_data[pre_area_index].already_installed_device.erase(it1);
		//全局状态恢复
		data.have_installed_device_num--;
	}

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
	data.have_installed_device_num++;
}