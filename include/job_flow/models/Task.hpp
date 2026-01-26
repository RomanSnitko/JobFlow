#pragma once
#include <string>
#include <userver/formats/json/value.hpp>

namespace job_flow::models 
{

enum class TaskStatus
{
    kPending,
    kQueued,
    kRunning,
    kCompleted,
    kFailed
};

struct Task 
{
    std::string id;
    std::string type;
    TaskStatus status;
    userver::formats::json::Value payload;
    std::chrono::system_clock::time_point run_at;
    int retries{};
    int max_retries{};
    std::optional<std::string> last_error;
};

// For postgre and json
constexpr std::string_view ToString(TaskStatus statusEnum) 
{
    switch (statusEnum) 
    {
        case TaskStatus::kPending:   return "pending";
        case TaskStatus::kQueued:    return "queued";
        case TaskStatus::kRunning:   return "running";
        case TaskStatus::kCompleted: return "completed";
        case TaskStatus::kFailed:    return "failed";
    }
    return "unknown";
}

// To read from postgre
inline TaskStatus ToEnum(std::string_view statusStr) 
{
    if (statusStr == "pending")   return TaskStatus::kPending;
    if (statusStr == "queued")    return TaskStatus::kQueued;
    if (statusStr == "running")   return TaskStatus::kRunning;
    if (statusStr == "completed") return TaskStatus::kCompleted;
    if (statusStr == "failed")    return TaskStatus::kFailed;
    
    return TaskStatus::kPending;
}

}