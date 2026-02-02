import pytest

async def test_ping(service_client):
    response = await service_client.get('/ping')
    assert response.status == 200

async def test_full_cycle(service_client):
    submit_payload = {
        "type": "test_type",
        "delay": 0,
        "payload": {"key": "value"}
    }
    
    submit_resp = await service_client.post('/v1/tasks/submit', json=submit_payload)
    assert submit_resp.status == 201
    task_id = submit_resp.json()['task_id']

    status_resp = await service_client.get(f'/v1/tasks/status?id={task_id}')
    assert status_resp.status == 200
    assert status_resp.json()['task_id'] == task_id