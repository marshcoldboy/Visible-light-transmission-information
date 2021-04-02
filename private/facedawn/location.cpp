#include <opencv2/opencv.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <ctime>
#include "find_tb.h"
#define ll long long
using namespace cv;
using namespace std;
#define NumberofColorBlocks 100
#define Numbmerofpixels 5
// �ݶ�ÿ��200��ɫ�飬ÿ��ɫ��3�����أ�һ�߹�600������
// �ṩһ����Ӧ��ͼƬ�ڰ�ɫ���01�����������Ӧ��ͼƬ

Mat pic()
{	
	int matrix[NumberofColorBlocks+10][NumberofColorBlocks+10];
	vector<Vec3b> rgb_pixels;  //���� rgb���ص��б�
	rgb_pixels.push_back({ 255,255,255 });//0--��ɫ
	rgb_pixels.push_back({ 0,0,0 });//1--��ɫ
	int all = NumberofColorBlocks * Numbmerofpixels;
	Mat img(all, all, CV_8UC3, Scalar(0, 0, 0));
	int i, j, p, q;
	for (i = 0; i < NumberofColorBlocks; i++)
	{
		for (j = 0; j < NumberofColorBlocks; j++)
		{
			matrix[i][j] = -1;
		}
	}//ȫ������-1������δʹ��
	int bigAnchorPoint[8][8] = {
		1,1,1,1,1,1,1,0,
		1,0,0,0,0,0,1,0,
		1,0,1,1,1,0,1,0,
		1,0,1,0,1,0,1,0,
		1,0,1,1,1,0,1,0,
		1,0,0,0,0,0,1,0,
		1,1,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0
	};//��λ��
	int smallAnchorPoint[5][5] = {
		1,1,1,1,1,
		1,0,0,0,1,
		1,0,1,0,1,
		1,0,0,0,1,
		1,1,1,1,1
	};//С��λ��
	for (i = 0; i < NumberofColorBlocks; i++)
	{
		for (j = 0; j < NumberofColorBlocks; j++)
		{
			if (matrix[i][j] == -1)
			matrix[i][j] = rand() % 2;
		}
	}//���ڴ洢01��Ϣ�Ĳ��֣����ܴ� NumberofColorBlocks*Numbmerofpixels-217 ��bit

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			matrix[i][j] = bigAnchorPoint[i][j];
			matrix[NumberofColorBlocks - 1 - i][j] = bigAnchorPoint[i][j];
			matrix[i][NumberofColorBlocks - 1 - j] = bigAnchorPoint[i][j];
		}
	}
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			matrix[NumberofColorBlocks - 1 - i][NumberofColorBlocks - 1 - j] = smallAnchorPoint[i][j];
		}
	}
	for (i = 0; i * Numbmerofpixels < img.rows; i++)
	{
		for (j = 0; j * Numbmerofpixels < img.cols; j++)
		{
			for (p = 0; p < Numbmerofpixels; p++)
			{
				for (q = 0; q < Numbmerofpixels; q++)
				{
					img.at<Vec3b>(i * Numbmerofpixels + p, j * Numbmerofpixels + q) = rgb_pixels[matrix[i][j]];
				}
			}
		}
	}
	imshow("img", img);
	waitKey();
	return img;
}

void show(Mat img)
{
	imshow("show", img);
	waitKey();
}

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold=50;
int const max_lowThreshold = 100;
int ratioo = 3;
int kernel_size = 3;
char* window_name = (char*)"Edge Map";
Mat fsrc;	
int ccc = 0;

void SobelEnhance(const Mat src, Mat& dst)
{
	Mat sobelx(src.size(), CV_16SC1);///��Ե���󣬻��и�ֵ��Ҳ���д���255��ֵ�����������ΪCV_16SC1�з�������
	Mat sobely(src.size(), CV_16SC1);
	Mat img_edgeFiltex(src.size(), CV_8UC1);///���ͼ��������ΪCV_8UC1������ֵ�ض�
	Mat img_edgeFiltey(src.size(), CV_8UC1);
	Sobel(src, sobelx, CV_16SC1, 1, 0, 3);
	convertScaleAbs(sobelx, img_edgeFiltex);
	Sobel(src, sobely, CV_16SC1, 0, 1, 3);
	convertScaleAbs(sobely, img_edgeFiltey);
	///���ؼ�Ȩ
	addWeighted(img_edgeFiltex, 1, img_edgeFiltey, 1, 0, dst);
}

/**
 * @���� CannyThreshold
 * @��飺 trackbar �����ص� - Canny��ֵ�������1:3
 */
void CannyThreshold(int, void*)
{
	/// ʹ�� 3x3�ں˽���
	//blur(src_gray, detected_edges, Size(3, 3));//����Ҷ�ͼsrc_gray������˲����ͼƬ��detected_edges
	equalizeHist(detected_edges, detected_edges);//ֱ��ͼ���⻯���������ػҶ�ֵ�Ķ�̬��Χ���ﵽ��ǿͼ������Աȶȵ�Ч��

	
	GaussianBlur(src_gray, detected_edges, Size(7, 7), 0, 0);
	threshold(src_gray, detected_edges, 150, 255, THRESH_BINARY);

	
	//SobelEnhance(detected_edges, detected_edges);// ��ǿͼ��
	
	//detected_edges = src_gray;
	//imwrite("pic.jpg", detected_edges);
	//show(detected_edges);
	//threshold(detected_edges, detected_edges, 150, 255, 0);
	/// ����Canny����, ��Ե��¼��ԭͼ��
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratioo, kernel_size);

	//�������Σ��ñ�Ե�պ�
	cv::dilate(detected_edges, detected_edges, cv::Mat());//����
	cv::dilate(detected_edges, detected_edges, cv::Mat());//����
	//cv::erode(detected_edges, detected_edges, cv::Mat());//��ʴ
	
	/// ʹ�� Canny���������Ե��Ϊ������ʾԭͼ��
	dst = Scalar::all(0);

	src.copyTo(dst, detected_edges);
	//imshow(window_name, dst);
	imwrite("edge.jpg", dst);
}

/**
 * @���� canny
 * @��飺 ����һ��Mat����ø�ͼ��Ե
 */
void canny(Mat a)
{
	src = a;
	src.copyTo(fsrc);
	//GaussianBlur(src, src, Size(9, 9), 0, 0);
	/*int col = src.cols, row = src.rows;
	while (col > 900 || row > 900)
	{
		col *= 0.99;
		row *= 0.99;
	}
	resize(src, src, Size(col, row));*/
	

	dst.create(src.size(), src.type());//������img��ͬ��С��ͼ
	if (src.type() != CV_8UC1)
		cvtColor(src, src_gray, COLOR_BGR2GRAY);//ת�Ҷ�ͼ
	else
		src.copyTo(src_gray);
	//namedWindow(window_name, WINDOW_AUTOSIZE);
	//createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);
	CannyThreshold(0, 0);
	waitKey(0);
}

struct po
{
	int minx, maxx, miny, maxy;
	int i;
	vector<Point> d;
};

bool po_cmp(po a,po b)
{
	if (a.maxx - a.minx != b.maxx - b.minx)
	{
		return a.maxx - a.minx > b.maxx - b.minx;
	}
	else
		return a.maxy - a.miny > b.maxy - b.miny;
}//�Ժ�ѡ��λ���������

string int2str(const int& int_temp)
{
	string string_temp;
	stringstream stream;
	stream << int_temp;
	string_temp = stream.str();   //�˴�Ҳ������ stream>>string_temp  
	return string_temp;
}



int find_point(vector<po>&s)//�Ӻ�ѡ�����ҵ�����,���ص�һ�������
{
	int all = s.size();
		int col = fsrc.cols, row = src.rows;
		for (int i = 0; i + 2 < all; i++)
		{
			int p1x, p2x, p3x, p1y, p2y, p3y, mid1x, mid1y, mid2x, mid2y, mid3x, mid3y;
			p1x = s[i].maxx - s[i].minx;
			p2x = s[i + 1].maxx - s[i + 1].minx;
			p3x = s[i + 2].maxx - s[i + 2].minx;
			p1y = s[i].maxy - s[i].miny;
			p2y = s[i + 1].maxy - s[i + 1].miny;
			p3y = s[i + 2].maxy - s[i + 2].miny;
			mid1x = (s[i].maxx + s[i].minx) / 2;
			mid1y = (s[i].maxy + s[i].miny) / 2;
			mid2x = (s[i + 1].maxx + s[i + 1].minx) / 2;
			mid2y = (s[i + 1].maxy + s[i + 1].miny) / 2;
			mid3x = (s[i + 2].maxx + s[i + 2].minx) / 2;
			mid3y = (s[i + 2].maxy + s[i + 2].miny) / 2;
			//printf("%d %d\n", col,row);
			double ab, bc, ac;
			ab = sqrt((mid2x - mid1x) * (mid2x - mid1x) + (mid2y - mid1y) * (mid2y - mid1y));
			bc = sqrt((mid2x - mid3x) * (mid2x - mid3x) + (mid2y - mid3y) * (mid2y - mid3y));
			ac = sqrt((mid3x - mid1x) * (mid3x - mid1x) + (mid3y - mid1y) * (mid3y - mid1y));
			double cosa, cosb, cosc;

			cosa = ((mid2x - mid1x) * (mid3x - mid1x) + (mid2y - mid1y) * (mid3y - mid1y)) / ab / ac;
			cosb = ((mid1x - mid2x) * (mid3x - mid2x) + (mid1y - mid2y) * (mid3y - mid2y)) / ab / bc;
			cosc = ((mid2x - mid3x) * (mid1x - mid3x) + (mid2y - mid3y) * (mid1y - mid3y)) / bc / ac;
			
			printf("a:%d %d\nb:%d %d\nc:%d %d\n", mid1x, mid1y, mid2x, mid2y, mid3x, mid3y);

			printf("cosa=%lf	cosb=%lf	cosc= %lf\n", cosa, cosb, cosc);
			if (abs(p3x - p1x) <= col / 150 && abs(p3y - p1y) <= row / 150 && abs(p3x - p2x) <= col / 150 && abs(p3y - p2y) <= row / 150)
			{
				//printf("a:%d %d\nb:%d %d\nc:%d %d\n", mid1x, mid1y, mid2x, mid2y, mid3x, mid3y);

				//printf("cosa=%lf	cosb=%lf	cosc= %lf\n", cosa, cosb, cosc);

			}
			else continue;
			double p = 0.25;
			if (cosa<p && cosa>-p || cosb<p && cosb>-p || cosc<p && cosc>-p)
			{//printf("%d..\n", i);
				return i;
				break;
			}
			else continue;
		}


		return -1;

}

Point2f vertexs_minRect_QR[4];//��¼��ά���ĸ������
void find4j()
{
	Point2f mid = Point2f(0, 0);
	for (int i = 0; i < 4; i++)
	{
		mid.x += vertexs_minRect_QR[i].x / 4;
		mid.y += vertexs_minRect_QR[i].y / 4;
	}
	for (int i = 0; i < 4; i++)
	{
		if (vertexs_minRect_QR[i].x < mid.x && vertexs_minRect_QR[i].y < mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[3]);
		}
		if (vertexs_minRect_QR[i].x < mid.x && vertexs_minRect_QR[i].y > mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[2]);
		}
		if (vertexs_minRect_QR[i].x > mid.x && vertexs_minRect_QR[i].y < mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[0]);
		}
		if (vertexs_minRect_QR[i].x > mid.x && vertexs_minRect_QR[i].y > mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[1]);
		}
	}
	if(ccc==0)
	{
		for (int i = 0; i < 4; i++)
	{
		vertexs_minRect_QR[i] += (vertexs_minRect_QR[i] - mid)/10;
	}
	}
}
bool location1(Mat a)//p==1
{
	canny(a);
	
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	//hierarchyΪ2ά�ģ�hierarchy[i][k]�����i�������ĵ�k����������Ӧcontours�е���ţ���������Ϊ-1
	//	k==0--������ͬ���ĺ�һ������
	//	k==1--������ͬ����ǰһ������
	//	k==2--��������һ��������
	//	k==3--�������ĸ�����
	//����ͼ����������������ʽ����������������������ģʽ�������ƽ� 
	findContours(detected_edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<int> vector_contours_filter;//���д�Ŷ�λ��ĺ�ѡ
	for (int k, c, i = 0; i < contours.size(); i++)
	{
		k = i;
		c = 0;
		while (hierarchy[k][2] != -1)//��ʾ��������
		{
			k = hierarchy[k][2];
			c = c + 1;
		}

		//----------------------Index����һ�Σ�c >= 5 -> c >= 4��-------------------------//

		if (c >= 4)			//�д��ڵ���5���������ģ���Ϊ��λ���ѡ
		{
			vector_contours_filter.push_back(i);
		}
	}


	vector<Point> vertex_minRect4;//װ��������λ�㹲12�����������
	if ((vector_contours_filter.size() < 3))//���û���ҵ��㹻�Ķ�λ��
	{
		printf("The picture does not meet the requirements\n");
		printf("cnt=	%d\n", vector_contours_filter.size());
		return false;
	}
	int all = vector_contours_filter.size();
	printf("all point: %d\n", all);
	vector<po>s;//�洢��λ����Ϣ
	/*string start = "out", end = ".jpg";*/
	for (int j = 0; j < all; j++)
	{
		
		po q;
		q.minx = 1e9, q.miny = 1e9, q.maxx = 0, q.maxy = 0;
		q.i = vector_contours_filter[j];
		q.d = find_tubao(contours[vector_contours_filter[j]]);
		//q.d = contours[vector_contours_filter[j]];
		approxPolyDP(q.d, q.d, 2, true);
		
		int cnt = q.d.size();
		printf("%d\n", cnt);
		if (cnt < 4)continue;
		for (int i = 0; i < cnt; i++)//��ȷ�������ľ��ζ��㶪��ȥ
		{
			//int temp = contours[vector_contours_filter[0]].size() / 4;
			q.minx = min(q.minx, q.d[i].x);
			q.miny = min(q.miny, q.d[i].y);
			q.maxx = max(q.maxx, q.d[i].x);
			q.maxy = max(q.maxy, q.d[i].y);
			//printf("%d %d\n", contours[vector_contours_filter[j]][i].x, contours[vector_contours_filter[j]][i].y);
		}
		//printf("\n%d\n", vector_contours_filter[0]);
		/*Mat temp;
		temp = src(Range(q.miny, q.maxy), Range(q.minx, q.maxx));
		string path = start + int2str(j) + end;
		imwrite(path, temp);*/
		//show(temp);
		//q.d = contours[vector_contours_filter[j]];
		s.push_back(q);
	}
	sort(s.begin(), s.end(), po_cmp);//�����������
	int ans = -1;
	ans = find_point(s);//����������ѡ��ĵ�һ�������
	if (ans == -1)
	{
		printf("not match\n");
		return false;
	}printf("%d\n",s[ans].i);
	vector<Point>res_contours;
	res_contours = contours[hierarchy[s[ans].i][3]];
	
	res_contours = find_tubao(res_contours);
	approxPolyDP(res_contours, res_contours, 100, true);
	cout << res_contours.size() << endl;

	for (int i=ans;i<ans+3;i++)
	{
		cout << "cnt:	" << i << " minx= " << s[i].minx << " maxx= " << s[i].maxx << " miny= " << s[i].miny << " maxy= " << s[i].maxy << endl;

		int cnt = s[i].d.size(); printf("cnt: %d\n", cnt);
		for (int j = 0; j < cnt; j++)
		{
			//line(src, s[i].d[j], s[i].d[(j+1)%cnt], Scalar(0, 255, 0), 1, 8);
			vertex_minRect4.push_back(s[i].d[j]);
			//printf("%d %d\n", s[i].d[j].x, s[i].d[j].y);
		}
	}
	Point P1, P2; ll maxdis = 0;
	int cnt = vertex_minRect4.size(); 
	for (int yi = 0; yi < cnt; yi++)//�ҵ�������Զ����
	{
		for (int yt = yi + 1; yt < cnt; yt++)
		{
			ll te = (vertex_minRect4[yi].x - vertex_minRect4[yt].x) * (vertex_minRect4[yi].x - vertex_minRect4[yt].x) + (vertex_minRect4[yi].y - vertex_minRect4[yt].y) * (vertex_minRect4[yi].y - vertex_minRect4[yt].y);
			if (te > maxdis)
			{
				maxdis = te;
				P1 = vertex_minRect4[yi];
				P2 = vertex_minRect4[yt];
			}
		}
	}
	//������Զ������е�Ϊ��ά����е�
	for (int yi = 0; yi < cnt; yi++)
	{
		Point temp;
		temp.x = (P1.x + P2.x) - vertex_minRect4[yi].x;
		temp.y = (P1.y + P2.y) - vertex_minRect4[yi].y;
		vertex_minRect4.push_back(temp);
	}//�����е㣬ȫ���Գƹ�ȥ�����Եõ����ĸ��ǵ�λ��
	
	Point2f point_minRect12[4];

	RotatedRect rect12 = minAreaRect(vertex_minRect4);//����С��Χ����
	//vertex_minRect4�д�Ŷ�λ���ȫ������

	rect12.points(point_minRect12);
	for (int j = 0; j < 4; j++)
	{
		vertexs_minRect_QR[j] = point_minRect12[j];
		//line(src, point_minRect12[j], point_minRect12[(j + 1) % 4], Scalar(0, 0, 255), 1, 8);
	}
	
	Point2f midPoint_rect;
	
	for (int j = 0; j < 4; j++)
	{
		midPoint_rect.x += (point_minRect12[j].x / 4);
		midPoint_rect.y += (point_minRect12[j].y / 4);
	}
	//circle(src, midPoint_rect, 10, Scalar(0, 255, 0), 3, 8);
	//src.copyTo(fsrc);
	int f = 0;
	/*for (int i = 0; i < 4; i++)
	{
		if (vertexs_minRect_QR[i].x > midPoint_rect.x && vertexs_minRect_QR[i].y > midPoint_rect.y)
		{
			f = i;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		vertexs_minRect_QR[i] = point_minRect12[(i + 4 - f)%4];
	}*/
	find4j();
	cout << vertexs_minRect_QR[0] << "  " << vertexs_minRect_QR[1] << "  " << vertexs_minRect_QR[2] << "  " << vertexs_minRect_QR[3] << "  " << endl;
	
	f = 6;
	for (int i = ans; i < ans + 3; i++)
	{
		if (s[i].minx < midPoint_rect.x && s[i].miny < midPoint_rect.y)
		{
			f -= 2;
		}
		if (s[i].minx > midPoint_rect.x && s[i].miny < midPoint_rect.y)
		{
			f -= 3;
		}
		if (s[i].minx > midPoint_rect.x && s[i].miny > midPoint_rect.y)
		{
			f -= 0;
		}
		if (s[i].minx < midPoint_rect.x && s[i].miny > midPoint_rect.y)
		{
			f -= 1;
		}
	}printf("f=%d\n", f); cout << vertexs_minRect_QR[0] << "    " << midPoint_rect << endl;
	Point2f temp1 = vertexs_minRect_QR[(2 + 0 + f) % 4];
	Point2f temp2 = vertexs_minRect_QR[(3 + 0 + f) % 4];
	Point2f temp3 = vertexs_minRect_QR[(0 + 0 + f) % 4];
	Point2f temp0 = vertexs_minRect_QR[(1 + 0 + f) % 4];

	vertexs_minRect_QR[0] = temp0;
	vertexs_minRect_QR[1] = temp1;
	vertexs_minRect_QR[2] = temp2;
	vertexs_minRect_QR[3] = temp3;
	
	cout << vertexs_minRect_QR[0] <<"    "<<midPoint_rect<< endl;
	
	cout << vertexs_minRect_QR[0] << "  " << vertexs_minRect_QR[1] << "  " << vertexs_minRect_QR[2] << "  " << vertexs_minRect_QR[3] << "  " << endl;
	int col = src.cols, row = src.rows;
	while (col > 1000 || row > 1000)
	{
		col *= 0.99;
		row *= 0.99;
	}
	resize(src, src, Size(col, row));
	
	//show(src);
	
}


bool location2(Mat a)
{
	canny(a);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	//hierarchyΪ2ά�ģ�hierarchy[i][k]�����i�������ĵ�k����������Ӧcontours�е���ţ���������Ϊ-1
	//	k==0--������ͬ���ĺ�һ������
	//	k==1--������ͬ����ǰһ������
	//	k==2--��������һ��������
	//	k==3--�������ĸ�����
	//����ͼ����������������ʽ����������������������ģʽ�������ƽ� 
	findContours(detected_edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<int> vector_contours_filter;//���д�Ŷ�λ��ĺ�ѡ
	for (int k, c, i = 0; i < contours.size(); i++)
	{
		k = i;
		c = 0;
		while (hierarchy[k][2] != -1)//��ʾ��������
		{
			k = hierarchy[k][2];
			c = c + 1;
		}

		//----------------------Index����һ�Σ�c >= 5 -> c >= 4��-------------------------//

		if (c >= 4)			//�д��ڵ���5���������ģ���Ϊ��λ���ѡ
		{
			vector_contours_filter.push_back(i);
		}
	}


	vector<Point> vertex_minRect4;//װ��������λ�㹲12�����������
	if ((vector_contours_filter.size() < 3))//���û���ҵ��㹻�Ķ�λ��
	{
		printf("The picture does not meet the requirements\n");
		printf("cnt=	%d\n", vector_contours_filter.size());
		return false;
	}
	int all = vector_contours_filter.size();
	printf("all point: %d\n", all);
	vector<po>s;//�洢��λ����Ϣ
	/*string start = "out", end = ".jpg";*/
	for (int j = 0; j < all; j++)
	{

		po q;
		q.minx = 1e9, q.miny = 1e9, q.maxx = 0, q.maxy = 0;
		q.i = vector_contours_filter[j];
		q.d = find_tubao(contours[vector_contours_filter[j]]);
		//q.d = contours[vector_contours_filter[j]];
		approxPolyDP(q.d, q.d, 2, true);

		int cnt = q.d.size();
		printf("%d\n", cnt);
		if (cnt < 4)continue;
		for (int i = 0; i < cnt; i++)//��ȷ�������ľ��ζ��㶪��ȥ
		{
			//int temp = contours[vector_contours_filter[0]].size() / 4;
			q.minx = min(q.minx, q.d[i].x);
			q.miny = min(q.miny, q.d[i].y);
			q.maxx = max(q.maxx, q.d[i].x);
			q.maxy = max(q.maxy, q.d[i].y);
			//printf("%d %d\n", contours[vector_contours_filter[j]][i].x, contours[vector_contours_filter[j]][i].y);
		}
		//printf("\n%d\n", vector_contours_filter[0]);
		/*Mat temp;
		temp = src(Range(q.miny, q.maxy), Range(q.minx, q.maxx));
		string path = start + int2str(j) + end;
		imwrite(path, temp);*/
		//show(temp);
		//q.d = contours[vector_contours_filter[j]];
		s.push_back(q);
	}
	sort(s.begin(), s.end(), po_cmp);//�����������
	int ans = -1;
	ans = find_point(s);//����������ѡ��ĵ�һ�������
	if (ans == -1)
	{
		printf("not match\n");
		return false;
	}
	printf("biaohao :	%d\n", hierarchy[s[ans].i][3]);
	vector<Point>res_contours;
	
	res_contours = contours[hierarchy[s[ans].i][3]];

	//res_contours = find_tubao(res_contours);
	
	approxPolyDP(res_contours, res_contours, 50, true);
	cout << res_contours << endl;
	cout << res_contours.size() << endl;

	vector<Point2f>point_minRect12;
	for (int i = 0; i < 4; i++)
	{
		Point2f temp = Point2f((double)res_contours[i].x, (double)res_contours[i].y);
		point_minRect12.push_back(temp);
	}
	for (int j = 0; j < 4; j++)
	{
		vertexs_minRect_QR[j] = point_minRect12[j];
		//line(src, point_minRect12[j], point_minRect12[(j + 1) % 4], Scalar(0, 0, 255), 1, 8);
	}

	Point2f midPoint_rect;

	for (int j = 0; j < 4; j++)
	{
		midPoint_rect.x += (point_minRect12[j].x / 4);
		midPoint_rect.y += (point_minRect12[j].y / 4);
	}
	//circle(src, midPoint_rect, 10, Scalar(0, 255, 0), 3, 8);
	//src.copyTo(fsrc);
	int f = 0;
	/*for (int i = 0; i < 4; i++)
	{
		if (vertexs_minRect_QR[i].x > midPoint_rect.x && vertexs_minRect_QR[i].y > midPoint_rect.y)
		{
			f = i;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		vertexs_minRect_QR[i] = point_minRect12[(i + 4 - f)%4];
	}*/
	find4j();
	cout << vertexs_minRect_QR[0] << "  " << vertexs_minRect_QR[1] << "  " << vertexs_minRect_QR[2] << "  " << vertexs_minRect_QR[3] << "  " << endl;

	f = 6;
	for (int i = ans; i < ans + 3; i++)
	{
		if (s[i].minx < midPoint_rect.x && s[i].miny < midPoint_rect.y)
		{
			f -= 2;
		}
		if (s[i].minx > midPoint_rect.x && s[i].miny < midPoint_rect.y)
		{
			f -= 3;
		}
		if (s[i].minx > midPoint_rect.x && s[i].miny > midPoint_rect.y)
		{
			f -= 0;
		}
		if (s[i].minx < midPoint_rect.x && s[i].miny > midPoint_rect.y)
		{
			f -= 1;
		}
	}printf("f=%d\n", f); cout << vertexs_minRect_QR[0] << "    " << midPoint_rect << endl;
	Point2f temp1 = vertexs_minRect_QR[(2 + 0 + f) % 4];
	Point2f temp2 = vertexs_minRect_QR[(3 + 0 + f) % 4];
	Point2f temp3 = vertexs_minRect_QR[(0 + 0 + f) % 4];
	Point2f temp0 = vertexs_minRect_QR[(1 + 0 + f) % 4];

	vertexs_minRect_QR[0] = temp0;
	vertexs_minRect_QR[1] = temp1;
	vertexs_minRect_QR[2] = temp2;
	vertexs_minRect_QR[3] = temp3;

	cout << vertexs_minRect_QR[0] << "    " << midPoint_rect << endl;

	cout << vertexs_minRect_QR[0] << "  " << vertexs_minRect_QR[1] << "  " << vertexs_minRect_QR[2] << "  " << vertexs_minRect_QR[3] << "  " << endl;
	int col = src.cols, row = src.rows;
	while (col > 1000 || row > 1000)
	{
		col *= 0.99;
		row *= 0.99;
	}
	resize(src, src, Size(col, row));

	//show(src);

}
//�Ѿ����ĽǺʹ����ĽǶ�Ӧ


void end_correct(Mat f)
{
	int lenth = 826;
	Point2f vertex_warp[4];
	vertex_warp[1] = Point2f(0, float(lenth - 1));
	vertex_warp[2] = Point2f(0, 0);
	vertex_warp[3] = Point2f(float(lenth - 1), 0);
	vertex_warp[0] = Point2f(float(lenth - 1), float(lenth));
	//find4j();
	Mat transform = getPerspectiveTransform(vertexs_minRect_QR, vertex_warp);
	//show(f);
	warpPerspective(f, dst, transform, Size(lenth, lenth));//dst��Ϊ�����и������Ķ�ά��
}
#include <fstream>
#define numberofblock 55
#define numberofpoint 825
int ans[numberofblock + 5][numberofblock + 5];
void read(Mat a)
{
	ofstream fp("out.txt", ios::out);
	cvtColor(a, a, COLOR_RGB2GRAY);
	//show(a);
	int pointOfeachBlock = numberofpoint / numberofblock;
	for (int i = 0; i < numberofblock; i++)
	{
		for (int j = 0; j < numberofblock; j++)
		{
			
			circle(a, Point2f(i * pointOfeachBlock, j * pointOfeachBlock), 1,Scalar(255,0,0));
			int cnt = 0;
			for (int ii = i*pointOfeachBlock; ii < (i+1)*pointOfeachBlock; ii++)
			{
				uchar* data = a.ptr<uchar>(ii);
				for (int jj = j*pointOfeachBlock; jj <(j+1)*pointOfeachBlock; jj++)
				{
					if (data[jj] < 150)
						cnt++;//��¼�����ص�ĸ���
				}
			}
			//printf("%d %d: %d\n", i, j, cnt);
			if ((double)cnt / (pointOfeachBlock * pointOfeachBlock) > 0.5)//�ڿ��
			{
				//printf("%d ", cnt);
				ans[i][j] = 1;
			}
			else
			{
				ans[i][j] = 0;
			}
			//fprintf(fp, "%d ", ans[i][j]);
			fp << ans[i][j] << " ";
			printf("%d %d %d\n", i, j,ans[i][j]);
		}
		fp << endl;
	}
	fp.close();
}


int main()
{
	//src=pic();

	double second;
	clock_t s_time, e_time;
	s_time = clock();


	string start = "C:\\Users\\xc\\Desktop\\��\\����\\��Ŀ\\data\\pic\\x", end = ".png";
	string start_2 = "C:\\Users\\xc\\Desktop\\��\\����\\��Ŀ\\data\\res\\x";
	int i = 0;
	while(++i)
	{
		//if (i > 1)return 0;
		printf("\n\n%d:\n\n", i);

		string num = int2str(i);
		string path = start + num + end;
		src = imread(path, 1);
		//src = imread("out.jpg");
		//canny(src);
		if (src.empty()) {
			//����Ƿ��ȡͼ��
			cout << "Error! Input image cannot be read...\n";
			return -1;
		}
		//show(src);
		ccc = 1;
		if (location2(src))
		{
			//correct();
			ccc = 1;//1Ϊ������Ե ��0Ϊ��������Ե
		
			end_correct(fsrc);
			//correct(dst);
		}

		e_time = clock();
		second = (double)(e_time - s_time) / CLOCKS_PER_SEC;
		printf("second:		%lf\n", second);

		read(dst);
		imwrite(start_2+num+end, dst);
	}
}
