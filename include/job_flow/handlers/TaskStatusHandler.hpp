#pragma once
#include <userver/server/handlers/http_handler_base.hpp>

namespace job_flow::handlers {
class TaskStatusHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-task-status";

    TaskStatusHandler(const userver::components::ComponentConfig& config,
                      const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest&,
        userver::server::request::RequestContext&) const override;
};
}