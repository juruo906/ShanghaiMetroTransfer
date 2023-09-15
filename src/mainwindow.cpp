#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_metroaddition.h"
#include<QMessageBox>
#include<QGraphicsLineItem>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumSize(1200,750);
    this->setWindowTitle(QString("上海地铁换乘指南"));
    this->setWindowIcon(QIcon(QPixmap(":/data/icon/ShanghaiMetro.png")));

    open_Help=new Help();
    metrosystem=new MetroSystem();
    myView=new GraphicsViewZoom(ui->graphicsView);

    scene=new QGraphicsScene;
    scene->setSceneRect(0,0,SCENE_WIDTH,SCENE_HEIGHT);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    //读地铁信息文件
    bool flag=metrosystem->readFile(":/data/Info/Infomation.txt");
    if(!flag){
        QMessageBox::critical(this,"错误","地铁数据文件读取失败");
    }

    setMyToolbar();//设置工具栏
    setLegend();//设置图例
    setConnect();//设置connect函数
    setTransferInfo();//设置换乘部分信息
    showMetroMap();//展示地铁网络图

}

//设置工具栏
void MainWindow::setMyToolbar(){
    ui->toolBar->addAction(ui->actionaddLine);
    ui->toolBar->addAction(ui->actionaddStation);
    ui->toolBar->addAction(ui->actionaddConnection);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionenlarge);
    ui->toolBar->addAction(ui->actionshrink);
    ui->toolBar->addAction(ui->actionopenMap);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionhelp);
}

//设置图例
void MainWindow::setLegend(){
    ui->textBrowser_legend->clear();
    ui->textBrowser_legend->setAlignment(Qt::AlignHCenter);
    QFont font;
    font.setFamily("黑体");
    font.setPixelSize(14);
    ui->textBrowser_legend->setFont(font);
    ui->textBrowser_legend->insertPlainText("图例\n");
    QVector<QString>listName=metrosystem->getLineList();

    ui->textBrowser_legend->setAlignment(Qt::AlignLeft);
    for(int i=0;i<listName.size();i++){
        ui->textBrowser_legend->setTextColor(Qt::black);
        ui->textBrowser_legend->setFontFamily("宋体");
        ui->textBrowser_legend->setFontPointSize(12);
        QVector<QString>temp;
        temp.push_back(listName[i]);
        QColor color=getLinesColor(temp);
        ui->textBrowser_legend->setTextColor(color);
        ui->textBrowser_legend->insertPlainText(listName[i]);
        ui->textBrowser_legend->insertPlainText("—— ");
        if((i+1)%4==0){
            ui->textBrowser_legend->insertPlainText("\n");
        }
    }
}

//设置connect函数
void MainWindow::setConnect(){
    connect(ui->actionhelp,&QAction::triggered,this,[=](){
        open_Help->show();
    });
    connect(ui->actionenlarge,&QAction::triggered,this,[=](){
        ui->graphicsView->scale(1.5,1.5);
    });
    connect(ui->actionshrink,&QAction::triggered,this,[=](){
        ui->graphicsView->scale(2.0/3,2.0/3);
    });
    connect(ui->actionopenMap,&QAction::triggered,this,[=](){
        showMetroMap();
    });
    connect(ui->actionaddLine,&QAction::triggered,this,&MainWindow::addLine);
    connect(ui->actionaddStation,&QAction::triggered,this,&MainWindow::addStation);
    connect(ui->actionaddConnection,&QAction::triggered,this,&MainWindow::addConnection);
    connect(ui->comboBoxStartLine,&QComboBox::currentTextChanged,this,&MainWindow::selectStartStation);
    connect(ui->comboBoxEndLine,&QComboBox::currentTextChanged,this,&MainWindow::selectEndStation);
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::transferResult);

}

//添加线路
void MainWindow::addLine(){
    metroaddition=new MetroAddition(ADDLINE,metrosystem);
    metroaddition->setAttribute(Qt::WA_DeleteOnClose);
    connect(metroaddition->ui->pB_AddLine,&QPushButton::clicked,this,&MainWindow::addLineComplete);
    metroaddition->show();
}

//添加完线路后更新信息
void MainWindow::addLineComplete(){
    QString newLineName=metroaddition->ui->inputLineName->text();
    if(newLineName==""){
        QMessageBox::critical(metroaddition,"错误","未输入线路名称");
    }
    else if(metroaddition->color==QColor::Invalid){
        QMessageBox::critical(metroaddition,"错误","未选择颜色");
    }
    else{
        //qDebug()<<metroaddition->color;
        QVector<QString>lineList=metrosystem->getLineList();
        if(lineList.contains(newLineName))
            QMessageBox::critical(this,"错误","该线路已存在");
        else{
            metrosystem->addLine(newLineName,metroaddition->color);
            QMessageBox::information(this,"提示","线路添加成功");
            metroaddition->close();
            setTransferInfo();
            setLegend();
        }
    }
}

//添加站点
void MainWindow::addStation(){
    metroaddition=new MetroAddition(ADDSTATION,metrosystem);
    metroaddition->setAttribute(Qt::WA_DeleteOnClose);
    connect(metroaddition->ui->pB_AddStation,&QPushButton::clicked,this,&MainWindow::addStationComplete);
    metroaddition->show();
}

//添加完站点后更新信息
void MainWindow::addStationComplete(){
    QString newStationName=metroaddition->ui->inputStationName->text();
    QVector<QString>stationList=metrosystem->getStations();
    double longitude=metroaddition->ui->inputLongi->value();
    double latitude=metroaddition->ui->inputLati->value();
    //qDebug()<<longitude<<latitude;
    int judge=1;
    for(const auto&it:stationList){
        if(metrosystem->getStationPos(it).x()==longitude&&metrosystem->getStationPos(it).y()==latitude){
            judge=0;
            QMessageBox::critical(metroaddition,"错误","站点位置重复");
            break;
        }
    }
    if(!judge)
        return;
    if(newStationName==""){
        QMessageBox::critical(metroaddition,"错误","未输入站点名称");
    }
    else{

        if(stationList.contains(newStationName))
            QMessageBox::critical(this,"错误","该站点已存在");
        else{
            metrosystem->addStation(newStationName,longitude,latitude);
            QMessageBox::information(this,"提示","站点添加成功");
            metroaddition->close();
            showMetroMap();
        }
    }
}

//添加连接
void MainWindow::addConnection(){
    metroaddition=new MetroAddition(ADDCONNECTION,metrosystem);
    metroaddition->setAttribute(Qt::WA_DeleteOnClose);
    connect(metroaddition->ui->pB_AddConnection,&QPushButton::clicked,this,&MainWindow::addConnectionComplete);
    metroaddition->show();
}

//添加完连接后更新信息
void MainWindow::addConnectionComplete(){
    QString station1=metroaddition->ui->comboBox->currentText();
    QString station2=metroaddition->ui->comboBox_2->currentText();
    QString line=metroaddition->ui->comboBox_3->currentText();
    QVector<QString>stationList=metrosystem->getStations();
    QVector<QString>stationsInLine=metrosystem->getLineStationList(line);

    if(!stationList.contains(station1))
        QMessageBox::critical(metroaddition,"错误","站点1不存在");
    else if(!stationList.contains(station2))
        QMessageBox::critical(metroaddition,"错误","站点2不存在");
    else if(station1==station2)
        QMessageBox::critical(metroaddition,"错误","两站点相同");
    else if(!stationsInLine.contains(station1)&&!stationsInLine.contains(station2)&&stationsInLine.size())
        QMessageBox::critical(metroaddition,"错误","站点脱离原线路");
    else if(metrosystem->judgeIsConnected(station1,station2,line))
        QMessageBox::critical(metroaddition,"错误","重复添加连接");
    else{
        metrosystem->addConnection(station1,station2,line);
        QMessageBox::information(this,"提示","连接添加成功");
        metroaddition->close();
        setTransferInfo();
        showMetroMap();
    }


}

//打开选择起点站列表
void MainWindow::selectStartStation(QString name){
    ui->comboBoxStartStation->clear();
    QVector<QString>StationList=metrosystem->getLineStationList(name);
    for(const auto &it:StationList){
        ui->comboBoxStartStation->addItem(it);
    }
}

//打开选择终点站列表
void MainWindow::selectEndStation(QString name){
    ui->comboBoxEndStation->clear();
    QVector<QString>StationList=metrosystem->getLineStationList(name);
    for(const auto &it:StationList){
        ui->comboBoxEndStation->addItem(it);
    }
}

//设置换乘提示信息
void MainWindow::setTransferInfo(){
    ui->comboBoxStartLine->clear();
    ui->comboBoxEndLine->clear();
    QVector<QString>LineList=metrosystem->getLineList();
    for(auto &i:LineList)
    {
        ui->comboBoxStartLine->addItem(i);
        ui->comboBoxEndLine->addItem(i);
    }
    selectStartStation(LineList[0]);
}

//得到线路集合的混合颜色
QColor MainWindow::getLinesColor(QVector<QString>lineList){
    if(lineList.size()){
        QColor color1=QColor(255,255,255);
        QColor color2;
        //计算混合颜色
        for (const auto&it:lineList)
        {
            color2=metrosystem->getLineColor(it);
            color1.setRed(color1.red()*color2.red()/255);
            color1.setGreen(color1.green()*color2.green()/255);
            color1.setBlue(color1.blue()*color2.blue()/255);
        }
        return color1;
    }
    else
        return QColor(0,0,0);
}

//将线路集合转化成字符串
QString MainWindow::getLinesNameStr(QVector<QString>Lines){
    QString LinesName;
    for(const auto& it:Lines){
        LinesName+=it;
        LinesName+=' ';
    }
    return LinesName;
}

//经纬度转化为地铁网络图上的坐标
QPointF MainWindow::tranferPos(QPointF pos){
    double minX,maxX,minY,maxY;
    metrosystem->getMinMaxLongiLati(minX,maxX,minY,maxY);
    //qDebug()<<minX<<maxX<<minY<<maxY;
    double x = (pos.x()-minX)/(maxX-minX)*MAP_WIDTH+MARGIN;
    double y = (maxY-pos.y())/(maxY-minY)*MAP_HEIGHT+MARGIN;
    return QPointF(x,y);
}

//画站点之间的边
void MainWindow::drawEdges(QVector<Edge>edgeLists){
    for(const auto&it:edgeLists){
        QString s1=it.first;
        QString s2=it.second;
        QVector<QString>commonLines=metrosystem->getCommonLines(s1,s2);//得到两个站点之间的所有线路
        QColor color=getLinesColor(commonLines);//得到混合线路颜色
        QString tip="途经："+s1+"--"+s2+"\n线路：";//提示信息
        tip+=getLinesNameStr(commonLines);
        QPointF pos1=tranferPos(metrosystem->getStationPos(s1));
        QPointF pos2=tranferPos(metrosystem->getStationPos(s2));

        QGraphicsLineItem* edgeItem=new QGraphicsLineItem;//画边
        edgeItem->setPen(QPen(color, PEN_WIDTH));
        edgeItem->setCursor(Qt::PointingHandCursor);
        edgeItem->setToolTip(tip);
        //edgeItem->setPos(pos1);
        edgeItem->setLine(pos1.x(), pos1.y(), pos2.x(), pos2.y());
        scene->addItem(edgeItem);
    }
}

//画站点
void MainWindow::drawStations(QVector<QString>stationList){
    for(const auto &it:stationList){
        QPointF coord=metrosystem->getStationPos(it);
        QPointF pos=tranferPos(coord);//得到站点在视图上的坐标
        QVector<QString>stationLines=metrosystem->getStationLines(it);
        QColor color=getLinesColor(stationLines);
        QString tip="站名："+it+"\n"+"经度："+QString::number(coord.x(),'f',7)+"\n"+
                      "纬度："+QString::number(coord.y(),'f',7)+"\n"+"线路："+getLinesNameStr(stationLines);
        //换乘站
        if(stationLines.size()>1){
            QGraphicsRectItem* stationItem=new QGraphicsRectItem;//画圆圈
            stationItem->setPos(pos);
            stationItem->setRect(-HALF_STATION_WIDTH, -HALF_STATION_WIDTH, HALF_STATION_WIDTH*2, HALF_STATION_WIDTH*2);
            stationItem->setCursor(Qt::PointingHandCursor);
            stationItem->setToolTip(tip);
            stationItem->setBrush(QColor(QRgb(0xffffff)));
            scene->addItem(stationItem);
        }
        //只有一条线的站
        else{
            QGraphicsEllipseItem* stationItem=new QGraphicsEllipseItem;//画正方形
            stationItem->setRect(-HALF_STATION_WIDTH, -HALF_STATION_WIDTH, HALF_STATION_WIDTH*2, HALF_STATION_WIDTH*2);
            stationItem->setPos(pos);
            stationItem->setPen(color);
            stationItem->setCursor(Qt::PointingHandCursor);
            stationItem->setToolTip(tip);
            stationItem->setBrush(QColor(QRgb(0xffffff)));
            scene->addItem(stationItem);
        }
        QGraphicsTextItem* textItem=new QGraphicsTextItem;//写站点名称
        textItem->setPlainText(it);
        textItem->setFont(QFont("宋体",6,3));
        textItem->setPos(pos.x(),pos.y()-HALF_STATION_WIDTH*2.5);
        scene->addItem(textItem);
    }
}

void MainWindow::showMetroMap(){
    scene->clear();
    //ui->textBrowser->clear();
    QVector<QString>stationList;
    QVector<Edge>edgeList;
    metrosystem->getGraph(stationList,edgeList);
    drawEdges(edgeList);//画边
    drawStations(stationList);//画站点
    ui->graphicsView->centerOn(tranferPos(metrosystem->getStationPos("人民广场")));//设置中心点
}

void MainWindow::transferResult(){
    QString start=ui->comboBoxStartStation->currentText();
    QString end=ui->comboBoxEndStation->currentText();
    QString startLine=ui->comboBoxStartLine->currentText();
    QString endLine=ui->comboBoxEndLine->currentText();
    bool judgeStart=metrosystem->judgeStationInLine(start,startLine);
    bool judgeEnd=metrosystem->judgeStationInLine(end,endLine);
    if(judgeStart==0||judgeEnd==0){
        QMessageBox::critical(this,"错误","输入的站点不存在或不在对应的线路上，请重新输入");
    }
    else if(start==end){
        QMessageBox::critical(this,"错误","起点和终点站相同，请重新输入");
    }
    else{
        QVector<QString>stationList;
        QVector<Edge>edgeList;
        int judge=0;//判断是否找到路径
        int type=0;//路径查找方案类型
        //换乘时间最短
        if(ui->radioButtonMinTime->isChecked()){
            judge=metrosystem->transferMinTime(start,end,stationList,edgeList);
            type=1;

        }
        //换乘次数最少
        else{
            judge=metrosystem->transferLeastTransfer(start,end,stationList,edgeList);
            type=2;
        }
        if(judge){
            scene->clear();
            drawEdges(edgeList);//画边
            drawStations(stationList);//画站点
            ui->graphicsView->centerOn(tranferPos(metrosystem->getStationPos(stationList[0])));//设置中心点
            QFont font;
            font.setPixelSize(14);
            ui->textBrowser->clear();
            ui->textBrowser->setAlignment(Qt::AlignHCenter);
            ui->textBrowser->setFont(font);
            ui->textBrowser->setTextColor(Qt::black);
            QString tip;
            if(type==1){
                ui->textBrowser->insertPlainText(QString("最短时间换乘方案如下所示,\n需要坐%1站：\n").arg(stationList.size()-1));
            }
            else if(type==2){
                ui->textBrowser->insertPlainText(QString("最少换乘次数换乘方案如下所示,\n需要换乘%1次,需要坐%2站：\n").arg(judge-1).arg(stationList.size()-1));
            }
            QColor color,oldcolor=Qt::white;//记录上一站的颜色
            for(int i=0;i<stationList.size();++i){
                QVector<QString>linesName;
                //根据现在站点和后一个站点所处的共同线路确定路径颜色
                if(i<stationList.size()-1)
                    linesName=metrosystem->getCommonLines(stationList[i],stationList[i+1]);
                else
                    linesName=metrosystem->getCommonLines(stationList[i-1],stationList[i]);
                //处理一条边有多条线路的情况,去掉多余的线路
                if(linesName.size()>1){
                    int find=0;
                    //正向寻找真正需要乘坐的线路
                    for(int j=i+1;j<stationList.size()-1;j++){
                        QVector<QString>thisLineName=metrosystem->getCommonLines(stationList[j],stationList[j+1]);
                        if(thisLineName.size()==1&&linesName.contains(thisLineName[0])){
                            linesName.clear();
                            linesName.push_back(thisLineName[0]);
                            find=1;
                            //qDebug()<<linesName;
                            break;
                        }
                    }
                    //若正向找不到，则反向寻找真正需要乘坐的线路
                    if(!find){
                        for(int j=i;j>=1;j--){
                            QVector<QString>thisLineName=metrosystem->getCommonLines(stationList[j-1],stationList[j]);
                            if(thisLineName.size()==1&&linesName.contains(thisLineName[0])){
                                linesName.clear();
                                linesName.push_back(thisLineName[0]);
                                //qDebug()<<linesName;
                                break;
                            }
                        }
                    }
                }
                color=getLinesColor(linesName);
                //初始乘坐线路
                if(i==0){
                    ui->textBrowser->setTextColor(Qt::black);
                    ui->textBrowser->insertPlainText("乘坐");
                    for(const auto&it:linesName)
                        ui->textBrowser->insertPlainText(it+" ");
                    ui->textBrowser->insertPlainText("\n    ↓\n");
                }
                ui->textBrowser->setTextColor(color);
                ui->textBrowser->insertPlainText("  "+stationList[i]+' ');
                ui->textBrowser->insertPlainText(getLinesNameStr(linesName));
                //换乘提示
                if(color!=oldcolor&&i>0){
                    ui->textBrowser->setTextColor(Qt::black);
                    ui->textBrowser->insertPlainText("(本站换乘)");
                    ui->textBrowser->setTextColor(color);
                }
                if(i<stationList.size()-1){
                    ui->textBrowser->insertPlainText("\n    ↓\n");
                }
                oldcolor=color;//记录上一站的颜色
            }
        }
        else{
            QMessageBox::warning(this,"提示","当前的两个站点间暂无路径可到达");
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete open_Help;
    delete myView;
    delete scene;
    delete metrosystem;

}

