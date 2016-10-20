// 反射格雷码映射方法
/*
Written by proverbs(xuhao)
i' = i + 2^q
Run with: mpiexec –n [2^p] MPI_test.exe [p] [q]
i.e.mpiexec –n 16 MPI_test.exe 4 1
*/
#include "stdafx.h"
#include "stdafx.h"  
#include "mpi.h"  
#include <stdio.h>  
#include <math.h>  
#include <Windows.h>  
#include <iostream>
#include <ctime>

using namespace std;

int val;
int p, q;
int srcRelation[20], dstRelation[20];

void getPQ(char* argv[]) {//读取p,q
    p = 0;
    int i = 0;
    while (argv[1][i] != '\0') {
        p = p * 10 + argv[1][i] - '0';
        i++;
    }

    q = 0;
    i = 0;
    while (argv[2][i] != '\0') {
        q = q * 10 + argv[2][i] - '0';
        i++;
    }
}

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
int main(int argc, char* argv[])
{
    getPQ(argv);//读取p,q值

    int rank, processNum;

    //初始化MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);//获得进程号
    MPI_Comm_size(MPI_COMM_WORLD, &processNum);//返回通信子的进程数

    int srcGray = rank;
    int dstGray = (srcGray + (1 << q)) & ((1 << p) - 1);
    int srcBit = bitToGray(srcGray);
    int dstBit = bitToGray(dstGray);

    val = srcGray;//处理器初始数据=处理器编号

    int send[2], receive[2];

    if (q == 0) {
        send[0] = val;
        send[1] = -1;//位移为1时，仅需要一次传输
        fprintf(stderr, "%d ---> %d\n", srcGray, dstGray);
        MPI_Send(send, 2, MPI_INT32_T, dstGray, 0, MPI_COMM_WORLD);
    }
    else {
        send[0] = val;
        send[1] = dstGray;
        int mid = getMidProcessor(srcBit, dstBit);
        //fprintf(stderr, "%d to %d acc %d\n", srcGray, dstGray, mid);
        //fprintf(stderr, "%d to %d = %d to %d acc %d\n", srcGray, dstGray, srcBit, dstBit, mid);
        fprintf(stderr, "%d ---> %d\n", srcGray, mid);
        MPI_Send(send, 2, MPI_INT32_T, mid, 0, MPI_COMM_WORLD);
    }
    //防止缓冲区溢出
    int rd = 100;
    Sleep(rd);
    //fprintf(stderr, "------rand %d \n", rd);

    MPI_Recv(receive, 2, MPI_INT32_T, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int ndst = receive[1];
    int nval = receive[0];
    if (ndst != -1) {
        fprintf(stderr, "%d ---> %d\n", srcGray, ndst);
        MPI_Send(&nval, 1, MPI_INT32_T, ndst, 0, MPI_COMM_WORLD);
        MPI_Recv(&val, 1, MPI_INT32_T, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else {
        val = nval;
    }

    fprintf(stderr, "process %d of value %d\n", srcGray, val);//输出最终处理器数据
    //system("pause");
    MPI_Finalize();
    return 0;
}