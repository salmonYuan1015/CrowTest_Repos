#include "blue_data.h"
#include "crow.h"
#include "curl/curl.h"
#include "QDebug"
#include <influxdb/include/InfluxDBFactory.h>
#include <influxdb/include/Point.h>
#include <cstdlib>  // 用于 system()
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QVector>
#include <string>
#include "Interface/influxclient.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <chrono>
#include <QtCore/QDateTime>
#include "utils.h"

using namespace std;
using namespace influxdb;

// 返回通道频率
int chennalL(const std::string &measurename){
    try {
        // 读取数据库。
        std::vector<Point> resPoint = InfluxClient::getInstance()->getMeasureFreq(measurename);
        // 检索
        for (const auto& point : resPoint) {
//            QString tag = QString::fromStdString(point.getTags());
//            qDebug() <<"打印记录---"<< tag;
            // 获取field字段
            QString fieldStr = QString::fromStdString(point.getFields());
            qDebug() <<"打印记录field---"<< fieldStr;
            int f = 0; // 默认值
            // 直接查找 "freq=" 并提取后面的数值
            int freqPos = fieldStr.indexOf("freq=");
            if (freqPos != -1) {
                QString valueStr = fieldStr.mid(freqPos + 5); // 从 "freq=" 后面开始
                double freqDouble = valueStr.toDouble(); // 直接转换，不检查结果
                f = static_cast<int>(freqDouble); // 转换为 int
                qDebug() << "freq 的值是:" << f;
                return f;
//                // 提取 "freq=" 后面的部分
//                QString valueStr = fieldStr.mid(freqPos + 5).trimmed(); // 移除可能的空白字符
//                // 截取到第一个非数字字符（如果有）
//                int nonDigitPos = 0;
//                while (nonDigitPos < valueStr.size() && (valueStr[nonDigitPos].isDigit() || valueStr[nonDigitPos] == '.')) {
//                    ++nonDigitPos;
//                }
//                valueStr = valueStr.left(nonDigitPos);
//                // 将字符串转换为 double 或 int
//                bool ok;
//                int f = valueStr.toDouble(&ok);
//                qDebug() << "freq 的值是:" << f;
            } else {
                qDebug() << "未找到 freq 字段";
            }
        }
        return 0;
    }
    catch (const std::exception& e) {
        return 0;
    }

}


//用类去封装独立的蓝图。将数据库相关操作解耦
blue_data::blue_data():bp("data") {

    //    1. 读出单个或者多个通道的数据
    //    {
    //    "measureName":"influx_measurement", // (string)
    //    "id_vec": [2,3],
    //    }
    CROW_BP_ROUTE(bp, "/channelList").methods("POST"_method)
            ([](const crow::request& req) {
            // 解析传入的JSON数据
            try {
                crow::json::rvalue json_data = crow::json::load(req.body);

                std::cout << "Type of json_data[id_vec]: " << typeid(json_data["id_vec"]).name() << std::endl;
                // 频率
                int freq= chennalL(json_data["measureName"].s());
                // std::cout<<freq<<std::endl;
                // 计算一个通道20秒占的列数
                int m = (freq * 20 + 999) / 1000;

                // 读取数据库。读一张表中所有数据
                std::vector<Point> resPoint = InfluxClient::getInstance()->readAllChanneldata(json_data["measureName"].s());

                // 打印resPoint是否为空
                if(!resPoint.empty()){
                    qDebug() << "command_query success!";
                }else{
                    qDebug() << "command_query ERROR!";
                }

                // 转换, m为一个通道占的列数
                crow::json::wvalue  response_json = vectorPointToJson3(resPoint, m, json_data["id_vec"]);

                // 构造要返回的JSON响应
                response_json["status"] = "success";
                response_json["res_size"] = resPoint.size();
                // 返回JSON响应
                return crow::response(200, response_json);
            }
            catch (const std::exception& e) {
                // 如果解析失败，返回错误信息
                crow::json::wvalue error_response;
                error_response["status"] = "error";
                error_response["message"] = e.what();
                return crow::response(400, error_response);
            }
                });


    //    2. 读出所有通道的数据
    //    {
    //    "measureName":"b_measurement", // (string)
    //    }
    CROW_BP_ROUTE(bp, "/all").methods("POST"_method)
            ([](const crow::request& req) {
            // 解析传入的JSON数据
            try {
                crow::json::rvalue json_data = crow::json::load(req.body);
                // 打印接收到的数据
                std::cout << "Received JSON data: " << json_data;

                // 读取数据库。读一张表中所有数据
                std::vector<Point> resPoint = InfluxClient::getInstance()->readAllChanneldata(json_data["measureName"].s());

                // 打印resPoint中各个Point的数值
                if(!resPoint.empty()){
                    qDebug() << "command_query success!";
                }else{
                    qDebug() << "command_query ERROR!";
                }
                // 转换
                crow::json::wvalue  response_json = vectorPointToJson2(resPoint);

                // 构造要返回的JSON响应
                response_json["status"] = "success";
                response_json["res_size"] = resPoint.size();
                // 返回JSON响应
                return crow::response(200, response_json);
            }
            catch (const std::exception& e) {
                // 如果解析失败，返回错误信息
                crow::json::wvalue error_response;
                error_response["status"] = "error";
                error_response["message"] = e.what();
                return crow::response(400, error_response);
            }
                });


    //    3. 写一行。数据库不存储通道号，通道数据按顺序存储
    //    {
    //    "measureName":"b_measurement",
    //    "time": "2025-03-05T07:03:00Z",
    //    "baseStrs_vec": ["a1","a2","a3","b1","b2","b3"],
    //    }
    CROW_BP_ROUTE(bp, "/writeLine").methods("POST"_method)
        ([](const crow::request& req) {
        // 解析传入的JSON数据
        try {
            crow::json::rvalue json_data = crow::json::load(req.body);
            // 打印接收到的数据
            std::cout << "Received JSON data: " << json_data;
            crow::json::rvalue baseStrsVecJson = json_data["baseStrs_vec"];

            // 将 baseStrsVecJson 转换为 std::vector<std::string>
            std::vector<std::string> baseStrsVec;
            for (const auto& item : baseStrsVecJson) {
                if (item.t() == crow::json::type::String) {
                    baseStrsVec.push_back(item.s());
                } else {
                    return crow::response(400, "Invalid data type in baseStrs_vec");
                }
            }

            // 时间格式转换
//            std::chrono::time_point<std::chrono::system_clock> time = convertIso8601ToTimePoint(json_data["time"].s());

            // 写入数据库
//            InfluxClient::getInstance()->writeOneLine(json_data["measureName"].s(), time, baseStrsVec);
            InfluxClient::getInstance()->writeOneLine(json_data["measureName"].s(), baseStrsVec);

            // 构造要返回的JSON响应
            crow::json::wvalue response_json;
            response_json["status"] = "success";

            // 返回JSON响应
            return crow::response(200, response_json);
        }
        catch (const std::exception& e) {
            // 如果解析失败，返回错误信息
            crow::json::wvalue error_response;
            error_response["status"] = "error";
            error_response["message"] = e.what();
            return crow::response(400, error_response);
        }
            });


    //请求访问http://localhost:18080/data/2
    CROW_BP_ROUTE(bp, "/2")([] {
        return "蓝图模块1.2";
        });

}
