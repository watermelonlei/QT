#ifndef _TIMESEQ_FILE_H_
#define _TIMESEQ_FILE_H_


#include <QString>
#include <QHash>


class TimeseqFilePrivate
{
public:
    TimeseqFilePrivate();
    ~TimeseqFilePrivate();

    // ����ʱ�� ����
    bool load();

    // ʱ���ļ�����
    void reload();


    // ʱ���ļ�·��
    QString timeseqFilePath();

    // �ļ���
    QString encryptSeqFileName();//timeseq.bin
    QString decryptSeqFileName();//decrypttimeseq.txt

    // �ļ���

    QString DdecryptSeqFileName();//timeseq.txt
    QString DencryptSeqFileName();//encrypttimeseq.bin

    static char const  S_HEIGH_CODE;
    static char const  S_LOW_CODE;

private:
    // ʱ���ļ�����
    void decrypt();

    // ����
    void encrypt();



    // ����ȫ��ʱ���ļ����ݣ�������֯���б�
    QStringList loadSeqFile();

    // ɾ�����ܺ���ļ�
    void deleteDecryptFile();



public:
    // Ŀ¼
    QString dir;
    QString m_fileNamePix;
};


#endif // _TIMESEQ_FILE_H_
