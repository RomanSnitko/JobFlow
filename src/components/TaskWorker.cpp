#include <job_flow/components/TaskWorker.hpp>
#include <job_flow/storage/PostgresDAO.hpp>
#include <job_flow/storage/RedisQueue.hpp>

#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/engine/sleep.hpp>

namespace job_flow::components 
{

TaskWorker::TaskWorker(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context)
    : ComponentBase(config, context)
    , pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster())
    , redis_client_(context.FindComponent<userver::components::Redis>("redis-tasks").GetClient("redis-tasks"))
{
    //inf cycle in coroutine
    worker_task_ = userver::engine::AsyncNoSpan([this] { ProcessingLoop(); });
}

void TaskWorker::ProcessingLoop()
{
    storage::RedisQueue queue(redis_client_);

    while (!userver::engine::current_task::ShouldCancel()) 
    {
        std::optional<std::string> task_id_opt = queue.Pop();

        if (task_id_opt) 
        {
            ExecuteTask(*task_id_opt);
        } 
        else 
        {
            //sleep 100ms
            userver::engine::SleepFor(std::chrono::milliseconds(100));
        }
    }
}

void TaskWorker::ExecuteTask(const std::string& task_id)
{
    storage::PostgresDAO dao(pg_cluster_);

    dao.UpdateStatus(task_id, models::TaskStatus::kRunning);

    //imitation task
    LOG_INFO() << "WORKER IS DOING TASK: " << task_id;
    
    //execution 700ms by 1 task.. on average)))
    userver::engine::SleepFor(std::chrono::milliseconds(700));

    dao.UpdateStatus(task_id, models::TaskStatus::kCompleted);
    LOG_INFO() << "SUCCESSFULLY FINISHED TASK: " << task_id;
}

} // namespace job_flow::components