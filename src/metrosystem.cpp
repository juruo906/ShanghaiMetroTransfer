#include "metrosystem.h"
#include"QFile"
#include<QDebug>
#include<QTextStream>
#include<algorithm>
#include<QtMath>
#include <queue>
#include<QQueue>
MetroSystem::MetroSystem()
{

}

//读数据文件
bool MetroSystem::readFile(QString path){
    QFile openFile(path);
    openFile.open(QIODevice::ReadOnly);
    QTextStream in(&openFile);
    if(!openFile.isOpen()){
        qDebug()<<"文件无法打开";
        return false;
    }
    else{
        this->num=0;
        bool judge=1;
        while(!in.atEnd()){
            QString id,name,color,fromTo,from,to,totalS,lineId,linecolor;
            QString formSta;
            bool ok;
            Line line;
            int numOfStations;
            //读线路信息
            in>>id>>line.id;
            in>>name>>line.name;
            in>>color>>linecolor;
            in>>fromTo>>from>>to;
            in>>totalS>>numOfStations;
            this->num++;

            line.lineColor.setRgba(linecolor.remove(0,1).toUInt(&ok, 16));
            line.startToEnd.push_back(from);
            line.startToEnd.push_back(to);
            //线路重复(支线)
            if(Lines.contains(line.name)){
                //添加起终点站
                Lines[line.name].startToEnd.push_back(from);
                Lines[line.name].startToEnd.push_back(to);
            }
            else{
                Lines[line.name]=line;
            }
            //读站点信息
            for(int i=0;i<numOfStations;i++){
                int no;
                QString longlati;
                QStringList strList;
                Station sta;
                in>>no>>sta.name>>longlati;

                strList=longlati.split(QChar(','));
                sta.longitude=strList.first().toDouble();
                sta.latitude=strList.last().toDouble();
                //更新最大最小经纬度
                maxLongitude=std::max(maxLongitude,sta.longitude);
                minLongitude=std::min(minLongitude,sta.longitude);
                maxLatitude=std::max(maxLatitude,sta.latitude);
                minLatitude=std::min(minLatitude,sta.latitude);

                //该站点未记录过
                if(!Stations.contains(sta.name)){
                    Stations[sta.name]=sta;
                    if(!Stations[sta.name].LinesAcrossStation.contains(line.name))
                        Stations[sta.name].LinesAcrossStation.push_back(line.name);//记录线路

                }
                //该站点记录过（换乘站）
                else{
                    if(!Stations[sta.name].LinesAcrossStation.contains(line.name))
                        Stations[sta.name].LinesAcrossStation.push_back(line.name);//记录线路
                }
                Lines[line.name].stationsInline.insert(sta.name);//线路记录此站点
                Stations[sta.name].ids.insert(indexInLine(line.name,no));//记录站点在不同线的索引

                //添加边
                if(i){
                    if(!edges.contains(Edge(formSta,sta.name))&&!edges.contains(Edge(sta.name,formSta)))
                        edges.insert(Edge(formSta,sta.name));
                }
                formSta=sta.name;
            }
            //判断文件格式是否正确
            if(id=="id:" && name=="name:" && color=="colour:" && fromTo=="fromTo:"
                    && totalS=="totalStations:" && ok && !in.atEnd())
                judge=1;
            else
                judge=0;
            if(judge==false)
            {
                openFile.close();
                clearData();
                return false ;
            }
            in.readLine();
        }

        openFile.close();
//        for(const auto &i:Lines.keys()){
//            qDebug()<<i<<' '<<Lines.value(i).name;
//        }
//        qDebug()<<maxLongitude<<' '<<minLongitude<<' '<<maxLatitude<<' '<<minLatitude;
        return true;

    }
}

//清空数据
void MetroSystem::clearData(){
    Lines.clear();
    Stations.clear();
    edges.clear();
}

//得到搜索用的图
void MetroSystem::makeGraph(){
    Graph.clear();
    for (const auto &it : edges)
    {
        double dist=calculateDist(it.first,it.second);
        Graph[it.first].push_back(Node(it.second, dist));
        Graph[it.second].push_back(Node(it.first, dist));
    }
}

//得到所有线路的列表
QVector<QString> MetroSystem::getLineList(){
    QVector<QString>temp;

    typedef QPair<QString,int> cmp;
    QVector<cmp>cmpList;
    for(auto it=Lines.begin();it!=Lines.end();++it){
        cmp ele(it.key(),it.value().id);
        cmpList.push_back(ele);
    }
    //根据线路id进行排序
    std::sort(cmpList.begin(),cmpList.end(),[](cmp s1,cmp s2){
        return s1.second<s2.second;
    });
    for(const auto &it:cmpList)
        temp.push_back(it.first);
    return temp;
}

//得到所有站点的列表
QVector<QString>MetroSystem::getStations(){
    QVector<QString>temp;
    for(const auto&it:Stations)
        temp.push_back(it.name);
    return temp;
}

//得到一条线路上所有站点的列表
QVector<QString> MetroSystem::getLineStationList(QString name){
    QVector<QString>temp;
    if(Lines.contains(name)){
        QSet<QString>stationList=Lines[name].stationsInline;
        typedef QPair<QString,int> cmp;
        QVector<cmp>cmpList;
        for(const auto &it:stationList){
            int id;
            //找到该站点在这条线的id
            for(const auto &i:Stations[it].ids){
                if(i.first==name)
                    id=i.second;
            }
            cmp ele(Stations[it].name,id);
            cmpList.push_back(ele);
        }
        //根据站点id进行排序
        std::sort(cmpList.begin(),cmpList.end(),[](cmp s1,cmp s2){
            return s1.second<=s2.second;
        });
        for(const auto &it:cmpList)
            temp.push_back(it.first);
//        for(const auto &it:temp){
//            qDebug()<<it;
//        }
    }
    return temp;

}

//得到同时经过两个站点的线路
QVector<QString> MetroSystem::getCommonLines(QString s1,QString s2){
    Station sta1=Stations[s1];
    Station sta2=Stations[s2];
    QVector<QString>commonLines;
    for(const auto &it:sta1.LinesAcrossStation){
        if(sta2.LinesAcrossStation.contains(it))
            commonLines.push_back(it);
    }
    return commonLines;
}

//得到单一线路的颜色
QColor MetroSystem::getLineColor(QString name){
    return Lines[name].lineColor;
}

//得到站点的经纬度坐标
QPointF MetroSystem::getStationPos(QString name){
    return QPointF(Stations[name].longitude,Stations[name].latitude);
}

//得到经过该站的所有线路
QVector<QString> MetroSystem::getStationLines(QString name){
    QVector<QString> Lines;
    for(const auto&it:Stations[name].LinesAcrossStation){
        Lines.push_back(it);
    }
    return Lines;
}

//计算两个站点之间的距离
double MetroSystem::calculateDist(QString station1,QString station2){
    const double EARTH_RADIUS = 6378.137;
    const double PI = 3.1415926535898;
    double radLati1 = Stations[station1].latitude*PI/180;
    double radLati2 = Stations[station2].latitude*PI/180;
    double radLongi1=Stations[station1].longitude*PI/180;
    double radLongi2=Stations[station2].longitude*PI/180;
    double a = radLati1 - radLati2;
    double b = radLongi1-radLongi2;
    double s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLati1)*cos(radLati2)*pow(sin(b/2),2)));
    s = s * EARTH_RADIUS;
    return s;
}

//判断站点是否在线路上
bool MetroSystem::judgeStationInLine(QString stationName,QString LineName){
    return Lines[LineName].stationsInline.contains(stationName);
}

//判断两站点是否已经连接到某条线路上
bool MetroSystem::judgeIsConnected(QString station1,QString station2,QString line){
    QVector<QString>commonLines=getCommonLines(station1,station2);
    return commonLines.contains(line)&&(edges.contains(Edge(station1,station2))||edges.contains(Edge(station2,station1)));
}

//得到最大和最小经纬度
void MetroSystem::getMinMaxLongiLati(double& minLongi,double& maxLongi,double& minLati,double& maxLati){
    minLongi=minLongitude;
    maxLongi=maxLongitude;
    minLati=minLatitude;
    maxLati=maxLatitude;
}

//得到站点和边连接表
void MetroSystem::getGraph(QVector<QString>&stationList,QVector<Edge>&edgeList){
    stationList.clear();
    edgeList.clear();
    for(const auto &it:Stations){
        stationList.push_back(it.name);
    }
    for(const auto &it:edges){
        edgeList.push_back(it);
    }

}

//时间最短换乘方案
bool MetroSystem::transferMinTime(QString start,QString end,QVector<QString>&stationList,QVector<Edge>&edgeList){
    makeGraph();//生成图结构
    int judge=0;//判断是否找到路径
    QMap<QString,double>dist;//记录起点站到每个站点的距离
    for(const auto&it:Stations)
        dist[it.name]=INT_MAX;
    QMap<QString,QString>path;//记录路径中每个站点的前继站点
    path[start]="";
    std::priority_queue<Node,std::vector<Node>,std::greater<Node>>q;//优先队列
    dist[start]=0;
    q.push(Node(start,0));
    //dijkstra算法
    while(!q.empty()){
        Node top=q.top();
        q.pop();
        if(top.name==end){
            judge=1;
            break;
        }
        for (const auto &it:Graph[top.name]) {
            if(top.dist+it.dist<dist[it.name]){
                path[it.name]=top.name;
                dist[it.name]=top.dist+it.dist;
                q.push(Node(it.name,dist[it.name]));
            }
        }
    }
    //根据前继站点表倒推生成路径的站点和边序列
    if(path.contains(end)){
        QString temp=end;
        while(temp!=""){
            stationList.push_front(temp);
            if(path[temp]!="")
                edgeList.push_front(Edge(path[temp],temp));
            temp=path[temp];
        }
    }
//    for(auto i:stationList)
//        qDebug()<<i;
    return judge;

}

//找到两个换乘站中间的一条线路上的最短路径
QVector<QString> MetroSystem::getBestMid(QString start,QString end){
    QVector<QString>stationList;
    QVector<QString>lines=getCommonLines(start,end);//同时经过两站点的所有线路
    QMap<int,QVector<QString>>tempStationsList;//记录不同路径
    //qDebug()<<start<<end;
    int index=-1;
    double min_Dist=INT_MAX;
    for(int i=0;i<lines.size();++i){
        double dist=0;
        QQueue<QString> q;
        QMap<QString,bool>stationVisited;//记录站点是否访问过
        for(const auto& i:Lines[lines[i]].stationsInline)
            stationVisited[i]=0;
        QMap<QString,QString>path;//记录路径中站点的前继站点
        path[start]="";
        stationVisited[start]=1;
        q.push_back(start);
        while(!q.empty()){
            QString current=q.front();
            //qDebug()<<"current"<<current;
            q.pop_front();
            if(current==end)
                break;
            for(const auto&it:Graph[current]){
                //在一条线上搜索路径
                if(stationVisited[it.name]==0&&Lines[lines[i]].stationsInline.contains(it.name)){
                    stationVisited[it.name]=1;
                    q.push_back(it.name);
                    path[it.name]=current;
                }
            }
        }
        //根据前继站点表得到路径站点表
        QString temp=path[end];
        dist+=calculateDist(temp,end);
        while(temp!=""){
            tempStationsList[i].push_front(temp);
            if(path[temp]!="")
                dist+=calculateDist(path[temp],temp);
            temp=path[temp];
        }
        //记录和标记最短路径
        if(dist<min_Dist){
            min_Dist=dist;
            index=i;
            //qDebug()<<dist;
        }
        //qDebug()<<lines[i]<<tempStationsList[i].size();
    }
    //录入一条线路上两个站点间的经过的最短路径
    for(const auto&it:tempStationsList[index])
        stationList.push_back(it);
    return stationList;
}

//根据换乘站点表还原整个最短路径
void MetroSystem::getMidStations(QVector<QString>tempStations,QVector<QString>&stationList,QVector<Edge>&edgeList){
    makeGraph();//生成图结构
    for(int i=0;i<tempStations.size()-1;i++){
        QString start=tempStations[i];
        QString end=tempStations[i+1];
        //找到两个换乘站中间的一条线路上的最短路径
        QVector<QString>tempStationsList=getBestMid(start,end);
        for(int j=0;j<tempStationsList.size();++j){
            //qDebug()<<tempStationsList[j];
            stationList.push_back(tempStationsList[j]);
            if(j!=tempStationsList.size()-1)
                edgeList.push_back(Edge(tempStationsList[j],tempStationsList[j+1]));
        }
        edgeList.push_back(Edge(tempStationsList[tempStationsList.size()-1],end));
    }
    stationList.push_back(tempStations[tempStations.size()-1]);
}

//换乘次数最少方案
int MetroSystem::transferLeastTransfer(QString start,QString end,QVector<QString>&stationList,QVector<Edge>&edgeList){
    int judge=0;
    QMap<QString,bool>LineVisited;//标记线路是否访问过
    for(const auto&it:Lines)
        LineVisited[it.name]=0;
    QMap<QString,QVector<QString>>path;//记录路径中站点的前继站点(终点站记录多个前继站点)
    path[start].push_back("");
    QQueue<QString> q;
    q.push_back(start);
    while(!q.empty()){
        QString current=q.front();
        q.pop_front();
        int quit=1;
        //除了经过终点站的线路外，其余线路都已访问，退出搜索
        for(auto it=LineVisited.begin();it!=LineVisited.end();it++){
            if(!Stations[end].LinesAcrossStation.contains(it.key())&&it.value()==0){
                quit=0;
                break;
            }
        }
        if(quit==1)
            break;
        //对当前站点上的所有线路进行搜索，目标是找到经过最少线路数的路径
        for(const auto&line:Stations[current].LinesAcrossStation){
            //对线路上的所有站点设置前继站点并放入队列
            for(const auto& station:Lines[line].stationsInline){
                //非终点站
                if(!path.contains(station)&&station!=end){
                    path[station].push_back(current);
                    q.push_back(station);
                    //qDebug()<<station;
                }
                //终点站
                else if(station==end){
                    path[station].push_back(current);
                    //qDebug()<<current;
                }
            }
            //标记访问没有经过终点站的线路
            if(!Stations[end].LinesAcrossStation.contains(line))
                LineVisited[line]=1;
        }
    }

    if(path.contains(end)){
        int least_Transfer=INT_MAX;
        int index;
        double min_Dist=INT_MAX;
        QMap<int,QVector<QString>>stations;//记录不同路径的换乘站列表
        QMap<int,QVector<QString>>allStations;//记录不同路径的站点列表
        QMap<int,QVector<Edge>>allEdges;//记录不同路径的边列表
        //遍历所有路径
        for(int i=0;i<path[end].size();i++){
            int count=0,num=0;
            double dist=0;
            //得到换乘站列表
            QString station=path[end][i];
            stations[i].push_front(end);
            while(station!=""){
                stations[i].push_front(station);
                station=path[station][0];
            }
            //根据换乘站得到所有站点的列表
            getMidStations(stations[i],allStations[i],allEdges[i]);
            count=allStations[i].size();
            num=stations[i].size();
            //计算路径总长度
            for(int j=0;j<count;j++){
                if(j<count-1)
                    dist+=calculateDist(allStations[i][j],allStations[i][j+1]);
            }
//            for(auto j:stations[i])
//                qDebug()<<j;
//            qDebug()<<dist;
            //路径最短且换乘数量不多于其他方案
            if(dist<min_Dist&&num<=least_Transfer){
                min_Dist=dist;
                least_Transfer=num;
                index=i;
                //qDebug()<<dist;
            }
            //换乘数最少，优先级更高
            else if(num<least_Transfer){
                least_Transfer=num;
                index=i;
            }
            //qDebug()<<i;
        }
        judge=stations[index].size()-1;
        stationList=allStations[index];
        edgeList=allEdges[index];
    }
    else
        judge=0;
    return judge;
}

//添加线路
void MetroSystem::addLine(QString name,QColor color){
    Line line;
    line.name=name;
    line.lineColor.setRgba(color.rgba());
    line.id=++this->num;
    Lines.insert(name,line);
}

//添加站点
void MetroSystem::addStation(QString name,double longitude,double latitude){
    Station station;
    station.name=name;
    station.longitude=longitude;
    station.latitude=latitude;
    //更新最大和最小经纬度
    minLongitude=std::min(longitude,minLongitude);
    maxLongitude=std::max(longitude,maxLongitude);
    minLatitude=std::min(latitude,minLatitude);
    maxLatitude=std::max(latitude,maxLatitude);
    Stations.insert(name,station);
}

//添加连接
void MetroSystem::addConnection(QString station1,QString station2,QString line){
    Lines[line].stationsInline.insert(station1);
    Lines[line].stationsInline.insert(station2);
    int num=Lines[line].stationsInline.size()+1;
    if(!Stations[station1].LinesAcrossStation.contains(line)){
        Stations[station1].LinesAcrossStation.push_back(line);
        Stations[station1].ids.insert(indexInLine(line,num++));
    }
    if(!Stations[station2].LinesAcrossStation.contains(line)){

        Stations[station2].LinesAcrossStation.push_back(line);
        Stations[station2].ids.insert(indexInLine(line,num));
    }
    if(!edges.contains(Edge(station1,station2)))
        edges.insert(Edge(station1,station2));

}

