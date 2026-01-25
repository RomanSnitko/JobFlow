#include <job_flow/handlers/TaskSubmitHandler.hpp>

namespace job_flow::handlers {

TaskSubmitHandler::TaskSubmitHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string TaskSubmitHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& /*request*/,
    userver::server::request::RequestContext& /*context*/) const {
    
    return "JobFlow: Submit endpoint is ready. Logic will be here.\n";
}

} // namespace job_flow::handlers