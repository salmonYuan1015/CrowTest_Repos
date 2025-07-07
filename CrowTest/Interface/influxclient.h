#ifndef INFLUXCLIENT_H
#define INFLUXCLIENT_H

#include <QObject>
#include <iostream>
#include <vector>
#include <QMutex>
#include <QDebug>
#include <influxdb/include/InfluxDBFactory.h>
// 数据库格式文件
#include <influxdb/include/Point.h>


#include <sstream>
#include <fstream>

#include <iomanip>
#include <ctime>
#include <chrono>
#include <string>

using namespace influxdb;

class InfluxClient : public QObject
{
    Q_OBJECT
public:

//    explicit InfluxClient(QObject *parent = nullptr);

    static InfluxClient* getInstance();

    // 更新实验名称到数据库中
    void initInflux(const std::string& currProName);
    // 记录采样频率
    void writeMeasurefreq(const std::string& measureName, int freq);

    // 获取采样频率
    std::vector<Point> getMeasureFreq(const std::string& measureName);

    //写入标志位到数据库，用于控制采样状态，condition = -1:关闭进程，0暂停采样，1开始采样
    void writeCondition(int condition);

    std::vector<Point> influxQuery(QString command);
    // 查找最近一秒钟对应通道的原始数据 【采样模式】用
//    std::vector<Point> readOriginData(QStandardItem* groupItem, int id);
    std::vector<Point> InfluxClient::readOriginData(const std::string& measureName, const std::string& oldBeginTime,const std::string& newBeginTime ,std::string id);
    std::vector<Point> InfluxClient::readOriginData2(const std::string& measureName, const std::string& oldBeginTime,const std::string& newBeginTime);
    std::vector<Point> InfluxClient::readAllData(const std::string& measureName, std::string id);
    // 读出表中所有数据（所有通道的数据）
    std::vector<Point> InfluxClient::readAllChanneldata(const std::string& measureName);

    // 【回放模式】时间起止点，起始为空
    QPair<QString, QString> timeBound;

    // 为时间起止点赋值，有数据返回true
    bool getTimeBound();

    // 回放时间自加一秒
    void increaseTime();

    // 一次性读取数据库中的数据，然后按每秒时间节拍更新全局变量中的原始数据 【回放模式】用
//    std::vector<Point> replayOriginData(QStandardItem* groupItem, int id);



    // 与数据库交互的数据类型转换
    // QVector<double> -> baseStr存数据库
    std::string QvecToBaseStr(QVector<double> array);
    // 数据库 baseStr -> QVector<double>
    QVector<double> BaseStrToQvec(std::string base_str);
    // str -> baseStr
    std::string BaseStrToStr(std::string base_str);
    // 时间戳 -> str
    std::string TimeStampToString(std::chrono::system_clock::
                                  time_point current_time);


    ///
    /// \brief writeOriginData
    /// \param tableName 写入的表格名称
    /// \param baseStrs 经过内存拷贝 + 编码转换之后的str写入数据库
    /// 如果是大文件，原始数据一行需要写入多个field
    ///

    // 原方法针对每个id(通道)均要调用一次db->write函数，现已弃用
//    void writeOriginData(QString tableName,
//                         int id, QVector<std::string> baseStrs);

    void writeOriginData(QString tableName,
                         const QVector<int>& id_vec,
                         const QVector<QVector<std::string>>& baseStrs_vec);

//  包含时间戳，现已更名为writeOneLine_withTimeStamp
//    void InfluxClient::writeOneLine(const std::string& measureName,
//                                       std::chrono::time_point<std::chrono::system_clock> time,
//                                       const std::vector<std::string>& baseStrsVec);

    // 用于DataManager类的一次性读取全部数据功能函数
//    QPair<QVector<int>, QVector<QVector<double>>>
//                        readAllOriginData(QStandardItem *groupItem, int id);


    // 不包含时间戳  写一行，一行存储多个通道多秒的数据，写入数据库
    void writeOneLine(const std::string &measureName, const std::vector<std::string> &baseStrsVec);

    // 包含时间戳  写一行，一行存储多个通道多秒的数据，写入数据库
    void writeOneLine_withTimeStamp(const std::string &measureName,
                                    std::chrono::time_point<std::chrono::system_clock> time,
                                    const std::vector<std::string> &baseStrsVec);
private:
    // 静态指针
    static InfluxClient* singleInstance;
    explicit InfluxClient(QObject *parent = nullptr);

    std::unique_ptr<InfluxDB> db;

signals:

};

#endif // INFLUXCLIENT_H
