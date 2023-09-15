#ifndef GRAPHICSVIEWZOOM_H
#define GRAPHICSVIEWZOOM_H

#include <QObject>
#include<QGraphicsView>
class GraphicsViewZoom : public QObject
{
    Q_OBJECT
private:
    QGraphicsView* myview;
    double basicZoomFactor=1.001;//初始缩放因子

    void zoom(double factor);//缩放函数
    bool eventFilter(QObject *object, QEvent *event);//事件过滤器，用来处理滚轮事件
public:
    GraphicsViewZoom(QGraphicsView* view);


signals:

};

#endif // GRAPHICSVIEWZOOM_H
