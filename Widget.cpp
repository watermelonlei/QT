#include "Widget.h"
#include "ui_Widget.h"
#include "timeseqfile.h"
#include<QLabel>
#include<QDebug>
class MyLabel:public QLabel
{
public:
    MyLabel(QString str);
    ~MyLabel();
};


MyLabel::MyLabel(QString str)
    :QLabel(str)
{

}

MyLabel::~MyLabel()
{
    qDebug()<<this->text()<<__FUNCTION__;
}


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    MyLabel* label = new MyLabel("111");
    connect(label,SIGNAL(destroyed(QObject*)),this,SLOT(on_printLable(QObject*)));
    if (1)
    {
        label = new MyLabel("2222");
        connect(label,SIGNAL(destroyed(QObject*)),this,SLOT(on_printLable(QObject*)));
        label->deleteLater();
        qDebug()<<"end 1";
    }
    qDebug()<<"end 2";


}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    TimeseqFilePrivate m_d;
    m_d.load();
}

void Widget::on_pushButton_2_clicked()
{
    TimeseqFilePrivate m_d;
    m_d.reload();
}

void Widget::on_printLable(QObject *obj)
{
    QLabel* lable = qobject_cast<QLabel*>( obj);
    if (lable)
    {
        qDebug()<<lable->text();
    }
}

