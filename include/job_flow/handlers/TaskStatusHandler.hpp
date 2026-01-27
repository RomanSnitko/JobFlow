#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <job_flow/storage/PostgresDAO.hpp>

namespace job_flow::handlers 
{

class TaskStatusHandler final : public userver::server::handlers::HttpHandlerBase 
{
public:
    // unique name for static_config.yaml
    static constexpr std::string_view kName = "handler-task-status";

    TaskStatusHandler(const userver::components::ComponentConfig& config,
                      const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace job_flow::handlers