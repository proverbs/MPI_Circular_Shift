// 反射格雷码映射方法，输出路径
/*
Written by proverbs(xuhao)
i' = i + 2^q
Run with: mpiexec –n [2^p] MPI_test.exe [p] [q]
i.e.mpiexec –n 16 MPI_test.exe 4 1
*/
#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>

#define N 1 << 20

using namespace std;

int p, q;

int grayToBit(int x) {//格雷码转二进制编码
	int rt = x & (1 << (p - 1));
	bool last;
	if (rt != 0) last = 1;
	else last = 0;
	for (int i = p - 2; i >= 0; i--) {
		if (x & (1 << i)) last = 1 ^ last;
		else last = last ^ 0;
		rt |= last << i;
	}
	return rt;
}

int bitToGray(int x) {//二进制编码转格雷码
	return (x >> 1) ^ x;
}

int getMidProcessor(int srcBit, int dstBit) {
	int w[20], num = 0;
	int status = srcBit ^ dstBit;
	for (int i = 0; i < p; i++) {
		if (status & (1 << i)) {
			w[num++] = i;
		}
	}
	int xBit = srcBit ^ (1 << w[0]);
	int yBit = srcBit ^ (1 << w[1]);
	//可以随便取一个，一定不会阻塞
	return grayToBit(xBit);
	
}

int main() {
	cin >> p >> q;
	ofstream output;
	output.open("sample.out");

	for (int i = 0; i < (1 << p); i++) {// process
		int srcGray = i;
		int dstGray = (srcGray + (1 << q)) & ((1 << p) - 1);
		int srcBit = bitToGray(srcGray);
		int dstBit = bitToGray(dstGray);

		if (q == 0) {// 位移为2^0(1)时，仅需要1次传输
			output << srcGray << " ---> " << dstGray << endl;
		}
		else {// 位移为2^k(k!=0)时，需要2次传输
			int mid = getMidProcessor(srcBit, dstBit);
			output << srcGray << " ---> " << mid << " ---> " << dstGray << endl;
		}
	}
	output.close();
	return 0;
}
