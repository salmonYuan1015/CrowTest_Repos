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
#include "blue_data.h"

using namespace std;
using namespace influxdb;


// 验证用户登录信息, 仅用于测试，实际要访问数据库查询并判断
bool verify_login(const std::string& username, const std::string& password) {
    if (username == "Test" && password=="123456") {  // 用户名密码写死，用于测试
        return true; // 简单对比密码，仅用于测试
    }
    return false;
}

int main() {
    crow::SimpleApp app;

    blue_data blue_data1;
    app.register_blueprint(blue_data1.bp);

    // 初始化, initInflux
    CROW_ROUTE(app, "/init").methods("POST"_method)
        ([](const crow::request& req) {
        // 解析传入的JSON数据
        try {
            crow::json::rvalue json_data = crow::json::load(req.body);
            // 打印接收到的数据（仅用于调试）
            std::cout << "Received JSON data: " << json_data << std::endl;

            // 数据库操作
            InfluxClient::getInstance()->initInflux(json_data["measureName"].s());

            // 构造要返回的JSON响应
            crow::json::wvalue response_json;
            response_json["status"] = "success";
            response_json["received_data"] = json_data;  // 返回接收到的数据作为示例
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

    //    {
    //    "measureName":"init_measurement",
    //    "freq": 1000,
    //    }
    // 记录采样频率
    CROW_ROUTE(app, "/freq").methods("POST"_method)
        ([](const crow::request& req) {
        // 解析传入的JSON数据
        try {
            crow::json::rvalue json_data = crow::json::load(req.body);
            // 打印接收到的数据（仅用于调试）
            std::cout << "Received JSON data: " << json_data << std::endl;

            // 数据库操作
            InfluxClient::getInstance()->writeMeasurefreq(json_data["measureName"].s(), json_data["freq"].i());

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


    // 登录，POST请求，接收前端传递的JSON数据
    CROW_ROUTE(app, "/login").methods("POST"_method)
        ([](const crow::request& req) {
        // 解析传入的JSON数据
        try {
            crow::json::rvalue json_data = crow::json::load(req.body);
            // 打印接收到的数据（仅用于调试）
            std::cout << "id & name " << json_data["username"] << json_data["password"] << std::endl;
            // 在这里可以添加对登录的处理逻辑
            // 检验用户名和密码是否正确, 函数verify_login检验用户名和密码是否正确
            bool is_valid = verify_login(json_data["username"].s(), json_data["password"].s());

            // 返回响应
            if (is_valid) { // 用户名和密码正确
                return crow::response(200, "Login successful");
            }
            else {
                return crow::response(401, "Invalid credentials");
            }
            // 前端根据响应判断是否登录成功
        }
        catch (const std::exception& e) {
            // 如果解析失败，返回错误信息
            crow::json::wvalue error_response;
            error_response["status"] = "error";
            error_response["message"] = e.what();
            return crow::response(400, error_response);
        }
            });

    app.port(18080).multithreaded().run();
//    app.port(18080).run();
}
