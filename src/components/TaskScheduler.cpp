#include <job_flow/components/TaskScheduler.hpp>
#include <job_flow/storage/PostgresDAO.hpp>
#include <job_flow/storage/RedisQueue.hpp>

#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/redis/component.hpp>

namespace job_flow::components
{

TaskScheduler::TaskScheduler(const userver::components::ComponentConfig& config,
                             const userver::components::ComponentContext& context)
    : ComponentBase(config, context)
    , pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster())
    , redis_client_(context.FindComponent<userver::components::Redis>("redis-tasks").GetClient("redis-tasks"))
    , task_(
          "scheduler-loop",
          userver::utils::PeriodicTask::Settings{std::chrono::seconds(1)},
          [this] { DoWork(); } 
      )
{

}

void TaskScheduler::DoWork()
{
    storage::PostgresDAO dao(pg_cluster_);
    storage::RedisQueue queue(redis_client_);

    std::vector<models::Task> tasks = dao.GetReadyTasks(100);

    for (const auto& task : tasks) 
    {
        queue.Push(task.id);
        dao.UpdateStatus(task.id, models::TaskStatus::kQueued);
    }
}

} // namespace job_flow::components