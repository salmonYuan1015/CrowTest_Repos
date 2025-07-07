#include "include/Point.h"
#include "LineProtocol.h"
#include <chrono>
#include <memory>
#include <sstream>
#include <iomanip>

namespace influxdb
{
    namespace
    {
        template <class... Ts>
        struct overloaded : Ts...
        {
            using Ts::operator()...;
        };
        template <class... Ts>
        overloaded(Ts...) -> overloaded<Ts...>;
    }

    Point::Point(const std::string& measurement)
        : mMeasurement(measurement), mTimestamp(std::chrono::system_clock::now()), mTags({}), mFields({})
    {
    }

    Point&& Point::addField(std::string_view name, const Point::FieldValue& value)
    {
        if (name.empty())
        {
            return std::move(*this);
        }

        mFields.emplace_back(std::make_pair(name, value));
        return std::move(*this);
    }

    Point&& Point::addTag(std::string_view key, std::string_view value)
    {
        if (key.empty() || value.empty())
        {
            return std::move(*this);
        }

        mTags.emplace_back(std::make_pair(key, value));
        return std::move(*this);
    }

    Point&& Point::setTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp)
    {
        mTimestamp = timestamp;
        return std::move(*this);
    }

    auto Point::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now())
    {
        return std::chrono::system_clock::now();
    }

    std::string Point::toLineProtocol() const
    {
        LineProtocol formatter;
        return formatter.format(*this);
    }

    std::string Point::getName() const
    {
        return mMeasurement;
    }

    std::chrono::time_point<std::chrono::system_clock> Point::getTimestamp() const
    {
        return mTimestamp;
    }

    std::string Point::getFields() const
    {
        std::stringstream convert;
        convert << std::setprecision(floatsPrecision) << std::fixed;
        bool addComma{false};
        for (const auto& field : mFields)
        {
            if (addComma)
            {
                convert << ',';
            }

            convert << field.first << "=";
            std::visit(overloaded{
                           [&convert](int v)
                           { convert << v << 'i'; },
                           [&convert](long long int v)
                           { convert << v << 'i'; },
                           [&convert](double v)
                           { convert << v; },
                           [&convert](const std::string& v)
                           { convert << '"' << v << '"'; },
                           [&convert](bool v)
                           { convert << (v ? "true" : "false"); },
                           [&convert](unsigned int v)
                           { convert << v << 'u'; },
                           [&convert](unsigned long long int v)
                           { convert << v << 'u'; },
                       },
                       field.second);
            addComma = true;
        }

        return convert.str();
    }

    const Point::FieldSet& Point::getFieldSet() const
    {
        return mFields;
    }

    std::string Point::getTags() const
    {
        if (mTags.empty())
        {
            return "";
        }

        std::string tags;
        for (const auto& tag : mTags)
        {
            tags += ",";
            tags += tag.first;
            tags += "=";
            tags += tag.second;
        }

        return tags.substr(1, tags.size());
    }

    const Point::TagSet& Point::getTagSet() const
    {
        return mTags;
    }

} // namespace influxdb
