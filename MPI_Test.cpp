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

int main(int argc, char* argv[])
{
	// 反射格雷码方法
	getPQ(argv);//读取p,q值, 2^p个处理器，移位q

	int rank, processNum;

	//初始化MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);//获得进程号
	MPI_Comm_size(MPI_COMM_WORLD, &processNum);//返回通信子的进程数

	int node = rank;//节点编号
	val = rank;//处理器初始数据=处理器编号

	int send[2], receive[2];

	if (q == 0) {//移位不能为0
		MPI_Finalize();
		return 0;
	}

	int toNode;//目标节点
	int dstNode = (node + q) % (1 << p);//最终节点
	for (int i = 0; i < p; i++) {
		if ((node & (1 << i)) == (dstNode & (1 << i))) continue;
		send[0] = val;
		send[1] = dstNode;
		toNode = node ^ (1 << i);
		fprintf(stderr, "%d ---> %d\n", node, toNode);
		MPI_Send(send, 2, MPI_INT32_T, toNode, 0, MPI_COMM_WORLD);
		MPI_Recv(receive, 2, MPI_INT32_T, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//MPI_Sendrecv(send, 2, MPI_INT32_T, toNode, 0, receive, 2, MPI_INT32_T, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		val = receive[0];
		dstNode = receive[1];
		//sleep，防止缓冲区溢出
		Sleep(500);
	}

	fprintf(stderr, "process %d of value %d\n", node, val);//输出最终处理器数据
	MPI_Finalize();
	return 0;
}