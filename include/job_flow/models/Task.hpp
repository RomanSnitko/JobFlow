#pragma once
#include <string>
#include <userver/formats/json/value.hpp>

namespace job_flow::models {
struct Task {
    std::string id;
    std::string type;
    std::string status;
    userver::formats::json::Value payload;
};
}