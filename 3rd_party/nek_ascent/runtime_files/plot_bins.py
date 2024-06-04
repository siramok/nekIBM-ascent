import yaml
import matplotlib.pyplot as plt

particles = 'particles1.dat'
with open(particles, 'r') as file:
    num_points = file.readline().split()[0]

# Load data from YAML file
file_path = 'bin_data.yaml'
with open(file_path, 'r') as file:
    data = yaml.safe_load(file)

timestep = list(data['2d_binning'].keys())[-1]

# Extracting the relevant binned data
binning_data = data['2d_binning'][timestep]['attrs']['value']['value']
bin_axes = data['2d_binning'][timestep]['attrs']['bin_axes']['value']['z']

# Extracting num_bins, x_min, and x_max
num_bins = bin_axes['num_bins']
x_min = bin_axes['min_val']
x_max = bin_axes['max_val']

# Creating x-axis values based on the bin count and range
x_values = [x_min + i * (x_max - x_min) / num_bins for i in range(num_bins)]

# Plotting the data as a line plot
plt.figure(figsize=(10, 6))
plt.plot(x_values, binning_data, label='Binned Values')
plt.xlabel('Z axis')
plt.ylabel('Values')
plt.title(f'Plot of avg. velocity magnitude along the Z axis for timestep {timestep}')
plt.legend()
plt.grid(True)

# Save the plot to disk
output_path = f'binned_velocity_magnitude_{num_points}_points_{timestep}_step.png'
plt.savefig(output_path)
