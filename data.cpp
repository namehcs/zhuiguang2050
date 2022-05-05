#include <iostream>
#include <vector>
#include "data.h"

using namespace std;

/*input:
����������K
������Դ�ӹ�ʱ�䣺T[5]
����������N ����������R ������Դ���ͣ�Energy[R][2]
���ػ�Ȧ����L
��һ�ֻ��صĴ���������2
*/
//�������������ĵ�ʱ��h�ļ��������ҲҪ��

vector<Dev_Match> dev_match;

void Data_Choose() {
    for (int i = 0; i < devices; i++) {
        Dev_Match dev;     
        dev.dev_class = Devices[i][0];
        //������ˮ���豸�����봰��֧��Ԥ�����ɸѡ
        for (int w = 0; w < winds; w++) {
            if (Windows[w][dev.dev_class + 3] == 1)
                dev.dev_win.push_back(w);
        }
        //�豸֧����Դ��������Դ��ɸѡ
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