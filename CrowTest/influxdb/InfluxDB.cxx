//#include "include/InfluxDB.h"
//#include "include/InfluxDBException.h"
//#include "LineProtocol.h"
////#include "BoostSupport.h"

#include "include/InfluxDB.h"

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <cstdlib>  // 用于 system()

#ifdef INFLUXDB_WITH_BOOST
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// 绝对路径
//#include "D:/MyProgramData/program_install/Cplus/boost/boost_1_86_0/boost/lexical_cast.hpp"
//#include "D:/MyProgramData/program_install/Cplus/boost/boost_1_86_0/boost/property_tree/ptree.hpp"
//#include "D:/MyProgramData/program_install/Cplus/boost/boost_1_86_0/boost/property_tree/json_parser.hpp"


#endif

// TIME_TYPE is used to get time stamp
#define TIME_TYPE "%Y-%m-%dT%H:%M:%S.%fZ"

namespace influxdb
{
InfluxDB::InfluxDB(std::unique_ptr<Transport> transport) :
  mTransport(std::move(transport))
{
  mBuffer = {};
  mBuffering = false;
  mBufferSize = 0;
  mGlobalTags = {};
}

void InfluxDB::batchOf(const std::size_t size)
{
  mBufferSize = size;
  mBuffering = true;
}

void InfluxDB::flushBuffer() {
  if (!mBuffering) {
    return;
  }
  std::string stringBuffer{};
  for (const auto &i : mBuffer) {
    stringBuffer+= i + "\n";
  }
  mBuffer.clear();
  transmit(std::move(stringBuffer));
}

void InfluxDB::addGlobalTag(std::string_view key, std::string_view value)
{
  if (!mGlobalTags.empty()) mGlobalTags += ",";
  mGlobalTags += key;
  mGlobalTags += "=";
  mGlobalTags += value;
}

InfluxDB::~InfluxDB()
{
  if (mBuffering) {
    flushBuffer();
  }
}

void InfluxDB::transmit(std::string&& point)
{
#ifdef DEBUG_OUTPUT
    std::cout << "measure string : "<<point << std::endl;
#endif
    mTransport->send(std::move(point));
}

void InfluxDB::write(Point&& metric)
{
  if (mBuffering) {
#ifdef DEBUG_OUTPUT
    std::cout << "measure string : "<<metric.toLineProtocol() << std::endl;
#endif
    mBuffer.emplace_back(metric.toLineProtocol());
    if (mBuffer.size() >= mBufferSize) {
      flushBuffer();
    }
  } else {
    transmit(metric.toLineProtocol());
  }
}


// write函数不支持多个Point，新编写具备缓冲（batching）功能的writeBatch函数，用于适配id_vec, baseStrs_vec
void InfluxDB::writeBatch(const std::vector<Point>& points)
{
    if (mBuffering)
    {
        // 如果缓冲模式激活，将所有点加入缓冲区
        for (const auto& point : points)
        {
            mBuffer.emplace_back(point.toLineProtocol());
            if (mBuffer.size() >= mBufferSize)
            {
                flushBuffer();  // 缓冲区满时自动刷新
            }
        }
    }
    else
    {
        // 非缓冲模式下直接写入所有点
        std::string lineProtocol;
        for (const auto& point : points)
        {
            lineProtocol += point.toLineProtocol() + "\n";
        }
        if (!lineProtocol.empty())
        {
            transmit(std::move(lineProtocol));
        }
    }
}


#ifdef INFLUXDB_WITH_BOOST
std::vector<Point> InfluxDB::query(const std::string&  query)
{
  auto response = mTransport->query(query);
  std::stringstream ss;
  ss << response;
  std::vector<Point> points;
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
#ifdef DEBUG_OUTPUT
  std::cout << "start query process !" <<std::endl;
#endif
  for (auto& result : pt.get_child("results")) {
    auto isResultEmpty = result.second.find("series");
    if (isResultEmpty == result.second.not_found()) return {};
    for (auto& series : result.second.get_child("series")) {
      auto columns = series.second.get_child("columns");

      for (auto& values : series.second.get_child("values")) {
        Point point{series.second.get<std::string>("name")};
        auto iColumns = columns.begin();
        auto iValues = values.second.begin();
        for (; iColumns != columns.end() && iValues != values.second.end(); iColumns++, iValues++) {
          auto value = iValues->second.get_value<std::string>();
          auto column = iColumns->second.get_value<std::string>();
          if (!column.compare("time")) {
            std::tm tm = {};
            std::stringstream ss;
            ss << value;
            ss >> std::get_time(&tm, TIME_TYPE);
            point.setTimestamp(std::chrono::system_clock::from_time_t(std::mktime(&tm)));
            continue;
          }
          // By observing whether it is numerical data, we can judge whether it is field or tag
          try { point.addField(column, boost::lexical_cast<double>(value)); }
          catch(...) { point.addTag(column, value); }
        }
        points.push_back(std::move(point));
      }
    }
  }
  return points;
}
#else
std::vector<Point> InfluxDB::query(const std::string& /*query*/)
{
  throw std::runtime_error("InfluxDB query() requires boost");
}
#endif

} // namespace influxdb
