import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read CSV file
df = pd.read_csv('node_depth.csv')

def plot_dataset(df, dataset_name):
    subset = df[df['File Name'] == dataset_name]

    strategies = subset['Selection Strategy'].values
    node_counts = subset['Node Count'].values
    depth_counts = subset['Depth Count'].values

    x = np.arange(len(strategies))
    width = 0.35

    fig, ax = plt.subplots(figsize=(8,5))

    bars1 = ax.bar(x - width/2, node_counts, width, label='Node Count', color='skyblue')
    bars2 = ax.bar(x + width/2, depth_counts, width, label='Depth Count', color='salmon')

    ax.set_ylabel('Count')
    ax.set_xticks(x)
    ax.set_xticklabels(strategies)
    ax.set_title(f'{dataset_name.capitalize()} Dataset: Tree Complexity Metrics')
    ax.legend()

    
    def add_labels(bars):
        for bar in bars:
            height = bar.get_height()
            ax.annotate(f'{int(height)}',
                        xy=(bar.get_x() + bar.get_width() / 2, height),
                        xytext=(0, 3),  # 3 points vertical offset
                        textcoords="offset points",
                        ha='center', va='bottom', fontsize=9)
    add_labels(bars1)
    add_labels(bars2)

    plt.tight_layout()
    plt.savefig(f'{dataset_name}_tree_complexity.png')
    plt.close()


plot_dataset(df, 'iris')
plot_dataset(df, 'adult')
