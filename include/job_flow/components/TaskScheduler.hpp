#pragma once

#include <userver/components/component_base.hpp>
#include <userver/utils/periodic_task.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/redis/client.hpp>

namespace job_flow::components 
{

class TaskScheduler final : public userver::components::ComponentBase 
{
public:
    static constexpr std::string_view kName = "task-scheduler";

    TaskScheduler(const userver::components::ComponentConfig& config,
                  const userver::components::ComponentContext& context);

private:
    void DoWork();

    userver::storages::postgres::ClusterPtr pg_cluster_;
    userver::storages::redis::ClientPtr redis_client_;
    
    userver::utils::PeriodicTask task_;
};

} // namespace job_flow::components