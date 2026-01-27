#include <job_flow/handlers/TaskStatusHandler.hpp>

#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/formats/json/value_builder.hpp>

#include <job_flow/storage/PostgresDAO.hpp>
#include <job_flow/models/Task.hpp>

namespace job_flow::handlers 
{

TaskStatusHandler::TaskStatusHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context)
{
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster();
}

std::string TaskStatusHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& /*context*/) const 
{
    const auto& task_id = request.GetArg("id");
    if (task_id.empty()) 
    {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return "Missing 'id' argument\n";
    }

    storage::PostgresDAO dao(pg_cluster_);
    auto task_opt = dao.GetTask(task_id);

    if (!task_opt.has_value()) 
    {
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        return "Task not found\n";
    }

    // data in json 
    const auto& task = task_opt.value();
    userver::formats::json::ValueBuilder builder;
    builder["task_id"] = task.id;
    builder["type"] = task.type;
    builder["status"] = models::ToString(task.status);
    builder["retries"] = task.retries;
    builder["max_retries"] = task.max_retries;

    return userver::formats::json::ToString(builder.ExtractValue());
}

} // namespace job_flow::handlers