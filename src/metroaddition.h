#ifndef METROADDITION_H
#define METROADDITION_H

#include <QMainWindow>
#include"metrosystem.h"
namespace Ui {
class MetroAddition;
}

#define ADDLINE 0
#define ADDSTATION 1
#define ADDCONNECTION 2
class MetroAddition : public QMainWindow
{
    Q_OBJECT

public:
    MetroAddition(int type,MetroSystem* _metrosystem);
    ~MetroAddition();
    friend class MainWindow;

private:
    Ui::MetroAddition *ui;
    MetroSystem* metrosystem;
    QColor color;//线路的颜色
    void getColor();//得到选择的颜色
    void setStationTip();//设置添加站点的提示信息
    void setConnectionInfo();//设置添加连接的访问信息

};

#endif // METROADDITION_H
