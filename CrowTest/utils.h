#ifndef UTILS_H
#define UTILS_H


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
std::string timePointToString(const std::chrono::time_point<std::chrono::system_clock>& timePoint, bool asTimestamp = true);

// 2. Point转换为Json格式, 数据库一行不再存储通道ID
crow::json::wvalue vectorPointToJson2(const std::vector<influxdb::Point>& points);

// 3. 过滤数据，得到应该返回的数据的列名
QVector<QString> filterTags(const crow::json::rvalue &list, int m);

// 4. 判断是不是需要返回前端的列
bool contains(const QVector<QString> &list, const std::string &value);

// 5. Point转换为Json格式, 并且进行数据过滤。数据库一行不再存储通道ID
crow::json::wvalue vectorPointToJson3(const std::vector<influxdb::Point>& points, int m, const crow::json::rvalue &list);

// 6. json数据转换。将QString jsonString中的baseStrs_vec数据转换为QVector<QVector<std::string>>, 用于写入数据库
QVector<QVector<std::string>> jsonTransform(QString jsonString);

// 7. 将一个ISO 8601格式的字符串（如 "2025-03-05T07:03:00Z"）转换为std::chrono::time_point<std::chrono::system_clock>
std::chrono::time_point<std::chrono::system_clock> convertIso8601ToTimePoint(const std::string& iso8601);


#endif // UTILS_H
