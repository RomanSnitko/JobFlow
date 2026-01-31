#include <job_flow/storage/RedisQueue.hpp>

#include <chrono>
#include <userver/storages/redis/client.hpp>

namespace job_flow::storage 
{

void RedisQueue::Push(const std::string& task_id)
{
    redis_client_->Lpush(kQueueKey, task_id, {}).Get();
}

std::optional<std::string> RedisQueue::Pop()
{   
    auto reply = redis_client_->Rpop(kQueueKey, {}).Get();

    if (reply)
    {
        return reply;
    }

    return std::nullopt;
}

} // namespace job_flow::storage