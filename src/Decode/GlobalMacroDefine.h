#pragma once

//ÿ�ߵĳ���
#ifndef NumberofColorBlocks
#define NumberofColorBlocks 51
#endif // !NumberofColorBlocks

//������
#ifndef DataContain
#define DataContain (NumberofColorBlocks * NumberofColorBlocks - 192 - 30 - (NumberofColorBlocks - 16) * 2) //���Դ������������217Ϊ��λ��ռ�ã�30Ϊ�汾��Ϣ
#endif // !DataContain

#ifndef DEBUG
//#define DEBUG
#endif