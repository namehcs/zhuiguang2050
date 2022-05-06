#include "data.h"

using namespace std;

vector<Dev_Match> dev_match;
unordered_set<int> Key_devices = { 0, 1, 2, 5, 6, 7, 9 };

void Data_Choose() {
    for (int i = 0; i < devices; i++) {
        Dev_Match dev;     
        dev.dev_class = Devices[i][0];
        //������ˮ���豸�����봰��֧��Ԥ�����ɸѡ
        for (int w = 0; w < winds; w++) {
            dev.dev_win.push_back(w);
            if (Key_devices.find(i)!= Key_devices.end() && Windows[w][dev.dev_class + 3] == 0)
                dev.dev_win.pop_back();
        }
        //�豸֧����Դ��������Դ��ɸѡ
        for (int r = 0; r < R; r++) {
            if (Devices[i][R_Energy[r][1] + 1] != 0)
                dev.dev_area.push_back(r);
        }
        dev_match.push_back(dev);
    }
    return;
}

int main() {
    Data_Choose();
    cout << dev_match.size() << endl;
    return 0;
}