#pragma once

#include <userver/components/component_base.hpp>
#include <userver/engine/task/task.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/redis/client.hpp>

namespace job_flow::components 
{

class TaskWorker final : public userver::components::ComponentBase 
{
public:
    static constexpr std::string_view kName = "task-worker";

    TaskWorker(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& context);

private:
    void ProcessingLoop();

    //imitation real task
    void ExecuteTask(const std::string& task_id);

    userver::storages::postgres::ClusterPtr pg_cluster_;
    userver::storages::redis::ClientPtr redis_client_;
    
    //the object of process (coroutines)
    userver::engine::Task worker_task_;
};

} // namespace job_flow::components