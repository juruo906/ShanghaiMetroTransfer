#ifndef LINE_H
#define LINE_H
#include<QString>
#include<QColor>
#include<QSet>

//定义边类型
typedef QPair<QString,QString> Edge;

class Line
{
private:
    int id;//线路编号
    QString name;//线路名称
    QColor lineColor;//线路颜色
    QSet<QString>stationsInline;//线路上的站点
    QVector<QString>startToEnd;//线路的起终点

public:
    Line();
    friend class Station;
    friend class MetroSystem;
};

#endif // LINE_H
