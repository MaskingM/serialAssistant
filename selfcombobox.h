#ifndef SELFCOMBOBOX_H
#define SELFCOMBOBOX_H
#include <QObject>
#include <QComboBox>
#include <QWidget>
#include <QMouseEvent>

class SelfComboBox: public QComboBox
{
    Q_OBJECT
public:
    explicit SelfComboBox(QWidget *parent = 0);
    virtual ~SelfComboBox();
protected:
    virtual void mousePressEvent(QMouseEvent *event);  //添加鼠标点击事件

signals:
    void clicked();  //自定义点击信号，在mousePressEvent事件发生时触发
};

#endif // SELFCOMBOBOX_H
