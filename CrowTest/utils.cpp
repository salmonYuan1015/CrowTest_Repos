#include "blue_data.h"
#include "crow.h"
#include <influxdb/include/Point.h>
#include "QDebug"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QVector>
#include <string>
#include <vector>
#include <iostream>
#include <QtCore/QDateTime>

using namespace std;
using namespace influxdb;

// 1. 将std::chrono::time_point<std::chrono::system_clock>转换为字符串（时间戳）
std::string timePointToString(const std::chrono::time_point<std::chrono::system_clock>& timePoint, bool asTimestamp = true) {

    // 将时间点转换为从1970年1月1日到该时间点的秒数（时间戳）
    std::time_t time_since_epoch = std::chrono::system_clock::to_time_t(timePoint);
    // std::cout << "时间戳（从1970年1月1日起的秒数）: " << time_since_epoch << std::endl;
    std::stringstream ss;
    ss << time_since_epoch;
    return ss.str();
}


// 2. Point转换为Json格式, 数据库一行不再存储通道ID
crow::json::wvalue vectorPointToJson2(const std::vector<influxdb::Point>& points) {
    crow::json::wvalue result;
    // 填充 "point" 数组部分
    result["point"] = crow::json::wvalue::list();

    for (const auto& point : points) {
        crow::json::wvalue singlePoint;
        // 填充时间戳（这里假设都转换为时间戳形式的字符串，你可以按需修改为格式化时间字符串形式）
        singlePoint["timestamp"] = timePointToString(point.getTimestamp());

        // 填充tags部分
        crow::json::wvalue tags;
        const auto& tagSet = point.getTagSet();
        for (const auto& tag : tagSet) {
            tags[tag.first] = tag.second;
        }
        singlePoint["tags"] = std::move(tags);

        // 获取当前result["point"]的长度，以此作为新元素的索引进行赋值
        std::size_t index = result["point"].size();
        result["point"][index] = std::move(singlePoint);


    }
    return result;
}

// 3. 过滤数据，得到应该返回的数据的列名
QVector<QString> filterTags(const crow::json::rvalue &list, int m) {
    QVector<QString> filtList;
    for (size_t i = 0; i < list.size(); ++i) { // 遍历 crow::json::type::List
        auto item = list[i].i(); // 将 crow::json::type::List 的元素转换为 int 类型
        for (int j = 1; j <= m; j++) {
            int tmp = m * (item - 1) + j;
            QString fieldName = "OriginData_" + QString::number(tmp);
            filtList.append(fieldName);
        }
    }
    return filtList;
}

// 判断是不是需要返回前端的列
bool contains(const QVector<QString> &list, const std::string &value) {
    QString qValue = QString::fromStdString(value); // 将std::string转换为QString
    return std::find(list.begin(), list.end(), qValue) != list.end();
}

// 5. Point转换为Json格式, 并且进行数据过滤。数据库一行不再存储通道ID
crow::json::wvalue vectorPointToJson3(const std::vector<influxdb::Point>& points, int m, const crow::json::rvalue &list) {
    crow::json::wvalue result;
    // 填充 "point" 数组部分
    result["point"] = crow::json::wvalue::list();
    result["MOD"] = m;

    for (const auto& point : points) {
        crow::json::wvalue singlePoint;
        // 填充时间戳（这里假设都转换为时间戳形式的字符串，你可以按需修改为格式化时间字符串形式）
        singlePoint["timestamp"] = timePointToString(point.getTimestamp());

        // 填充tags部分
        crow::json::wvalue tags;
        const auto& tagSet = point.getTagSet();
        // 得到返回数据的列名
        QVector<QString> filterList =  filterTags(list, m);

        for (const auto& tag : tagSet) {
            // std::cout << "Type of tagSet: " << typeid(tag.first).name() << std::endl;
            // std::cout << "tag.first: " <<  tag.first << std::endl;
            if (contains(filterList, tag.first)){
                tags[tag.first] = tag.second;
            }
        }
        singlePoint["tags"] = std::move(tags);

        // 获取当前result["point"]的长度，以此作为新元素的索引进行赋值
        std::size_t index = result["point"].size();
        result["point"][index] = std::move(singlePoint);

    }
    return result;
}


// 6. json数据转换。将QString jsonString中的baseStrs_vec数据转换为QVector<QVector<std::string>>, 用于写入数据库
QVector<QVector<std::string>> jsonTransform(QString jsonString){
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject jsonObj = doc.object();
    // 提取baseStrs_vec
    QJsonArray baseStrsArray = jsonObj["baseStrs_vec"].toArray();
    QVector<QVector<std::string>> baseStrs_vec;
    for (const QJsonValue &rowValue : baseStrsArray) {
        QVector<std::string> row;
        QJsonArray rowArray = rowValue.toArray();
        for (const QJsonValue &strValue : rowArray) {
            // 将QString转换为std::string
            row.push_back(strValue.toString().toStdString());
        }
        baseStrs_vec.append(row);
    }
    return baseStrs_vec;
}

// 7. 将一个ISO 8601格式的字符串（如 "2025-03-05T07:03:00Z"）转换为std::chrono::time_point<std::chrono::system_clock>
std::chrono::time_point<std::chrono::system_clock> convertIso8601ToTimePoint(const std::string& iso8601) {
    // 从std::string创建QString
    QString qStringTime = QString::fromStdString(iso8601);

    // 使用QDateTime解析ISO 8601格式的时间字符串
    QDateTime dateTime = QDateTime::fromString(qStringTime, Qt::ISODate);
    // 确保时间是UTC时间
    if (dateTime.timeSpec() != Qt::UTC) {
        dateTime.setTimeSpec(Qt::UTC);
    }
    // 转换为自纪元以来的秒数
    std::intmax_t unixTime = dateTime.toSecsSinceEpoch();
    // 创建并返回对应的时间点
    return std::chrono::system_clock::from_time_t(static_cast<time_t>(unixTime));
}


