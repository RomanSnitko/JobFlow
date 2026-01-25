#include <job_flow/handlers/TaskStatusHandler.hpp>

namespace job_flow::handlers {

TaskStatusHandler::TaskStatusHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string TaskStatusHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& /*request*/,
    userver::server::request::RequestContext& /*context*/) const {
    
    return "JobFlow: Status endpoint is ready. Logic will be here.\n";
}

} // namespace job_flow::handlers