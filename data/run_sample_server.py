import subprocess
from tqdm import tqdm
import sys
# Get the request count from command-line argument
if len(sys.argv) > 1:
    request = int(sys.argv[1])
else:
    print("Please provide the request count as a command-line argument.")
    sys.exit(1)

offset = 0
start = 0
waiting_time_estimation = []
for i in tqdm(range(1, request)):
    subprocess.run(f"./prover_client {i}", shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    subprocess.run("sleep 3", shell=True)





