#pragma once
#include <userver/storages/postgres/postgres_fwd.hpp>

namespace job_flow::storage {
class PostgresDAO {
public:
    PostgresDAO(userver::storages::postgres::ClusterPtr pg_cluster) 
        : pg_cluster_(std::move(pg_cluster)) {}
private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};
}