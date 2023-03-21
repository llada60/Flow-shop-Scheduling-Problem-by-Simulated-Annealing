#include<iostream>
#include<math.h>
#include<vector>
#include<cstring>
#include<stdlib.h>
#include<time.h>
#include<algorithm>
#define random(x) (rand()%(x))
using namespace std;
const int IMAX=100;
const double MAX=1e6;
//可修改参数
//HC
int LOOPT[8]={100,1000,10000,50000,100000,500000,1000000,5000000};
//SA
double dT=0.9;//温度的变化
int tMAX=3000;//同温度下的迭代次数,即t
double T=10000;//初始化温度
double eps=1e-6;//当T<eps则退火结束
int process[IMAX][IMAX]={0};//记录每个零件对应工序加工时间 
int sizex=0,sizey=0;//sizex:the num of process;sizey:the num of obj
vector<int> order;//每道工序的零件加工顺序
vector<int> best_order;//记录最优的val和加工顺序
void print(vector<int>& some)
{
	for(int j=0;j<sizey;j++)
	{
		cout<<some[j]<<" ";
	}
	cout<<endl;
	return;
}
//读取文件 
void init()
{
	FILE *fp=fopen("input.txt","r");
	//读入sizex:工序数，sizey:零件数 
	fscanf(fp,"%d %d",&sizey,&sizex);
	//读入每个零件对应工序的加工时间 
	for(int i=0;i<sizey;i++)
	{
		for(int j=0;j<sizex;j++)
		{
			int tmp=-1;
			fscanf(fp,"%d",&tmp);
			fscanf(fp,"%d",&process[i][tmp]);
		}
	}
	//构造1~sizey个零件的整数序列 
	for(int j=0;j<sizey;j++)
	{
		order.push_back(j);
	}
	random_shuffle(order.begin(),order.end());
	return;
}
//计算当前策略的val
int sta[IMAX][IMAX];
int cal_value(vector<int>& order)
{
	//第x道工序，y零件的最早开始时间 
	memset(sta,0,sizeof(sta));
	//计算val:结束时间=该零件在该层加工时间+max(同一层上个零件结束时间 , 上一层同个零件结束时间)
	for(int i=0;i<sizex;i++)
	{
		for(int j=0;j<sizey;j++)
		{
			sta[i+1][j]=max(sta[i+1][j],sta[i][j]+process[order[j]][i]);
			sta[i][j+1]=max(sta[i][j+1],sta[i][j]+process[order[j]][i]);
		}
	}
	return sta[sizex-1][sizey];
}
//随机移动：随机交换两个零件的加工顺序 
void random_change(vector<int>& order)
{
	int c1=random(sizey),c2=random(sizey);
	swap(order[c1],order[c2]);
	return;
}
//update
void upd(vector<int>& cge,vector<int>& save)
{
	cge.clear();
	for(int i=0;i<save.size();i++)
	cge.push_back(save[i]);
	return;
}
// Simulated Annealing
void SA()
{
	vector<int> las_order;
	double start,end;
	int cnt=0;
	int flag=1;
	cout<<"======Simulated Annealing Result======"<<endl;
	T=10000;
	random_shuffle(order.begin(),order.end());
	int best_val=MAX;//记录最优的总加工时间结果
	int las_val=MAX,now_val=0;//记录当前val和上一轮的val
	cout<<"t: "<<tMAX<<endl;
	start=clock();
	while(T>eps)
	{
		cnt++;
		for(int t=0;t<=tMAX;t++)//在同一个温度下尝试移动 
		{
			//存储上一次的order
			upd(las_order,order);
			//随机移动 
			random_change(order);
			//计算出当前的val
			now_val=cal_value(order);
			if(flag&&now_val==7038)
			{
				FILE *f=fopen("output.csv","w+");
				for(int j=0;j<sizey;j++)
				{
					fprintf(f,"%d,%d,%d,%d,%d\n",sta[0][j],sta[1][j],sta[2][j],sta[3][j],sta[4][j]);
				}
				flag=0;
			}
			//求val的变化
			int deta=now_val-las_val;
			//找到更优的解，更新 
			if(deta<0)
			{
				las_val=now_val;
				if(now_val<best_val)//更新存储的最优顺序及val 
				{
					best_val=now_val;
					upd(best_order,order);
				}
			}
			//未找到更优的解 
			else
			{
				//计算是否更新的概率 
				double p=exp((-1)*deta/(T));
				//确定更新 
				if((rand()/RAND_MAX)>p)
				{
					las_val=now_val;
				}
				else//不更新则返回到开始时的工序顺序 
				{
					upd(order,las_order);
				}
			}
		}
		T*=dT;
	}
	end=clock();
	print(best_order);
	cout<<best_val<<endl;
	cout<<" time: "<<(double)(end-start)/CLOCKS_PER_SEC<<endl;
	return;
}
// Hill Climbing
void HC()
{
	clock_t start,end;
	vector<int> las_order;
	cout<<"======Hill Climbing Result======"<<endl;
    //搜索最优值
	for(int j=0;j<8;j++)
	{
		random_shuffle(order.begin(),order.end());
		double best_val=MAX;//记录最优的总加工时间结果
		double now_val=0;//记录当前val和上一轮的val
		start=clock();
		cout<<"LOOPT: "<<LOOPT[j]<<endl;
		for(int i=0;i<LOOPT[j];i++)
		{
			//存储上一次的order
			upd(las_order,order);
			//随机移动 
			random_change(order);
			//计算出当前的val
			now_val=cal_value(order);
			//求val的变化
			double deta=now_val-best_val;
			//找到更优的解，更新 
			if(deta<0)
			{
				best_val=now_val;
				upd(best_order,order);
			}
			//未找到更优解，返回原先状态
			else 
			{
				upd(order,las_order);
			}
		}
		print(best_order);
		cout<<best_val;
		end=clock();
		cout<<" time: "<<(double)(end-start)/CLOCKS_PER_SEC<<endl;
	}
}
int main()
{
	srand((unsigned)time(0));//使random的结果非伪随机 
	init();//读入数据 
	HC();
	SA();
	return 0;
}