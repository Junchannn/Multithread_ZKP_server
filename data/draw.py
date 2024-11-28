import matplotlib.pyplot as plt
import pandas as pd

# Path to your data file
data_file = "client_output.txt"  # Replace this with your actual file path

try:
    # Load data from the text file into a DataFrame
    data = pd.read_csv(data_file, sep=" ", header=None, names=["x", "y"])
except FileNotFoundError:
    print(f"Error: File '{data_file}' not found.")
    exit()
except pd.errors.EmptyDataError:
    print("Error: File is empty.")
    exit()
except Exception as e:
    print(f"Error reading file: {e}")
    exit()

# Ensure data is numeric, coerce invalid values to NaN
data["x"] = pd.to_numeric(data["x"], errors="coerce")
data["y"] = pd.to_numeric(data["y"], errors="coerce")

# Drop any rows with NaN values (caused by non-numeric data)
data = data.dropna()

# Ensure there is still data after dropping NaN values
if data.empty:
    print("Error: No valid data left after cleaning.")
    exit()

# Group by 'x' and calculate the median for each group
median_points = data.groupby("x")["y"].median().reset_index()

# Plot the data
plt.figure(figsize=(10, 6))

# Scatter plot of the original points
plt.scatter(data["x"], data["y"], alpha=0.6, label="Waiting time (millisecond)", color="blue")

# Plot the median line
plt.plot(
    median_points["x"].to_numpy(),
    median_points["y"].to_numpy(),
    color="red",
    marker="o",
    label="Median waiting time",
)

# Add labels, title, and legend
plt.xlabel("Requests")
plt.ylabel("Waiting time in thread queue")
plt.title("Waiting time of requests to ZKP server")
plt.legend()
plt.grid(True)

# Save the plot as a vector image (SVG or PDF)
plt.savefig("waiting_time_plot.pdf")  # Saves as PDF (you can change this to 'svg', 'png', etc.)

# Show the plot
plt.show()
