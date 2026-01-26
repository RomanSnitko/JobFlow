# JobFlow
Высокопроизводительная платформа распределенной обработки задач (аналог Celery/Sidekiq). Включает асинхронный Task API, систему планирования и сеть воркеров. Построена на микросервисной архитектуре с использованием C++20, корутин (userver), PostgreSQL и Redis. Реализованы механизмы гарантированной доставки, политики повторных попыток (retries) и обработка отказов через Dead Letter Queue. Позволяет масштабировать выполнение тяжелых фоновых операций независимо от основного API.

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
