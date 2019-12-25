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

// ����ʱ��
bool TimeseqFilePrivate::load()
{



    // ʱ���ļ�����
    this->decrypt();

    // ����ȫ��ʱ���ļ����ݣ�������֯���б�
    QStringList lines = this->loadSeqFile();
    qDebug()<<__FUNCTION__<<lines.count();

    return true;
}

void TimeseqFilePrivate::reload()
{
    // ʱ���ļ�����
    this->encrypt();
}

// ʱ���ļ�·��
QString TimeseqFilePrivate::timeseqFilePath()
{
    return this->dir;
}

// �����ļ���
QString TimeseqFilePrivate::encryptSeqFileName()
{
    // ��ʱ��Ŀ¼�µ�timeseq*.bin�ļ���Ϊʱ���ļ�
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

// �����ļ���
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



// ʱ���ļ�����
void TimeseqFilePrivate::decrypt()
{
    qDebug() << "TimeseqFilePrivate decrypt ";

    char* src = 0;
    char* dest = 0;
    QFile srcFile(this->encryptSeqFileName());
    QFile destFile(this->decryptSeqFileName());
    do 
    {
        // ���ļ�
        if (!srcFile.open(QIODevice::ReadOnly) || 
            !destFile.open(QIODevice::WriteOnly))
        {
            qWarning() << "TimeseqFilePrivate decrypt open fail";
            break;
        }

        // ��ȡ���ݳ��ȣ������仺����
        int srcLen = srcFile.size();
        int destLen = srcLen / 2;

        src = new char[srcLen];
        dest = new char[destLen];
        if (0 == src || 0 == dest)
        {
            qWarning() << "TimeseqFilePrivate decrypt new fail";
            break;
        }

        // ��ȡ����
        memset(src, 0, srcLen);

        int len = srcFile.read(src, srcLen);
        if (len < srcLen)
        {
            qWarning() << "TimeseqFilePrivate decrypt read fail";
            break;
        }

        // ����
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

        // д����ܺ������
        len = destFile.write(dest, destLen);
        if (len < destLen)
        {
            qWarning() << "TimeseqFilePrivate decrypt write fail";
            break;
        }
    } while (0);

    // �ͷ���Դ
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
        // ���ļ�
        if (!binFile.open(QIODevice::WriteOnly) ||
            !destFile.open(QIODevice::ReadOnly))
        {
            qWarning() << "TimeseqFilePrivate encrypt open fail";
            break;
        }

        // ��ȡ���ݳ��ȣ������仺����
        int destLen = destFile.size();
        int srcLen = destLen * 2;


        src = new char[srcLen];
        dest = new char[destLen];
        if (0 == src || 0 == dest)
        {
            qWarning() << "TimeseqFilePrivate encrypt new fail";
            break;
        }

        // ��ȡ����
        memset(dest, 0, destLen);

        int len = destFile.read(dest, destLen);
        if (len < destLen)
        {
            qWarning() << "TimeseqFilePrivate encrypt read fail";
            break;
        }

        // ����
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

        // д����ܺ������
        len = binFile.write(src, srcLen);
        if (len < srcLen)
        {
            qWarning() << "TimeseqFilePrivate encrypt write fail";
            break;
        }
    } while (0);

    // �ͷ���Դ
    delete [] src;
    delete [] dest;

    binFile.close();
    destFile.close();
}



// ����ȫ��ʱ���ļ����ݣ�������֯���б�
QStringList TimeseqFilePrivate::loadSeqFile()
{
    qDebug() << "TimeseqFilePrivate loadSeqFile";

    QStringList lines;
    QString filename = this->decryptSeqFileName();

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // ���ı�������ʽ���ʣ�ÿ�ж�Ӧһ��ʱ����
        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            // ���ж�ȡ��ÿ��һ��ʱ����
            QString line = stream.readLine();

            // ȥ��ע��
            int index = line.indexOf("//");
            if (0 <= index)
            {
                line = line.left(index);
            }

            // �ض�
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

// ɾ�����ܺ���ļ�
void TimeseqFilePrivate::deleteDecryptFile()
{
    QString filename = this->decryptSeqFileName();
    QFile::remove(filename);
}

