//#include "crow.h"
//#include "curl/curl.h"
//#include "QDebug"
//#include <influxdb/include/InfluxDBFactory.h>
//#include <influxdb/include/Point.h>
//#include <cstdlib>  // 用于 system()
//#include <random>
//#include <QJsonDocument>
//#include <QJsonObject>
//#include <QJsonArray>
//#include "Interface/influxclient.h"
//#include <QDateTime>

//using namespace influxdb;

//// json数据转换
//QVector<QVector<std::string>> jsonTransform(QString jsonString){
//    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
//    QJsonObject jsonObj = doc.object();
//    // 提取baseStrs_vec
//    QJsonArray baseStrsArray = jsonObj["baseStrs_vec"].toArray();
//    QVector<QVector<std::string>> baseStrs_vec;
//    for (const QJsonValue &rowValue : baseStrsArray) {
//        QVector<std::string> row;
//        QJsonArray rowArray = rowValue.toArray();
//        for (const QJsonValue &strValue : rowArray) {
//            // 将QString转换为std::string
//            row.push_back(strValue.toString().toStdString());
//        }
//        baseStrs_vec.append(row);
//    }
//    return baseStrs_vec;
//}

//int main() {
//    crow::SimpleApp app;

//    CROW_ROUTE(app, "/aaa")
//        ([]() {

//// --------------------------------------------------------------------------------------------------------------
//// ----------------------------------------数据库连接与database创建操作--------------------------------------------
//// --------------------------------------------------------------------------------------------------------------
//            // 创建InfluxDB连接实例
//            std::unique_ptr<InfluxDB> db = InfluxDBFactory::Get("http://localhost:8086?db=NOAA_water_database");

//            // 判定是否有NOAA_water_database数据库，若没有则从本地.txt文件导入
//            std::string command = "SHOW DATABASES";
//            auto result_show = db->query(command);

//            bool exists = false;
//            for (auto& row : result_show) {
//                qDebug() << QString::fromStdString(row.getTags());  // 打印所有DATABASES的名称
//                if (row.getTags() == "name=NOAA_water_database") {
//                    exists = true;
//                    std::cout << "Database 'NOAA_water_database' exists! Dose not need importing" << std::endl;
//                    break;
//                }
//            }
//            if(!exists){
//                std::cout << "Database 'NOAA_water_database' does not exist. Need importing from NOAA_data.txt" << std::endl;

//                // 不存在Database 'NOAA_water_database'，需要从.txt文件导入

//                // ---------------------------------------------------------------------
//                // ****重要！下面influx.exe以及.txt的存储路径所在路径均不能有中文或有空格
//                std::string command = "F:/iGreatWorks/iGreatworks1.0/iGreatworks1.0/influxdb-1.8.3-1/influx -import -path=C:/Users/sunyu/Desktop/NOAA_data.txt -precision=s -database=NOAA_water_database";
//                // ---------------------------------------------------------------------

//                // 执行命令
//                int result_import = std::system(command.c_str());

//                // 检查命令是否成功执行
//                auto result_checking = db->query("SHOW DATABASES");
//                for (auto& row : result_checking) {
//                    qDebug() << QString::fromStdString(row.getTags());  // 打印所有DATABASES的名称
//                }
//            }


//// --------------------------------------------------------------------------------------------------------------
//// -----------------------------------------------数据库读取操作--------------------------------------------------
//// --------------------------------------------------------------------------------------------------------------

//            // ----从NOAA_water_database中的measurements h2o_pH 读取数据，以std::vector<Point> resPoint的形式返回
//            QVector<QVector<double>> groupOriginData;
//            QVector<double> OriginDataVec;

//            QString measureName = "h2o_pH";
//            QString command_query = "SELECT * from " + measureName + " LIMIT 10";
//            std::vector<Point> resPoint = db->query(command_query.toStdString());

//            // 打印resPoint中各个Point的数值
//            if(!resPoint.empty()){
//                std::cout << "=============First 10 Points in h2o_pH==============" << std::endl;
//                std::cout << "=====================================================" << std::endl;
//                for(int i = 0; i < resPoint.size(); i++)
//                    {
//                        std::cout << "=============Now is " << i + 1 << "/10 Point in h2o_pH==============" << std::endl;
//                        std::cout << "Name: " <<resPoint[i].getName()<<std::endl;
//                        std::cout << "Fields: " <<resPoint[i].getFields()<<std::endl;
//                        std::cout << "Tags: " <<resPoint[i].getTags()<<std::endl;
//                    }

//                qDebug() << "command_query success!";
//            }else{
//                qDebug() << "resPoint is NULL!";
//            }


//// --------------------------------------------------------------------------------------------------------------
//// -----------------------------------------------数据库写入操作--------------------------------------------------
//// --------------------------------------------------------------------------------------------------------------
//            // 首先在databse NOAA_water_database下新建一个measurements Measure_test作为测试
//            std::string command_query_measurement = "SHOW MEASUREMENTS";
//            // 执行查询measurements命令
//            auto result_measurements = db->query(command_query_measurement);
//            bool exists_measurement = false;
//            for (auto& row : result_measurements) {
//                qDebug() << QString::fromStdString(row.getTags());  // 打印所有MEASUREMENTS的名称
//                if (row.getTags() == "name=Measure_test") {
//                    exists_measurement = true;
//                    std::cout << "MEASUREMENT 'Measure_test' exists! Dose not need Setting Up!" << std::endl;
//                    break;
//                }
//            }
//            if(!exists){
//                std::cout << "MEASUREMENT 'Measure_test' does not exist. Need need Setting Up!" << std::endl;

//                // MEASUREMENT 'Measure_test'，需要通过命令新建
//                std::string newMeasurement = "Measure_test";
//                std::string command_initNewMeasurement = "INSERT " + newMeasurement + ",location=office value=22.5";
//            }


//            // 创建一个Point对象，并指定Measurement为Measure_test
//            Point point("Measure_test");
//            // 随机数生成器
//            std::random_device rd;
//            std::mt19937 gen(rd());

//            // 随机生成 "value" 字段
//            std::uniform_real_distribution<> value_dist(10.0, 50.0);  // 随机生成10到50之间的浮动数值
//            double random_value = value_dist(gen);

//            // 随机选择 location 的值
//            std::uniform_int_distribution<> location_dist(0, 1);  // 0和1之间选择，模拟选择两个位置
//            std::string location = (location_dist(gen) == 0) ? "office" : "lab";  // 随机选择 "office" 或 "lab"

//            // 添加字段和值
//            point.addField("value", random_value);

//            // 添加标签，选择随机位置
//            point.addTag("location", location);

//            // 输出调试信息
//            std::cout << "Location: " << location << ", Value: " << random_value << std::endl;
//            // 将数据点写入到InfluxDB
//            db->write(std::move(point));
//            std::cout << "Data inserted successfully!" << std::endl;

//            QString command_checkWriteStatus = "SELECT * from Measure_test";
//            std::vector<Point> Measure_test_Points = db->query(command_checkWriteStatus.toStdString());
//            if(!Measure_test_Points.empty()){
//                for(int i = 0; i < Measure_test_Points.size(); i++)
//                    {
//                        std::cout << "=============Now is " << i + 1 << "th Point in Measure_test==============" << std::endl;
//                        std::cout << "Name: " <<Measure_test_Points[i].getName()<<std::endl;
//                        std::cout << "Fields: " <<Measure_test_Points[i].getFields()<<std::endl;
//                        std::cout << "Tags: " <<Measure_test_Points[i].getTags()<<std::endl;
//                    }

//                qDebug() << "command_checkWriteStatus success!";
//            }else{
//                qDebug() << "Measure_test_Points is NULL!";
//            }

//            return "Hello, World!";
//        });


//    // 设置POST路由来接收前端传递的JSON数据
//    CROW_ROUTE(app, "/submit-data").methods("POST"_method)
//        ([](const crow::request& req) {
//        // 解析传入的JSON数据
//        try {
//            crow::json::rvalue json_data = crow::json::load(req.body);

//            // 将json_data转换为QString
//            // 注意：json_data本身是一个复杂的对象，不能直接转换为QString，
//            // 我们需要将其序列化为字符串形式。
//            std::ostringstream oss;
//            oss << json_data; // 使用流操作符将json_data输出为字符串
//            QString jsonString = QString::fromStdString(oss.str());
//            // 打印接收到的数据（仅用于调试）
//            qDebug() << "Received JSON data as QString:" << jsonString;
//            // 解析JSON字符串为QJsonDocument
//            QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
//            QJsonObject jsonObj = doc.object();
//            // 提取tableName
//            QString tableName = jsonObj["tableName"].toString();
//            // 提取id_vec
//            QJsonArray idArray = jsonObj["id_vec"].toArray();
//            QVector<int> id_vec;
//            for (const QJsonValue &value : idArray) {
//                id_vec.append(value.toInt());
//            }
//            // 提取baseStrs_vec
//            QJsonArray baseStrsArray = jsonObj["baseStrs_vec"].toArray();
//            QVector<QVector<std::string>> baseStrs_vec;
//            for (const QJsonValue &rowValue : baseStrsArray) {
//                QVector<std::string> row;
//                QJsonArray rowArray = rowValue.toArray();
//                for (const QJsonValue &strValue : rowArray) {
//                    // 将QString转换为std::string
//                    row.push_back(strValue.toString().toStdString());
//                }
//                baseStrs_vec.append(row);
//            }

//            // 批量写入数据
//            qDebug() << "Entering back-end writeOriginData";

//            QTime currentTime = QTime::currentTime();
//            qDebug() << QString::fromLocal8Bit("当前时间（精确到毫秒）:") << currentTime.toString("HH:mm:ss:zzz");

////            InfluxClient::getInstance()->writeOriginData(tableName, id_vec, baseStrs_vec);

//            // 现在你已经有了原始的tableName, id_vec, 和 baseStrs_vec
//            qDebug() << "TableName:" << tableName;
//            qDebug() << "ID Vector:" << id_vec;
//            qDebug() << "Base Strings Vector:";
//            for (const auto& row : baseStrs_vec) {
//                for (const std::string& str : row) {
//                    qDebug() << QString::fromStdString(str);
//                }
//            }


//            // 构造要返回的JSON响应
//            crow::json::wvalue response_json;
//            response_json["status"] = "success";
//            // response_json["received_data"] = json_data;  // 返回接收到的数据作为示例

//            // 返回JSON响应
//            return crow::response(200, response_json);


//        }
//        catch (const std::exception& e) {
//            // 如果解析失败，返回错误信息
//            crow::json::wvalue error_response;
//            error_response["status"] = "error";
//            error_response["message"] = e.what();
//            return crow::response(400, error_response);
//        }
//            });

//    app.port(18080).multithreaded().run();
//}
