#!/bin/bash

g++ -std=c++20 -I/user/include/nlohmann -o verifier_server src/server/*.cpp src/ZKP/*.cpp src/thread_pool/*.cpp -lntl -pthread
g++ -std=c++20 -I/user/include/nlohmann -o prover_client src/client/*.cpp src/ZKP/*.cpp -lntl -pthread


