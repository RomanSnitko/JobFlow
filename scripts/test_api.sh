#!/bin/bash

HOST="http://localhost:8080"

echo "1. Submitting new task..."
SUBMIT_RES=$(curl -s -X POST "$HOST/v1/tasks/submit" \
     -H "Content-Type: application/json" \
     -d '{"type": "video_transcode", "delay": 0, "payload": {"file": "video.mp4", "resolution": "1080p"}}')

echo "Response: $SUBMIT_RES"

TASK_ID=$(echo $SUBMIT_RES | grep -oP '(?<="task_id":")[^"]+')

if [ -z "$TASK_ID" ]; then
    echo "Failed to get task_id"
    exit 1
fi

echo "2. Checking status for task: $TASK_ID"
for i in {1..5}; do
    STATUS_RES=$(curl -s "$HOST/v1/tasks/status?id=$TASK_ID")
    echo "Current status: $STATUS_RES"
    sleep 1
done