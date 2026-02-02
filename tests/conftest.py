import pytest
import pathlib

from testsuite.databases.pgsql import discover

pytest_plugins = [
    'pytest_userver.plugins.core',
    'pytest_userver.plugins.postgresql',
    'pytest_userver.plugins.redis',
]

@pytest.fixture(scope='session')
def service_source_dir():
    return pathlib.Path(__file__).parent.parent

@pytest.fixture(scope='session')
def pgsql_local_create(pgsql_local_settings):
    return discover.find_schemas(
        'job_flow',
        [pgsql_local_settings.path],
    )