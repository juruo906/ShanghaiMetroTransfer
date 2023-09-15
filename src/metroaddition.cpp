#include "metroaddition.h"
#include "ui_metroaddition.h"
#include<QColorDialog>
#include<QMessageBox>
MetroAddition::MetroAddition(int type,MetroSystem* _metrosystem) :
    QMainWindow(nullptr),
    ui(new Ui::MetroAddition)
{
    metrosystem=_metrosystem;
    const QVector<QString>titles={"添加线路","添加站点","添加连接"};
    const QVector<QString>icons={":/data/icon/Line.png",":/data/icon/Station.png",":/data/icon/connect.png"};
    ui->setupUi(this);
    this->setFixedSize(600,400);
    this->setWindowTitle(titles[type]);
    this->setWindowIcon(QIcon(icons[type]));
    this->color=QColor::Invalid;
    this->setWindowModality(Qt::ApplicationModal);

    switch (type) {
    case ADDLINE:
        ui->stackedWidget->setCurrentIndex(0);
        connect(ui->pB_SelectColor,&QPushButton::clicked,this,&MetroAddition::getColor);
        break;
    case ADDSTATION:
        ui->stackedWidget->setCurrentIndex(1);
        setStationTip();
        break;
    case ADDCONNECTION:
        ui->stackedWidget->setCurrentIndex(2);
        setConnectionInfo();

    default:
        break;
    }
}

//得到选择的颜色
void MetroAddition::getColor(){
    color=QColorDialog::getColor(Qt::white,this);
    ui->pB_SelectColor->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue()));
    //qDebug()<<color;
}

//设置添加站点的提示信息
void MetroAddition::setStationTip(){
    double minLongi,maxLongi,minLati,maxLati;
    metrosystem->getMinMaxLongiLati(minLongi,maxLongi,minLati,maxLati);
    QString tip=QString("经度和纬度范围:\n经度：[%1,%2]\n纬度：[%3,%4]").arg(minLongi-0.01,0,'g',6)
        .arg(maxLongi+0.01,0,'g',6).arg(minLati-0.01,0,'g',6).arg(maxLati+0.01,0,'g',6);
    ui->label_MinMaxLoLa->setText(tip);
    ui->label_MinMaxLoLa->setAlignment(Qt::AlignHCenter);
    //经纬度的边界设置的比实际值略大，便于延长线路
    ui->inputLongi->setMinimum(minLongi-0.01);
    ui->inputLongi->setMaximum(maxLongi+0.01);
    ui->inputLongi->setValue(minLongi-0.01);
    ui->inputLati->setMinimum(minLati-0.01);
    ui->inputLati->setMaximum(maxLati+0.01);
    ui->inputLati->setValue(minLati-0.01);
    ui->label_tip->setTextInteractionFlags(Qt::TextSelectableByMouse);
}

//设置添加连接的访问信息
void MetroAddition::setConnectionInfo(){
    QVector<QString>allStations=metrosystem->getStations();
    QVector<QString>lineList=metrosystem->getLineList();
    QVector<QString>stationList;
    //按照线路顺序添加站点
    for(const auto&i:lineList){
        QVector<QString>stationsInLine=metrosystem->getLineStationList(i);
        for(const auto&j:stationsInLine){
            if(!stationList.contains(j))
            stationList.push_back(j);
        }
    }
    //添加不在线路上的站点
    for(const auto&it:allStations){
        if(!stationList.contains(it))
            stationList.push_back(it);
    }
    for(const auto&it:stationList){
        ui->comboBox->addItem(it);
        ui->comboBox_2->addItem(it);
    }
    for(const auto&it:lineList)
        ui->comboBox_3->addItem(it);
}




MetroAddition::~MetroAddition()
{
    delete ui;
}
