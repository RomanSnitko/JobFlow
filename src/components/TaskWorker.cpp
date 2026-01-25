#include <job_flow/components/TaskWorker.hpp>

namespace job_flow::components {

TaskWorker::TaskWorker(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context) {
    // Здесь позже добавим цикл обработки задач
}

} // namespace job_flow::components