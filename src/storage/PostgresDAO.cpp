#include <job_flow/storage/PostgresDAO.hpp>
#include "job_flow/models/Task.hpp"

namespace job_flow::storage 
{

void PostgresDAO::InsertTask(const models::Task& task)
{
    pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO tasks (*) "
        "VALUES ($1, $2, $3, $4, $5, $6, $7) "
        "ON CONFLICT (id) DO NOTHING",
        task.id,
        task.type,
        task.payload,
        models::ToString(task.status),
        task.run_at,
        task.retries,
        task.max_retries
    );
}

void PostgresDAO::UpdateStatus(const std::string& task_id, models::TaskStatus status)
{
    pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE tasks SET status = $2, updated_at = NOW() WHERE id = $1",
        task_id,
        models::ToString(status)
    );
}

// get the pack of task with status = pending and run_t <= NOW()
std::vector<models::Task> PostgresDAO::GetReadyTasks(int limit)
{
    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT id, type, payload, status, run_at, retries, max_retries "
        "FROM tasks "
        "WHERE status = 'pending' AND run_at <= NOW() "
        "ORDER BY run_at ASC "
        "LIMIT $1",
        limit
    );

    std::vector<models::Task> tasks;

    tasks.reserve(result.Size());

    //mapping
    for (const auto& row : result) 
    {
        models::Task task;

        task.id = row["id"].As<std::string>();
        task.type = row["type"].As<std::string>();
        task.status = models::ToEnum(row["status"].As<std::string>());
        task.payload = row["payload"].As<userver::formats::json::Value>();
        task.run_at = row["run_at"].As<userver::storages::postgres::TimePointTz>();
        task.retries = row["retries"].As<int>();
        task.max_retries = row["max_retries"].As<int>();

        tasks.push_back(std::move(task));
    }

    return tasks;
}

void PostgresDAO::MarkAsFailed(const std::string& task_id, std::string_view error_message)
{
    pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE tasks SET status = 'failed', error_message = $2, updated_at = NOW() WHERE id = $1",
        task_id,
        error_message
    );
}

}