#ifndef METROSYSTEM_H
#define METROSYSTEM_H

#include<QMap>
#include<QVector>
#include"line.h"
#include"station.h"

struct Node{
    QString name;
    double dist;
    Node(QString Name,double Dist):name(Name),dist(Dist){};
    Node(){};
    //>重载，构造优先队列的小顶堆
    bool operator > (const Node& n)const
    {
        return this->dist>n.dist;
    }
};


class MetroSystem
{
private:
    int num;//线路的数量(包括支线)
    QMap<QString,Line>Lines;//所有线路
    QMap<QString,Station>Stations;//所有站点
    QSet<Edge> edges;//所有边
    QMap<QString,QVector<Node>>Graph;//用于搜索的图
    double minLongitude=180,maxLongitude=0,minLatitude=90,maxLatitude=0;//最大和最小的经纬度
    void clearData();//清空数据
    void makeGraph();//得到搜索用的图
    QVector<QString> getBestMid(QString start,QString end);//找到两个换乘站中间的一条线路上的最短路径
    void getMidStations(QVector<QString>tempStations,QVector<QString>&stationList,QVector<Edge>&edgeList);//根据换乘站点表还原整个最短路径

public:
    MetroSystem();
    bool readFile(QString path);//读数据文件
    QVector<QString>getLineList();//得到所有线路的列表
    QVector<QString>getStations();//得到所有站点的列表
    QVector<QString>getLineStationList(QString name);//得到一条线路上所有站点的列表
    QVector<QString>getCommonLines(QString s1,QString s2);//得到同时经过两个站点的线路
    QColor getLineColor(QString name);//得到单一线路的颜色
    QPointF getStationPos(QString name);//得到站点的经纬度坐标
    QVector<QString>getStationLines(QString name);//得到经过该站的所有线路
    double calculateDist(QString station1,QString station2);//计算两个站点之间的距离
    bool judgeStationInLine(QString stationName,QString LineName);//判断站点是否在线路上
    bool judgeIsConnected(QString station1,QString station2,QString line);//判断两站点是否已经连接到某条线路上
    void getMinMaxLongiLati(double& minLongi,double& maxLongi,double& minLati,double& maxLati);//得到最大和最小经纬度
    void getGraph(QVector<QString>&stationList,QVector<Edge>&edgeList);//得到站点和边连接表
    bool transferMinTime(QString start,QString end,QVector<QString>&stationList,QVector<Edge>&edgeList);//时间最短换乘方案
    int transferLeastTransfer(QString start,QString end,QVector<QString>&stationList,QVector<Edge>&edgeList);//换乘次数最少方案
    void addLine(QString name,QColor color);//添加线路
    void addStation(QString name,double longitude,double latitude);//添加站点
    void addConnection(QString station1,QString station2,QString line);//添加连接

};

#endif // METROSYSTEM_H
