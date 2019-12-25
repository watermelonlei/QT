#include <QDir>
#include <QFile>
#include <QtDebug>
#include <QStringList>
#include <QTextStream>

#include "timeseqfile.h"



const char TimeseqFilePrivate::S_HEIGH_CODE = 'b';
const char TimeseqFilePrivate::S_LOW_CODE = 'w';
TimeseqFilePrivate::TimeseqFilePrivate()
    : dir("./timeseq/")
{

}

TimeseqFilePrivate::~TimeseqFilePrivate()
{

}

// 加载时序
bool TimeseqFilePrivate::load()
{



    // 时序文件解密
    this->decrypt();

    // 加载全部时序文件内容，按行组织成列表
    QStringList lines = this->loadSeqFile();
    qDebug()<<__FUNCTION__<<lines.count();

    return true;
}

void TimeseqFilePrivate::reload()
{
    // 时序文件解密
    this->encrypt();
}

// 时序文件路径
QString TimeseqFilePrivate::timeseqFilePath()
{
    return this->dir;
}

// 加密文件名
QString TimeseqFilePrivate::encryptSeqFileName()
{
    // 将时序目录下的timeseq*.bin文件作为时序文件
    QDir dir(this->timeseqFilePath());
    QFileInfoList fileInfos = dir.entryInfoList(QDir::Files);

    QString fileName = "timeseq.bin";
    foreach(QFileInfo fileInfo, fileInfos)
    {
        fileName = fileInfo.fileName();
        if (fileName.contains("timeseq") && fileName.contains(".bin"))
        {
            qDebug() << "TimeseqFilePrivate encryptSeqFileName" << fileName;
            break;
        }
    }
    qDebug()<<(this->timeseqFilePath() + fileName);

    return this->timeseqFilePath() + fileName;
}

// 解密文件名
QString TimeseqFilePrivate::decryptSeqFileName()
{
    return this->timeseqFilePath() + "decrypttimeseq.txt";
}

QString TimeseqFilePrivate::DencryptSeqFileName()
{
    return this->timeseqFilePath() + "encrypttimeseq.bin";
}

QString TimeseqFilePrivate::DdecryptSeqFileName()
{
    return this->timeseqFilePath() + "timeseq.txt";
}



// 时序文件解密
void TimeseqFilePrivate::decrypt()
{
    qDebug() << "TimeseqFilePrivate decrypt ";

    char* src = 0;
    char* dest = 0;
    QFile srcFile(this->encryptSeqFileName());
    QFile destFile(this->decryptSeqFileName());
    do 
    {
        // 打开文件
        if (!srcFile.open(QIODevice::ReadOnly) || 
            !destFile.open(QIODevice::WriteOnly))
        {
            qWarning() << "TimeseqFilePrivate decrypt open fail";
            break;
        }

        // 获取数据长度，并分配缓冲区
        int srcLen = srcFile.size();
        int destLen = srcLen / 2;

        src = new char[srcLen];
        dest = new char[destLen];
        if (0 == src || 0 == dest)
        {
            qWarning() << "TimeseqFilePrivate decrypt new fail";
            break;
        }

        // 读取数据
        memset(src, 0, srcLen);

        int len = srcFile.read(src, srcLen);
        if (len < srcLen)
        {
            qWarning() << "TimeseqFilePrivate decrypt read fail";
            break;
        }

        // 解密
        memset(dest, 0, destLen);
        for (int i = 0, j = 0; i < srcLen; i += 2, j++)
        {
            char high = *(src + i);
            high -= S_HEIGH_CODE;//x
            high &= 0xF;

            char low = *(src + i + 1);
            low -= S_LOW_CODE;//z
            low &= 0xF;

            *(dest + j) = ((high << 4) | low);
        }

        // 写入解密后的数据
        len = destFile.write(dest, destLen);
        if (len < destLen)
        {
            qWarning() << "TimeseqFilePrivate decrypt write fail";
            break;
        }
    } while (0);

    // 释放资源
    delete [] src;
    delete [] dest;

    srcFile.close();
    destFile.close();
}

void TimeseqFilePrivate::encrypt()
{
    qDebug() << "TimeseqFilePrivate encrypt ";

    char* src = 0;
    char* dest = 0;
    QFile binFile(this->DencryptSeqFileName());    // bin
    QFile destFile(this->DdecryptSeqFileName());   // TXT

    do
    {
        // 打开文件
        if (!binFile.open(QIODevice::WriteOnly) ||
            !destFile.open(QIODevice::ReadOnly))
        {
            qWarning() << "TimeseqFilePrivate encrypt open fail";
            break;
        }

        // 获取数据长度，并分配缓冲区
        int destLen = destFile.size();
        int srcLen = destLen * 2;


        src = new char[srcLen];
        dest = new char[destLen];
        if (0 == src || 0 == dest)
        {
            qWarning() << "TimeseqFilePrivate encrypt new fail";
            break;
        }

        // 读取数据
        memset(dest, 0, destLen);

        int len = destFile.read(dest, destLen);
        if (len < destLen)
        {
            qWarning() << "TimeseqFilePrivate encrypt read fail";
            break;
        }

        // 加密
        memset(src, 0, srcLen);
        for (int i = 0, j = 0; j < destLen; i += 2, j++)
        {
            char value = *(dest + j);

            char high = (value & 0xF0) >> 4;
            char low  = (value & 0x0F);
            high += S_HEIGH_CODE; // x
            low  += S_LOW_CODE; // z



            *(src + i) = high;
            *(src + i + 1) = low;

        }

        // 写入解密后的数据
        len = binFile.write(src, srcLen);
        if (len < srcLen)
        {
            qWarning() << "TimeseqFilePrivate encrypt write fail";
            break;
        }
    } while (0);

    // 释放资源
    delete [] src;
    delete [] dest;

    binFile.close();
    destFile.close();
}



// 加载全部时序文件内容，按行组织成列表
QStringList TimeseqFilePrivate::loadSeqFile()
{
    qDebug() << "TimeseqFilePrivate loadSeqFile";

    QStringList lines;
    QString filename = this->decryptSeqFileName();

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // 以文本流的形式访问，每行对应一个时序动作
        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            // 逐行读取，每行一个时序动作
            QString line = stream.readLine();

            // 去除注释
            int index = line.indexOf("//");
            if (0 <= index)
            {
                line = line.left(index);
            }

            // 截断
            line = line.trimmed();
            if (line.isEmpty())
            {
                continue;
            }

            lines.append(line);
        }
    }
    else
    {
        qWarning() << "TimeseqFilePrivate loadSeqFile open fail";
    }

    return lines;
}

// 删除解密后的文件
void TimeseqFilePrivate::deleteDecryptFile()
{
    QString filename = this->decryptSeqFileName();
    QFile::remove(filename);
}

