import subprocess
import numpy as np
from tqdm import tqdm
import matplotlib.pyplot as plt

request = int(input())
offset = 0
start = 0
waiting_time_estimation = []
for i in tqdm(range(1, request)):
    subprocess.run(f"./prover_client {i}", shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    subprocess.run("sleep 3",shell=True)

# data = []
# with open("measure/waiting_time.txt","r") as f:
    
#     for line in f.readlines():
#         data.append(eval(line))
# print(len(data))
    
# for request in range(1,300):
#     request_at_a_time = []
#     count = 0
#     while count < request:
#         request_at_a_time.append(data[offset])
#         offset += 1
#         count += 1
  
        
#     count = 0
#     waiting_time_estimation.append(round(sum(request_at_a_time)/request))
# print(waiting_time_estimation)


