#include "influxclient.h"
#include <string>
#include <sstream>

InfluxClient* InfluxClient::singleInstance = nullptr;

InfluxClient *InfluxClient::getInstance()
{
    //判断是否是第一次调用
    //假如不为NULL,会直接return
    if (singleInstance == nullptr) {

        //防止多进程同时调用创建2个对象的问题
        //原理是因为是静态变量只会定义一次
        static QMutex mutex;

        //上锁  QMutexLocker会锁住mutex，当QMutexLocker 被释放的时候会自动解锁
        //locker是局部变量，所以getInstance（）函数结束之后会自动解锁
        QMutexLocker locker(&mutex);

        //在判断一次,防止其它进程抢先new过了
        if (singleInstance == NULL) {
            singleInstance = new InfluxClient;
        }
    }
    return singleInstance; //返回指针
}


//void InfluxClient::initInflux()
//{
//    qDebug() << "write influx" << currProName;
//    db->write(influxdb::Point{"Experiment_Record"}.
//                    addTag("ExperimentName", currProName.toStdString()).
//                    addField("Experiment", 1));
//}

// 新建工程时会调用，向Experiment_Record表中写入最新新建或打开的这个工程的名字
//void InfluxClient::initInflux(const std::string& currProName)
//{
//    db->write(influxdb::Point{"Experiment_Record"}.
//                    addTag("ExperimentName", currProName).
//                    addField("Experiment", 1));
//}
// 更新实验名称到数据库中
void InfluxClient::initInflux(const std::string& currProName)
{
    db->write(influxdb::Point{"Experiment_Record"}.
                    addTag("ExperimentName", currProName).
                    addField("Experiment", 1));
}
// 记录采样频率
void InfluxClient::writeMeasurefreq(const std::string& measureName, int freq)
{
    db->write(influxdb::Point{"Measure_Freq"}.
                    addTag("measureName", measureName).
                    addField("freq", freq));
}

// 获取采样频率
//std::vector<Point> InfluxClient::getMeasureFreq(const std::string& measureName)
//{
//    // 拼接查询语句
//    std::string sqlQuery = "SELECT * FROM Measure_Freq WHERE measureName = '" + measureName + "'";
//    std::cout << "getMeasureFreq-sqlQuery: " << sqlQuery << std::endl;
//    // 从数据库中读取选择通道的数据
//    std::vector<Point> resPoint =
//            db->query(sqlQuery);
//    return resPoint;
//}

std::vector<Point> InfluxClient::getMeasureFreq(const std::string& measureName)
{
    // 拼接查询语句，按时间倒序，取最近写入的一条
    std::string sqlQuery = "SELECT * FROM Measure_Freq WHERE measureName = '"
                           + measureName + "' ORDER BY time DESC LIMIT 1";

    std::cout << "getMeasureFreq-sqlQuery: " << sqlQuery << std::endl;

    // 查询并返回
    std::vector<Point> resPoint = db->query(sqlQuery);
    return resPoint;
}


void InfluxClient::writeCondition(int condition)
{
    db->write(influxdb::Point{"condition"}.
                    addTag("type", "collection").
              addField("condition", condition));
}

std::vector<Point> InfluxClient::influxQuery(QString command)
{
    return db->query(command.toStdString());
}

// 动态SQL语句, SQL语句拼接
std::string buildInfluxDBQuery(const std::string& measureName,
                               const std::string& oldBeginTime,
                               const std::string& newBeginTime,
                               const std::string& id) {
    std::stringstream query;

    // Constructing the InfluxDB query using InfluxQL
    query << "SELECT * FROM \"" << measureName << "\" "
          << "WHERE \"ID\" = " << id << " AND "
          << "time >= '" << oldBeginTime << "' AND "
          << "time <= '" << newBeginTime << "'";

    return query.str();
}
std::vector<Point> InfluxClient::readAllData(
        const std::string& measureName ,std::string id)
{
    // 拼接查询语句
    std::string sqlQuery = "SELECT * FROM \"" + measureName + "\" WHERE \"ID\" = " + id;
    std::cout << "influxclient.cpp-readAllData-sqlQuery: " << sqlQuery << std::endl;
    // 从数据库中读取选择通道的数据
    std::vector<Point> resPoint =
            db->query(sqlQuery);
    qDebug() << "influxclient.cpp-readAllData-sqlQuery 1s data size is : " << resPoint.size();
    return resPoint;
}

// 读出单个或者多个通道的数据
std::vector<Point> InfluxClient::readAllChanneldata(
        const std::string& measureName)
{
    // 拼接查询语句
    std::string sqlQuery = "SELECT * FROM " + measureName;
    std::cout << "influxclient.cpp-readList-sqlQuery: " << sqlQuery << std::endl;
    // 从数据库中读取选择通道的数据
    std::vector<Point> resPoint =
            db->query(sqlQuery);
    qDebug() << "influxclient.cpp-readList-sqlQuery 1s data size is : " << resPoint.size();
    return resPoint;
}

std::vector<Point> InfluxClient::readOriginData(
        const std::string& measureName , const std::string& oldBeginTime,const std::string& newBeginTime ,std::string id)
{
    // measureName 表名
    // 拼接查询语句
    std::string sqlQuery = buildInfluxDBQuery(measureName, oldBeginTime, newBeginTime, id);
    std::cout << "influxclient.cpp--sqlQuery: " << sqlQuery <<std::endl;
    // 从数据库中读取选择通道的数据
    std::vector<Point> resPoint =
            db->query(sqlQuery);
    qDebug() << "influxclient.cpp--sqlQuery 1s data size is : " << resPoint.size();
    return resPoint;
}

std::vector<Point> InfluxClient::readOriginData2(
        const std::string& measureName , const std::string& oldBeginTime,const std::string& newBeginTime)
{
    // measureName 表名
    // 拼接查询语句
    std::stringstream query;
    // Constructing the InfluxDB query using InfluxQL
    query << "SELECT * FROM \"" << measureName << "\" "
          << "WHERE time >= '" << oldBeginTime << "' AND "
          << "time <= '" << newBeginTime << "'";
    std::string sqlQuery = query.str();

    std::cout << "influxclient.cpp-readOriginData2-sqlQuery: " << sqlQuery <<std::endl;
    // 从数据库中读取选择通道的数据
    std::vector<Point> resPoint =
            db->query(sqlQuery);
    qDebug() << "readOriginData2-sqlQuery 1s data size is : " << resPoint.size();
    return resPoint;
}

//std::vector<Point> InfluxClient::readOriginData(
//        QStandardItem *groupItem, int id)
//{
//    // 从数据库中读取选择通道的数据
//    QString measureName = currProName + "_" +
//    Global::getInfluxText(groupItem) + "_OriginData";
//    QString command = "select * from " + measureName +
//            " where time > now() - 1s and ID = '" +
//            QString::number(id) + "'";
//    std::vector<Point> resPoint =
//            db->query(command.toStdString());
//    qDebug() << "1s data size is : " << resPoint.size();
//    return resPoint;
//}

//bool InfluxClient::getTimeBound()
//{
//    if(channelModel)
//    {
//        QQueue<QStandardItem*> queue;
//        QStandardItem* rootItem = channelModel->invisibleRootItem();
//        for(int i = 0; i < rootItem->rowCount(); ++i)
//        {
//            queue.enqueue(rootItem->child(i));
//        }
//        while(!queue.empty())
//        {
//            // 源节点、复制节点一前一后
//            QStandardItem* childItem = queue.dequeue();
//            NodeData nodedata =
//                    childItem->data(Qt::UserRole).value<NodeData>();
//            // 遍历到通道节点，执行操作并结束递归
//            if(nodedata.name == "Group")
//            {
//                // 找到一个分组节点就退出，只需要利用一个分组节点查找时间边界
//                // 从数据库中读取选择通道的数据
//                QString measureName = currProName + "_" +
//                Global::getInfluxText(childItem) + "_OriginData";
//                std::vector<Point> resPoint;
//                // 先查一次所有的记录下开始和截止时间
//                // 如果以一秒为时间间隔查找到数据就继续按一秒查找，否则遇到不连续时间段就更新查找起点
//                // 直到endTime才停止给数据

//                QString command = "select * from " + measureName;
//                //resPoint = db->query("show databases");
//                resPoint = db->query(command.toStdString());
//                if(!resPoint.empty())
//                {
//                    std::chrono::system_clock::time_point beginTimeStamp =
//                            resPoint.front().getTimestamp() + std::chrono::hours(8);
//                    std::chrono::system_clock::time_point endTimeStamp =
//                            resPoint.back().getTimestamp() + std::chrono::hours(8);
//                    long long beginNano = std::chrono::duration_cast<std::chrono::nanoseconds>(
//                            beginTimeStamp.time_since_epoch()).count();
//                    long long endNano = std::chrono::duration_cast<std::chrono::nanoseconds>(
//                            endTimeStamp.time_since_epoch()).count();
////                    qDebug() << "begin time is : " << beginNano;
////                    qDebug() << "end time is : " << endNano;

//                    QString beginTime = QString::number(beginNano);
//                    QString endTime = QString::number(endNano);
//                    qDebug() << "begin time is : " << beginTime;
//                    qDebug() << "end time is   : " << endTime;
//                    // 记录时间上下限
//                    timeBound = {beginTime, endTime};
//                    // 查找一次即可退出
//                    return true;
//                }
//                // 没有数据
//                else
//                {
//                    qDebug() << "no data";
//                    return false;
//                }

//            }
//            // 如果不是通道节点，继续遍历
//            for(int i = 0; i < childItem->rowCount(); ++i)
//            {
//                queue.enqueue(childItem->child(i));
//            }
//        }
//    }
//    return false;

//}

void InfluxClient::increaseTime()
{
    QString oldBeginTime = timeBound.first;
    // 转化为纳秒之后相加，再转成字符串进行查询
    QString newBeginTime = QString::number(oldBeginTime.toULongLong()
                + (long long)1000000000);
    // 更新时间起始点
    timeBound.first = newBeginTime;
}

//std::vector<Point> InfluxClient::replayOriginData(QStandardItem *groupItem, int id)
//{
//    // 从数据库中读取选择通道的数据
//    QString measureName = currProName + "_" +
//    Global::getInfluxText(groupItem) + "_OriginData";
//    std::vector<Point> resPoint;
//    // 先查一次所有的记录下开始和截止时间
//    // 如果以一秒为时间间隔查找到数据就继续按一秒查找，否则遇到不连续时间段就更新查找起点
//    // 直到endTime才停止给数据

//    QString oldBeginTime = timeBound.first;
//    // 转化为纳秒之后相加，再转成字符串进行查询
//    QString newBeginTime = QString::number(oldBeginTime.toULongLong()
//                + (long long)1000000000);
////    // 更新时间起始点，只能在定时器里累加
////    timeBound.first = newBeginTime;

//    QString command = "select * from " + measureName +
//            " where time >= " + oldBeginTime + " and time < "
//            + newBeginTime + " and ID = '" +
//            QString::number(id) + "'";
////    QString command = "select * from " + measureName +
////            " where time > now() - 1s and ID = '" +
////            QString::number(id) + "'";
//    resPoint = db->query(command.toStdString());
//    // 如果查询结果为空且未到时间结束点，表示出现非连续采样时间段，则更新时间起始点
//    if(resPoint.empty())
//    {
//        // 出现非连续时间段
//        if(oldBeginTime.toULongLong() < timeBound.second.toULongLong())
//        {
//            // 将起始点移动至下一个时间段的起点，重新查找一次
//            QString command = "select * from " + measureName +
//                    " where time >= " + newBeginTime + " and ID = '" +
//                    QString::number(id) + "'";
//            resPoint = db->query(command.toStdString());
//            std::chrono::system_clock::time_point beginTimeStamp =
//                    resPoint.front().getTimestamp() + std::chrono::hours(8);
//            long long beginNano = std::chrono::duration_cast<std::chrono::nanoseconds>(
//                    beginTimeStamp.time_since_epoch()).count();
//            qDebug() << "next begin time is : " << beginNano;
//            QString beginTime = QString::number(beginNano);
////            // 存储的纳秒数
////            QString beginTime = QString::number(resPoint.front().
////                    getTimestamp().time_since_epoch().count());
////            // 更新时间起始点，继续查找
//            timeBound.first = beginTime;

//            resPoint = replayOriginData(groupItem, id);
//        }
//        // 回放到结束点
//        else
//        {
//            // 直接返回一个空的vector
//            return resPoint;
//        }
//    }
//    qDebug() << "1s data size is : " << resPoint.size();
//    // 返回一秒数据，如果为空说明回放完了
//    return resPoint;
//}


std::string InfluxClient::QvecToBaseStr(QVector<double> array)
{
    int arr_size = array.size();
    double* data_float = new double[arr_size];
    char* data_char = new char[arr_size * sizeof(double)];
    // 数据由QVector -> float*
    for(int i = 0; i < arr_size; ++i){
        data_float[i] = array[i];
    }
    memcpy(data_char, data_float, arr_size * sizeof(double));
    // base64编码中转
    QByteArray text(data_char, arr_size * sizeof(double));
    // 转成base编码存储
    std::string base_str = text.toBase64().toStdString();
    return base_str;
}

QVector<double> InfluxClient::BaseStrToQvec(std::string base_str)
{
//    // base64编码转普通hex
//    QByteArray text_hex = QByteArray::fromBase64(base_str.c_str());
//    std::string hex_str = text_hex.toStdString();
//    // 根据memcpy之后的字节流计算原始数组的长度，默认类型为float
//    int arr_size = hex_str.length() / sizeof(float);
//    //cout << "base size is : " << hex_str.length() << endl;
//    float* data_float = new float[arr_size];
//    memcpy(data_float, hex_str.c_str(), arr_size * 4);
//    //cout << "-----------base output------------" << endl;
//    //cout.precision(7);
//    QVector<double> array;
//    // float* -> QVector
//    for(int i = 0; i < arr_size; ++i){
//        array.push_back(data_float[i]);
//        //cout << data_float[i] << " ";
//    }
//    //cout << endl;
//    return array;

    // base64编码转普通hex
    QByteArray text_hex = QByteArray::fromBase64(base_str.c_str());
    std::string hex_str = text_hex.toStdString();
    // 根据memcpy之后的字节流计算原始数组的长度，默认类型为float
    int arr_size = hex_str.length() / sizeof(double);
    //cout << "base size is : " << hex_str.length() << endl;
    double* data_double = new double[arr_size];
    memcpy(data_double, hex_str.c_str(), arr_size * 8);
    //cout << "-----------base output------------" << endl;
    //cout.precision(7);
    QVector<double> array;
    // float* -> QVector
    for(int i = 0; i < arr_size; ++i){
        array.push_back(data_double[i]);
        //cout << data_float[i] << " ";
    }
    //cout << endl;
    return array;
}

std::string InfluxClient::BaseStrToStr(std::string base_str)
{
    // base64编码转普通hex
    QByteArray text_hex = QByteArray::fromBase64(base_str.c_str());
    std::string hex_str = text_hex.toStdString();
    // 根据memcpy之后的字节流计算原始数组的长度
    int arr_size = hex_str.length() / sizeof(float);
    //cout << "base size is : " << hex_str.length() << endl;
    float* data_float = new float[arr_size];
    memcpy(data_float, hex_str.c_str(), arr_size * 4);
    //cout << "-----------base output------------" << endl;
    //cout.precision(7);
    std::string float_str;
    // float* -> string 叠加
    for(int i = 0; i < arr_size; ++i){
        float_str = float_str + std::to_string(data_float[i]) + " ";
        //cout << data_float[i] << " ";
    }
    //cout << endl;
    return float_str;
}


std::string InfluxClient::TimeStampToString(
        std::chrono::system_clock::time_point current_time)
{
    // 转换为时间结构
    std::time_t time = std::chrono::system_clock::to_time_t(current_time);
    // 将时间结构转换为字符串
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    std::string time_str = ss.str();
    return time_str;
}


    // 原方法针对每个id(通道)均要调用一次db->write函数，现已弃用
//void InfluxClient::writeOriginData(QString tableName,
//                    int id, QVector<std::string> baseStrs)
//{
//    // std::move可选
//    influxdb::Point writePoint = influxdb::Point{tableName.toStdString()}.
//            addTag("ID", std::to_string(id));
//    // 如果freq大于6k，则需要写入下一个Field
//    for(int i = 0; i < baseStrs.size(); ++i)
//    {
////        qDebug() << "baseStr is : " << QString::fromStdString(baseStrs[i]);
//        QString fieldName = "OriginData_" + QString::number(i + 1);
//        writePoint = writePoint.addField(fieldName.toStdString(), baseStrs[i]);
//    }
//    // 如果是没有值的Field,select的结果是空，例如查找OriginData_23结果是空
//    db->write(influxdb::Point(writePoint));
//}

// 写入数据库
void InfluxClient::writeOriginData(QString tableName,
                                   const QVector<int>& id_vec,
                                   const QVector<QVector<std::string>>& baseStrs_vec)
{
    qDebug() << "[InfluxClient] Entering writeOriginData";
    std::vector<influxdb::Point> writePoints;
    for (int k = 0; k < id_vec.size(); ++k)
    {
        influxdb::Point writePoint = influxdb::Point{tableName.toStdString()}
            .addTag("ID", std::to_string(id_vec[k]));
        const auto& baseStrs = baseStrs_vec[k];
        for (int i = 0; i < baseStrs.size(); ++i)
        {
            QString fieldName = "OriginData_" + QString::number(i + 1);
            writePoint = writePoint.addField(fieldName.toStdString(), baseStrs[i]);
        }
        writePoints.push_back(std::move(writePoint));
    }
    // 批量写入
    db->writeBatch(writePoints);
}

// 写一行，一行存储多个通道多秒的数据，写入数据库
void InfluxClient::writeOneLine_withTimeStamp(const std::string& measureName,
                                   std::chrono::time_point<std::chrono::system_clock> time,
                                   const std::vector<std::string>& baseStrsVec)
{
    influxdb::Point writePoint{measureName};
    writePoint.setTimestamp(time); // 时间设置

    // 获取字段名编号的最大位数，用于补零
    int maxFieldNumber = baseStrsVec.size();
    int fieldWidth = QString::number(maxFieldNumber).length(); // 动态计算补零位数

    // 数据设置addField，应用补零规则
    for (size_t i = 0; i < baseStrsVec.size(); ++i) {
        // 格式化字段名，补零到 fieldWidth 位
        QString fieldName = QString("OriginData_%1").arg(i + 1, fieldWidth, 10, QChar('0'));
        writePoint.addField(fieldName.toStdString(), baseStrsVec[i]);
    }

    try {
        db->write(std::move(writePoint)); // 写入单个点
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

// 20250414固定OriginData_0001 ~ OriginData_9999补零版本
void InfluxClient::writeOneLine(const std::string& measureName,
                                const std::vector<std::string>& baseStrsVec)
{
    influxdb::Point writePoint{measureName};

    // 自动使用数据库当前时间戳
    auto time = std::chrono::system_clock::now();
    writePoint.setTimestamp(time);

    const int fieldWidth = 4; // 固定4位宽度，不再根据通道数量变化

    for (size_t i = 0; i < baseStrsVec.size(); ++i) {
        // 格式化字段名为 OriginData_0001 ~ OriginData_9999
        QString fieldName = QString("OriginData_%1").arg(static_cast<int>(i + 1), fieldWidth, 10, QChar('0'));
        writePoint.addField(fieldName.toStdString(), baseStrsVec[i]);

        // 可选调试输出
        // qDebug() << "[InfluxClient] Add field:" << fieldName << ", value length:" << baseStrsVec[i].size();
    }

    try {
        db->write(std::move(writePoint));
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}


// 20250414自动补零版本
//void InfluxClient::writeOneLine(const std::string& measureName,
//                               const std::vector<std::string>& baseStrsVec)
//{
//    influxdb::Point writePoint{measureName};

//    // 自动使用数据库当前时间戳
//    auto time = std::chrono::system_clock::now();
//    writePoint.setTimestamp(time); // 自动使用当前时间戳

//    // 获取字段名编号的最大位数，用于补零
//    int maxFieldNumber = baseStrsVec.size();
//    int fieldWidth = QString::number(maxFieldNumber).length(); // 动态计算补零位数

//    // 数据设置addField，应用补零规则
//    for (size_t i = 0; i < baseStrsVec.size(); ++i) {
//        // 格式化字段名，补零到 fieldWidth 位
//        QString fieldName = QString("OriginData_%1").arg(i + 1, fieldWidth, 10, QChar('0'));
//        writePoint.addField(fieldName.toStdString(), baseStrsVec[i]);
//    }

//    try {
//        db->write(std::move(writePoint)); // 写入单个点
//    } catch (std::exception &e) {
//        std::cerr << e.what() << std::endl;
//    }
//}

InfluxClient::InfluxClient(QObject *parent) : QObject(parent)
{
    qDebug() << "create influx";
    // 创建数据库，前提要有 iGreatWorks_db
    db = influxdb::InfluxDBFactory::Get("http://localhost:8086?db=iGreatWorks_db");
//    db->write(influxdb::Point{"measure"}.
//              addTag("ID", std::to_string(1)).
//              addField("OriginFft1", "base"));

    //db->query("select * from measure");
//    qDebug() << "db is : " << db->write()
//    db->query("create database iGreatWorks_db");
    // use iGreatWorks_db
    // create database iGreatWorks_db
    //std::vector<Point> test = db->query("show databases");
    for (auto i: db->query("show databases"))
    {
        qDebug() << QString::fromStdString(i.getTags());
    }
    //auto i = db->query("SHOW DATABASES");
}


//QPair<QVector<int>, QVector<QVector<double>>> InfluxClient::readAllOriginData(QStandardItem *groupItem, int id)
//{
//    QString measureName = currProName + "_" + Global::getInfluxText(groupItem) + "_OriginData";
//    QString command = "SELECT * FROM " + measureName + " WHERE ID = '" + QString::number(id) + "'";

//    std::vector<Point> allDataPoints = db->query(command.toStdString());
//    QVector<int> timestamps;
//    QVector<QVector<double>> decodedDataVectors;

//    for (const auto& point : allDataPoints)
//    {
//        // 提取时间戳并转换为秒数，并加入8小时调整
//        uint32_t secTime = std::chrono::duration_cast<std::chrono::seconds>(
//                               point.getTimestamp().time_since_epoch()).count() + 8 * 3600;
//        timestamps.push_back(static_cast<int>(secTime));

//        // 获取字段集和标签集
//        Point::FieldSet fieldSet = point.getFieldSet();
//        Point::TagSet tagSet = point.getTagSet();

//        // 解码每个字段数据
//        QVector<double> singleTimeData;
//        for (const auto& tag : tagSet)
//        {
//            QString tagName = QString::fromStdString(tag.first);
//            if (tagName.startsWith("OriginData_"))
//            {
//                std::string baseStr = tag.second;
//                QVector<double> tempVec = BaseStrToQvec(baseStr);
//                singleTimeData.append(tempVec);
//            }
//        }

//        // 如果解析的数据为空，显示警告信息
//        if (singleTimeData.isEmpty())
//        {
//            qDebug() << "Warning: No decoded data found for timestamp:" << secTime;
//        }

//        decodedDataVectors.push_back(singleTimeData);
//    }

//    // 调试输出
//    qDebug() << "Loaded data for ID:" << id;
//    qDebug() << "Total Data Points:" << decodedDataVectors.size();

//    for (int i = 0; i < timestamps.size(); ++i)
//    {
//        qDebug() << "Timestamp:" << QDateTime::fromSecsSinceEpoch(timestamps[i]).toString("yyyy-MM-dd hh:mm:ss")
//                 << "Data size:" << decodedDataVectors[i].size();
//    }

//    return {timestamps, decodedDataVectors};
//}


