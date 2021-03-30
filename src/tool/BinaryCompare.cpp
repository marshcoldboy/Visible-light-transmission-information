#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

int main()
{
    string file_A;
    string file_B;
    cout << "�������ļ�1��";
    cin >> file_A;
    cout << "�������ļ�2��";
    cin >> file_B;
    ifstream in1(file_A, ios::in | ios::binary);
    ifstream in2(file_B, ios::in | ios::binary);
    if (!in1 || !in2)
    {
        in1.close();
        in2.close();
        cout << "�ļ���ʧ�ܣ�";
        return 0;
    }
    long long wrong = 0;

    long long start = in1.tellg();
    in1.seekg(0, ios::end);
    long long end = in1.tellg();
    long long len1 = end - start;

    start = in2.tellg();
    in2.seekg(0, ios::end);
    end = in2.tellg();
    long long len2 = end - start;
	
	cout << endl;
	cout << endl;
    cout << "�ļ�1�ֽ�����" << len1 << endl;
    cout << "�ļ�2�ֽ�����" << len2 << endl;

    in1.seekg(0, ios::beg);
    in2.seekg(0, ios::beg);
    while (!in1.eof() && !in2.eof())
    {
        char *tmp = new char[2];
        char *tmp2 = new char[2];
        in1.read(tmp, 1);
        in2.read(tmp2, 1);
        if (tmp[0] != tmp2[0])
            wrong++;
        delete[] tmp;
        delete[] tmp2;
    }
	
	cout << endl;
	cout << endl;
	cout << "�����ֽ�����" << wrong << endl; 
    cout << "��ȷ�ʣ�" << (double)(len2 - wrong) / (double)len2 << endl;
    system("pause");
    return 0;
}