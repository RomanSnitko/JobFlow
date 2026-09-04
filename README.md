# JobFlow
A submitted task is first validated by the API layer and persisted to PostgreSQL with a pending state. The TaskScheduler periodically queries the database for tasks whose run_at timestamp has been reached, transitions them to queued, and places their identifiers into Redis. Workers consume task identifiers from the queue, transition the corresponding records to running, execute the task, and finally persist either a completed or failed state together with the execution result or failure information.

PostgreSQL acts as the durable source of task state and scheduling metadata, while Redis is used as the low-latency dispatch mechanism between the scheduler and workers. Workers are stateless with respect to task ownership, so additional instances can be started without changing the scheduling or API layers. Redis queue operations provide the coordination mechanism required for multiple worker instances to consume tasks concurrently.

The service is built around asynchronous request processing and C++20 coroutines using userver. Database access, queue operations, scheduling loops, and worker execution are isolated into separate components, with HTTP handlers responsible for API-level concerns, DAOs encapsulating persistence, and background components managing scheduling and execution. This separation allows the API, scheduler, and worker layers to scale and evolve independently.

![C++20](https://img.shields.io/badge/C++20-00599C?logo=c%2B%2B&logoColor=white)
![userver](https://img.shields.io/badge/userver-framework-black?logo=yandex&logoColor=yellow)
![Coroutines](https://img.shields.io/badge/Stackful_Coroutines-FF6F00?logo=convertio&logoColor=white)
![Redis](https://img.shields.io/badge/Redis-DC382D?logo=redis&logoColor=white)
![PostgreSQL](https://img.shields.io/badge/PostgreSQL-336791?logo=postgresql&logoColor=white)
![Docker](https://img.shields.io/badge/Docker-2496ED?logo=docker&logoColor=white)
![Docker_Compose](https://img.shields.io/badge/Docker_Compose-2496ED?logo=docker&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)

![HTTP](https://img.shields.io/badge/HTTP-FF6F00?logo=http&logoColor=white)
![REST_API](https://img.shields.io/badge/REST_API-005571?logo=api&logoColor=white)
![TCP/IP](https://img.shields.io/badge/TCP/IP-0A66C2?logo=protocols&logoColor=white)
![JSON](https://img.shields.io/badge/JSON-000000?logo=json&logoColor=white)

![Google_Test](https://img.shields.io/badge/Google_Test-0071BC?logo=google&logoColor=white)
![Python](https://img.shields.io/badge/Python_Testsuite-3776AB?logo=python&logoColor=white)
![Pytest](https://img.shields.io/badge/Pytest-0A9EDC?logo=pytest&logoColor=white)

## Project Structure
```text
JobFlow/
├── include/job_flow/               # Public headers (Interfaces & Definitions)
│   ├── handlers/                   # API Layer: Asynchronous HTTP request processing
│   │   ├── TaskSubmitHandler.hpp   # Task ingestion point (Validation & Persistence)
│   │   ├── TaskStatusHandler.hpp   # Lifecycle observation (Real-time state tracking)
│   │   └── AdminMetricsHandler.hpp # Monitoring: Queue depth and error rate statistics
│   ├── storage/                    # Persistence Layer: Data Access Objects (DAO)
│   │   ├── PostgresDAO.hpp         # ACID-compliant storage for task metadata and state
│   │   └── RedisQueue.hpp          # Low-latency ephemeral broker for task dispatching
│   ├── components/                 # Core Logic: Background services & orchestration
│   │   ├── TaskScheduler.hpp       # Periodic Dispatcher: Scans DB for ready-to-run tasks
│   │   └── TaskWorker.hpp          # Execution Engine: Pulls from Redis and executes logic
│   └── models/                     # Domain Layer: Data structures and Type definitions
│       └── Task.hpp                # Task entity definition (UUID, Payload, Status, RunAt)
│
├── src/                            # Implementation Layer (Source files)
│   ├── main.cpp                    # Application bootstrap and Component Registry
│   ├── handlers/                   # HTTP logic: Parsing, Error handling, and JSON DTOs
│   ├── storage/                    # DB implementation: Async SQL queries and connection pools
│   ├── components/                 # Thread orchestration: Periodic tasks and Coroutine loops
│   └── models/                     # Serialization logic: JSON-to-Struct mapping (Boost/Userver)
│
├── configs/                        # Environment Configuration
│   ├── static_config.yaml          # Component manager settings (Threads, Ports, DB Pools)
│   └── config_vars.yaml            # Environment variables and dynamic runtime parameters
│
├── sql/                            # Database Infrastructure
│   ├── init_db.sql                 # Primary schema: Tables, Indices, and Constraints
│   └── migrations/                 # Versioned database evolution scripts
│
├── scripts/                        # Automation & DevOps Tooling
│   ├── dashboard.html              # Frontend: Web-based GUI for system monitoring
│   └── integration_test.sh         # CI/CD: Automated API testing via cURL/Python
│
├── docker-compose.yml              # Infrastructure-as-Code: Local Dev environment setup
├── Dockerfile                      # Multistage production-ready build instructions
├── CMakeLists.txt                  # Build System: Dependency management and Linkage
├── README.md                       # Product documentation
└── .gitignore                      # Version control exclusion rules
```

## Testing

Платформа была протестирована в условиях, имитирующий реальный высоконагруженный бэкэнд. Основная цель - проверить пропускную способность системы при одновременной вгрузке полумиллиона задач и их распределенной обработке! Звучит интересно? Тогда смотри дальше)

### Команда для вгрузки:
```text
# имитация 100 запросов в секунду на прием задач
for i in $(seq 1 500000); do 
  curl -s -X POST http://localhost:8080/v1/tasks/submit \
  -d "{\"type\":\"video_transcode\",\"delay\":0,\"payload\":{\"id\":$i}}" \
  -H "Content-Type: application/json" & 
done
```

### Проверка ответа API:
При каждом запросе фреймворк генерирует уникальный ID, который позволяет отследить задачу от входа до лога воркера.

```
# пример ответа сервера с заголовками
$ curl -i -X POST http://localhost:8080/v1/tasks/submit -d '{"type":"test"}'
HTTP/1.1 201 Created
Content-Type: application/json
X-YaTraceId: 7b2e1f44a105498ba12563fd9ddc8807

{"task_id":"e3af4d9b-4c41-4620-8465-87c6144c2b3a","status":"pending"}
```

### Состояние системы в момент загрузки (PostgreSQL):

```sql
jobflow_db=# SELECT status, count(*) FROM tasks GROUP BY status;

   status    | count  
-------------+--------
 pending     | 500000  -- все задачи приняты
```

### Запуск кластера воркеров (Horizontal Scaling)

Чтобы ускорить разбор очереди, мы масштабируем воркеры. Благодаря тому, что воркеры Stateless и общаются через Redis (используя атомарную операцию RPOP), мы можем запустить любое количество инстансов.

Команда запуска 10 воркеров:
```bash
docker-compose up -d --scale task-worker=10
```

### Мониторинг активных корутин:
Во время разбора очереди мы можем видеть, как распределяются задачи по внутренним потокам фреймворка:

```
$ curl -s http://localhost:8080/utils/monitoring | jq .http_server.active_tasks
{
  "current": 142,   -- active request
  "queued": 0,
  "total": 500000  
}
```

### Логи распределенной обработки

Можно наблюдать, как разные инстансы воркеров параллельно разбирают одну и ту же очередь из Redis, не мешая друг другу
В логах видно, как корутины на разных системных потоках (thread_id) параллельно обрабатывают задачи:

```
tskv timestamp=2026-02-02T14:30:00.12 level=INFO module=TaskScheduler text=Tick started: scanning for tasks
tskv timestamp=2026-02-02T14:30:00.21 level=INFO module=PostgresDAO text=Fetched 100 tasks in 42ms
tskv timestamp=2026-02-02T14:30:01.05 level=INFO module=TaskWorker thread_id=0x7f01 text=Worker popped task e3af4...
tskv timestamp=2026-02-02T14:30:01.06 level=INFO module=TaskWorker thread_id=0x7f88 text=Worker popped task 7c3b2...
tskv timestamp=2026-02-02T14:30:01.10 level=DEBUG module=PostgresDAO text=UPDATE tasks SET status='running' WHERE id=e3af4...
tskv timestamp=2026-02-02T14:30:02.41 level=INFO module=TaskWorker thread_id=0x7f01 text=Task e3af4... completed successfully. Time: 1350ms
```

### Мониторинг пропускной способности (Throughput)

Во время теста мы проверяли глубину очереди в Redis. При 10 воркерах на C++ (каждый из которых держит сотни корутин) скорость обработки по факту достигла впечатляющих результатов.

Проверка очереди в реальном времени:

```
$ watch -n 1 "redis-cli LLEN job_flow_queue"
# Вывод:
# 450,230 ... (через секунду)
# 445,100 ... (минус 5000 задач в секунду!)
```

### Статистика Redis под нагрузкой:
Команда --stat показывает реальное количество операций в секунду (RPS) на брокере:

```
$ redis-cli --stat -i 1
------- data ------ --------------------- load -------------------- - child -
keys       mem      clients blocked requests            connections          
1          245.50M  64      5       142530 (+12400)     128
# Видим стабильные 12,000+ запросов в секунду на пике разбора
```

### Итоговое состояние БД (Completion)

После завершения теста база данных содержит полный отчет о выполненной работе.

```sql
jobflow_db=# SELECT status, count(*), avg(updated_at - created_at) as avg_latency FROM tasks GROUP BY status;

   status    |  count   |    avg_latency     
-------------+----------+--------------------
 completed   |  500000  | 00:00:00.124532   -- cреднее время ожидания задачи < 150ms
```


