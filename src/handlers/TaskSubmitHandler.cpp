#include <job_flow/handlers/TaskSubmitHandler.hpp>

#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/uuid4.hpp>

#include <job_flow/models/Task.hpp>
#include <job_flow/storage/PostgresDAO.hpp>

namespace job_flow::handlers
{

TaskSubmitHandler::TaskSubmitHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context)
{
    // get cluster postgre from components
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster();
}

std::string TaskSubmitHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const 
{
    auto request_json = userver::formats::json::FromString(request.RequestBody());

    auto task_type = request_json["type"].As<std::string>();
    auto task_payload = request_json["payload"];
    auto delay_seconds = request_json["delay"].As<int>(0);

    models::Task task;
    task.id = userver::utils::generators::GenerateUuid();
    task.type = std::move(task_type);
    task.payload = std::move(task_payload);
    task.status = models::TaskStatus::kPending;
    
    auto now = std::chrono::system_clock::now();
    task.run_at = userver::storages::postgres::TimePointTz{now + std::chrono::seconds(delay_seconds)};

    storage::PostgresDAO dao(pg_cluster_);
    dao.InsertTask(task);

    // json for client
    userver::formats::json::ValueBuilder response_builder;
    response_builder["task_id"] = task.id;
    response_builder["status"] = models::ToString(task.status);

    return userver::formats::json::ToString(response_builder.ExtractValue());
}

} // namespace job_flow::handlers