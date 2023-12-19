#!/bin/bash

# Stop servers based on saved PIDs
kill $(cat node.pid)
kill $(cat node2.pid)
kill $(cat node3.pid)
kill $(cat node4.pid)

# Clean up PID files
rm node.pid node2.pid node3.pid node4.pid