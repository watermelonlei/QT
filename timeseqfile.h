#ifndef _TIMESEQ_FILE_H_
#define _TIMESEQ_FILE_H_


#include <QString>
#include <QHash>


class TimeseqFilePrivate
{
public:
    TimeseqFilePrivate();
    ~TimeseqFilePrivate();

    // 加载时序 解密
    bool load();

    // 时序文件加密
    void reload();


    // 时序文件路径
    QString timeseqFilePath();

    // 文件名
    QString encryptSeqFileName();//timeseq.bin
    QString decryptSeqFileName();//decrypttimeseq.txt

    // 文件名

    QString DdecryptSeqFileName();//timeseq.txt
    QString DencryptSeqFileName();//encrypttimeseq.bin

    static char const  S_HEIGH_CODE;
    static char const  S_LOW_CODE;

private:
    // 时序文件解密
    void decrypt();

    // 加密
    void encrypt();



    // 加载全部时序文件内容，按行组织成列表
    QStringList loadSeqFile();

    // 删除解密后的文件
    void deleteDecryptFile();



public:
    // 目录
    QString dir;
    QString m_fileNamePix;
};


#endif // _TIMESEQ_FILE_H_
