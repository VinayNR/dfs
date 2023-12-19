#!/bin/bash

# Start servers in the background and save their PIDs
../nodeserver ../config/node.conf & echo $! > node.pid
../nodeserver ../config/node2.conf & echo $! > node2.pid
../nodeserver ../config/node3.conf & echo $! > node3.pid
../nodeserver ../config/node4.conf & echo $! > node4.pid

# Optionally, add a sleep to keep the script running
sleep infinity