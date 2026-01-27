#pragma once

#include <vector>

#include <job_flow/models/Task.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace job_flow::storage 
{

class PostgresDAO 
{
public:
    PostgresDAO(userver::storages::postgres::ClusterPtr pg_cluster) 
        : pg_cluster_(std::move(pg_cluster)) 
    {}

    void InsertTask(const models::Task& task);
    void UpdateStatus(const std::string& task_id, models::TaskStatus status);

    // limit for unoverloading workers
    std::vector<models::Task> GetReadyTasks(int limit);

    void MarkAsFailed(const std::string& task_id, std::string_view error_message);

    std::optional<models::Task> GetTask(const std::string& task_id);

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace job_flow::storage