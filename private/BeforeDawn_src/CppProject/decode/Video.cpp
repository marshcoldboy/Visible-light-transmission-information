#include "Video.h"

Video::Video()
{
	init();
}
Video::Video(string _filename)
{
	loadfile(_filename);
}
void Video::init()
{
	img.clear();
	now = 0;
	filename = "";
}

bool Video::loadfile(string _filename)
{
	init();
	filename = _filename;

	Mat frame;
	VideoCapture cap(filename.data());
	if (!cap.isOpened())
		return false;

	while (cap.read(frame))
	{
		Mat tmp;
		//������Ҫ�������������һ����ַ������Mat���������ֻ��frame�������ַ����ֻ��һ��
		//ÿ���½�frame�ĵ�ַ���ǲ�һ���ģ�����frame��ֵ��ȥ֮��洢�ĵ�ַҲ����һ��
		//�ڴ����βε�ʱ����Զ�copyTo�����Բ��������������ӵ�����
		frame.copyTo(tmp); 

		img.push_back(tmp);
	}
	cap.release();

	return true;
}

bool Video::isEnd()
{
	return (now == img.size());
}

Mat Video::nextImg()
{
	return img[now++];
}
void Video::resetPointer()
{
	now = 0;
}
int Video::size()
{
	return img.size();
}
unsigned int Video::pointer()
{
	return now;
}