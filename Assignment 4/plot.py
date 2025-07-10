import pandas as pd
import matplotlib.pyplot as plt
import os

# Replace with your actual filename
filename = 'adult_igr.csv'
data = pd.read_csv(filename)

# Extract dataset name and selection criterion
basename = os.path.basename(filename)
parts = basename.split('_')
dataset_name = parts[0].capitalize()
criterion_code = parts[1].split('.')[0].lower()

# Map short codes to full criterion names
criterion_map = {
    'ig': 'Information Gain',
    'igr': 'Information Gain Ratio',
    'nwig': 'Normalized Weighted Information Gain'
}
criterion_name = criterion_map.get(criterion_code, criterion_code.upper())

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(data['Max Depth'], data['Training Accuracy'], label='Training Accuracy', marker='o')
plt.plot(data['Max Depth'], data['Test Accuracy'], label='Test Accuracy', marker='o')

plt.xlabel('Max Depth')
plt.ylabel('Accuracy')
plt.title(f'Accuracy vs Max Depth\nDataset: {dataset_name} | Selection Criterion: {criterion_name}')
plt.legend()
plt.grid(True)
plt.tight_layout()

# Save plot as image (same name as CSV but .png)
output_filename = os.path.splitext(filename)[0] + '.png'
plt.savefig(output_filename, dpi=300)
plt.close()  # Close the figure to free memory
