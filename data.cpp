#include <iostream>
#include <vector>
#include "data.h"

using namespace std;

/*input:
生产次数：K
五种能源加工时间：T[5]
车间数量：N 区域数量：R 区域能源类型：Energy[R][2]
最大回环圈数：L
第一种环回的窗口数量：2
*/
//下面两个变量改的时候，h文件里的数据也要改

vector<Dev_Match> dev_match;

void Data_Choose() {
    for (int i = 0; i < devices; i++) {
        Dev_Match dev;     
        dev.dev_class = Devices[i][0];
        //核心流水线设备类型与窗口支持预处理的筛选
        for (int w = 0; w < winds; w++) {
            if (Windows[w][dev.dev_class + 3] == 1)
                dev.dev_win.push_back(w);
        }
        //设备支持能源与区域能源的筛选
        for (int r = 0; r < R; r++) {
            if (Devices[i][R_Energy[r][1] + 1] != 0)
                dev.dev_area.push_back(r);
        }
        dev_match.push_back(dev);
    }
}

int main() {
    Data_Choose();
    cout << dev_match.size() << endl;
    return 0;
}