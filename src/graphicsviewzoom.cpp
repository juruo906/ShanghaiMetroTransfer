#include "graphicsviewzoom.h"
#include<QEvent>
#include<QWheelEvent>
GraphicsViewZoom::GraphicsViewZoom(QGraphicsView* view)
    : QObject(view)
{
    myview=view;
    myview->viewport()->installEventFilter(this);
}

//缩放函数
void GraphicsViewZoom::zoom(double factor){
    myview->scale(factor, factor);
}

//事件过滤器，用来处理滚轮事件
bool GraphicsViewZoom::eventFilter(QObject *object, QEvent *event){
    if (event->type() == QEvent::Wheel) {
        QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
        if (wheel_event->angleDelta().y() != 0&&wheel_event->angleDelta().x() == 0) {
            double angle = wheel_event->angleDelta().y();
            //根据滚轮移动的垂直角度来进行不同程度的缩放
            double factor = qPow(basicZoomFactor, angle);
            zoom(factor);
            return true;
        }
    }
    return false;
}
