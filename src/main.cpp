#include <userver/components/minimal_server_component_list.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>

#include <job_flow/handlers/hello.hpp>
#include <job_flow/handlers/TaskSubmitHandler.hpp>
#include <job_flow/handlers/TaskStatusHandler.hpp>
#include <job_flow/components/TaskScheduler.hpp>
#include <job_flow/components/TaskWorker.hpp>

int main(int argc, char* argv[]) {
    auto component_list = userver::components::MinimalServerComponentList()
        .Append<userver::server::handlers::Ping>()
        .Append<userver::components::TestsuiteSupport>("testsuite-support")
        .Append<userver::components::HttpClient>("http-client")
        .Append<userver::clients::dns::Component>("dns-client")
        .Append<userver::server::handlers::TestsControl>()
        .Append<userver::components::Secdist>()
        .Append<userver::components::DefaultSecdistProvider>("default-secdist-provider")
        .Append<userver::components::Postgres>("postgres-db-1")
        .Append<userver::components::Redis>("redis-tasks")
        .Append<job_flow::components::TaskScheduler>("task-scheduler")
        .Append<job_flow::components::TaskWorker>("task-worker")
        .Append<job_flow::handlers::TaskSubmitHandler>("handler-task-submit")
        .Append<job_flow::handlers::TaskStatusHandler>("handler-task-status");

    job_flow::AppendHello(component_list);

    return userver::utils::DaemonMain(argc, argv, component_list);
}