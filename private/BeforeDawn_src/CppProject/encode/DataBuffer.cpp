#include "DataBuffer.h"

DataBuffer::DataBuffer()
{
    now = 0;
    len = 0;
    data = NULL;
    resetPointer();
}
DataBuffer::DataBuffer(string filename)
{
    data = NULL;
    reload(filename);
}

bool DataBuffer::reload(string filename)
{
    if (data != NULL)
        delete data;
    ifstream in(filename.data(), ios::in | ios::binary);
    if (!in)
    {
        now = 0;
        len = 0;
        data = NULL;
        return false;
    }
    resetPointer();

    long long start = in.tellg();
    in.seekg(0, ios::end);
    long long end = in.tellg();
    len = end - start;
    data = new char[len + 10];

    in.clear();
    in.seekg(0, ios::beg);
    in.read(data, len);
    in.close();

    zip();
    correcting();

    int index = filename.rfind(".") + 1;
    filetype = filename.substr(index, filename.size() - index);

    return true;
}
void DataBuffer::correcting()
{
    //������д������Ĳ���
    //�ⲿ�ֵ����ȼ��ϵͣ����ܻ���������
}
void DataBuffer::zip()
{
    //������дѹ���Ĳ���
    //�ⲿ�ֵ����ȼ��ܵͣ����ܲ�����д
}

unsigned int DataBuffer::size()
{
    return len;
}
unsigned int DataBuffer::pointer()
{
    return now;
}
string DataBuffer::getFiletype()
{
    return filetype;
}
char DataBuffer::nextChar()
{
    if (now < len)
        return data[now++];
    else
        return (char)0;
}
bool DataBuffer::isEnd()
{
    return (now == len) || (len == 0);
}
void DataBuffer::resetPointer()
{
    now = 0;
}