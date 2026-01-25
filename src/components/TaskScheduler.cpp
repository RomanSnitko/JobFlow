#include <job_flow/components/TaskScheduler.hpp>

namespace job_flow::components {

TaskScheduler::TaskScheduler(const userver::components::ComponentConfig& config,
                             const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context) {
    // Здесь позже добавим PeriodicTask
}

} // namespace job_flow::components