#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QGraphicsScene>
#include"help.h"
#include"metrosystem.h"
#include"GraphicsViewZoom.h"
#include"metroaddition.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define MARGIN 30           //视图边距
#define MAP_WIDTH 3000      //地铁网络图宽度
#define MAP_HEIGHT 3000     //地铁网络图高度
#define SCENE_WIDTH MAP_WIDTH+MARGIN*2       //视图宽度
#define SCENE_HEIGHT MAP_HEIGHT+MARGIN*2     //视图高度

#define PEN_WIDTH 2    //线路边宽
#define HALF_STATION_WIDTH 3 //站点半宽度

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;//主窗口ui
    Help* open_Help;//帮助窗口
    GraphicsViewZoom *myView;//视图子类(用于缩放)
    QGraphicsScene *scene;//视图类
    MetroSystem *metrosystem;//地铁换乘及绘图实现
    MetroAddition *metroaddition;//添加站点线路的窗口

    void setMyToolbar();//设置工具栏
    void setLegend();//设置图例
    void setConnect();//设置connect函数

    void addLine();//添加线路
    void addLineComplete();//添加完线路后更新信息
    void addStation();//添加站点
    void addStationComplete();//添加完站点后更新信息
    void addConnection();//添加连接
    void addConnectionComplete();//添加完连接后更新信息

    void selectStartStation(QString name);//打开选择起点站列表
    void selectEndStation(QString name);//打开选择终点站列表
    void setTransferInfo();//设置换乘提示信息

    QColor getLinesColor(QVector<QString>lineList);//得到线路集合的混合颜色
    QString getLinesNameStr(QVector<QString>Lines);//将线路集合转化成字符串
    QPointF tranferPos(QPointF pos);//经纬度转化为地铁网络图上的坐标

    void drawEdges(QVector<Edge>edgeLists);//画站点之间的边
    void drawStations(QVector<QString>stationList);//画站点
    void showMetroMap();//画地铁网络图

    void transferResult();//输出换乘结果


};
#endif // MAINWINDOW_H

