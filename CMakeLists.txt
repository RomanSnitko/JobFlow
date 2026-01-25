cmake_minimum_required(VERSION 3.12...3.31)
project(job_flow CXX)


set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)
set(USERVER_FEATURE_REDIS ON CACHE BOOL "" FORCE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)

# Сначала пробуем найти userver тихо (QUIET)
find_package(userver COMPONENTS core postgresql redis QUIET)

# Если не нашли в системе — скачиваем исходники
if(NOT userver_FOUND)
  download_userver(TRY_DIR third_party/userver VERSION 2.8)
endif()

userver_setup_environment()

# Указываем путь к нашим заголовочным файлам
include_directories(include)

# --- Секция 1: Основные исходные файлы (Business Logic) ---
add_library(${PROJECT_NAME}_objs OBJECT
    # Models
    # src/models/Task.cpp (добавим, если будет логика)
    
    # Storage Layer (DAO)
    src/storage/PostgresDAO.cpp
    src/storage/RedisQueue.cpp
    
    # Background Components
    src/components/TaskScheduler.cpp
    src/components/TaskWorker.cpp
    
    # API Handlers
    src/handlers/TaskSubmitHandler.cpp
    src/handlers/TaskStatusHandler.cpp
    src/handlers/hello.cpp
)

# Связываем наш объектный файл с библиотеками userver
target_link_libraries(${PROJECT_NAME}_objs PUBLIC 
    userver::core 
    userver::postgresql 
    userver::redis
)

# --- Секция 2: Исполняемый файл (The Service) ---
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

# --- Секция 3: Инфраструктура тестов (Закомментировано до востребования) ---
# add_executable(${PROJECT_NAME}_unittest src/hello_test.cpp)
# target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)

# --- Секция 4: Установка (Production-ready install) ---
include(GNUInstallDirs)
if(DEFINED ENV{PREFIX})
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX "${PREFIX_PATH}")
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME})