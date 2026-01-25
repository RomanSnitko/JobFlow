#pragma once
#include <userver/storages/redis/client_fwd.hpp>

namespace job_flow::storage {
class RedisQueue {
public:
    RedisQueue(userver::storages::redis::ClientPtr redis_client) 
        : redis_client_(std::move(redis_client)) {}
private:
    userver::storages::redis::ClientPtr redis_client_;
};
}