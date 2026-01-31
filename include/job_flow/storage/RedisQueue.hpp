#pragma once

#include <string>
#include <optional>
#include <userver/storages/redis/client.hpp>

namespace job_flow::storage 
{

class RedisQueue 
{
public:
    RedisQueue(userver::storages::redis::ClientPtr redis_client)
        : redis_client_(std::move(redis_client)) {}

    void Push(const std::string& task_id);
    std::optional<std::string> Pop();

private:
    userver::storages::redis::ClientPtr redis_client_;
    // key in redis, where the queue (list) will be
    static constexpr const char* kQueueKey = "job_flow_queue";
};

} // namespace job_flow::storage