#ifndef STATION_H
#define STATION_H
#include<QString>
#include<QSet>
#include<QPoint>
#include"line.h"

typedef QPair<QString,int> indexInLine;

class Station
{
private:
    QString name;//站点名称
    double longitude,latitude;//站点的经纬度
    QVector<QString>LinesAcrossStation;//站点经过的线路
    QSet<indexInLine>ids;//站点在线路上的顺序

public:
    Station();
    friend class Line;
    friend class MetroSystem;
};

#endif // STATION_H
