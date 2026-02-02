DROP TABLE IF EXISTS tasks;

CREATE TABLE tasks (
    id UUID PRIMARY KEY,
    type TEXT NOT NULL,
    payload JSONB NOT NULL,
    status TEXT NOT NULL,
    run_at TIMESTAMPTZ NOT NULL,
    retries INTEGER DEFAULT 0,
    max_retries INTEGER DEFAULT 3,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW(),
    error_message TEXT
);

CREATE INDEX idx_tasks_status_run_at ON tasks (status, run_at) WHERE status = 'pending';
CREATE INDEX idx_tasks_created_at ON tasks (created_at);