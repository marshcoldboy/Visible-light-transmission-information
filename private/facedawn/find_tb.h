#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <random>
#include <vector>
#define ll long long
using namespace cv;
using namespace std;
double cross(Point before, Point next, Point mid)//before->mid    mid->next  ������ˣ�ƽ���ı��������
{
	double x1, y1, x2, y2;
	x1 = mid.x - before.x;
	x2 = next.x - mid.x;
	y1 = mid.y - before.y;
	y2 = next.y - mid.y;
	return (x1 * y2) - (x2 * y1);
}

double dis(Point a, Point b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

Point base;//��׼�㣬��Ϊ͹����һ�㣬���������е㶼�ڸõ��ұ�
bool cmp(Point a, Point b)//���ݸõ����׼������ߺ�ˮƽ�����ɵļн�����
{
	double tana, tanb;
	if (a.x - base.x == 0)
		tana = (a.y - base.y) * 1000;//�˴�Ĭ��x��ֵ��С��С��0.001
	else
		tana = (a.y - base.y) / (a.x - base.x);
	if (b.x - base.x == 0)
		tanb = (b.y - base.y) * 1000;
	else
		tanb = (b.y - base.y) / (b.x - base.x);
	if (tana != tanb)
		return tana < tanb;
	else
		return dis(a, base) < dis(b, base);
}

void findbase(vector<Point>& a)
{
	int cnt = a.size(), t = 0;
	base = a[0];
	for (int i = 0; i < cnt; i++)
	{
		if (a[i].x < base.x || (a[i].x == base.x && a[i].y < base.y))
		{
			base = a[i];
			t = i;
		}
	}
	swap(a[t], a[0]);
}

vector<Point> find_tubao(vector<Point>& a)
{
	vector<Point> result;
	//findbase(a);
	//sort(a.begin() + 1, a.end(), cmp);
	result.push_back(a[0]);
	int top = 0, cnt = a.size();
	for (int i = 1; i < cnt; i++)
	{
		while (top > 0 && cross(result[top - 1], a[i], result[top]) >= 0)
		{
			top--;
			result.pop_back();
		}
		result.push_back(a[i]); top++;
	}
	return result;
}