#pragma once
#include <userver/components/loggable_component_base.hpp>

namespace job_flow::components {
class TaskWorker final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "task-worker";

    TaskWorker(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& context);
};
}