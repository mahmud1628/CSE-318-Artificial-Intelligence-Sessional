import csv
import matplotlib.pyplot as plt

names = []
randomized = []
greedy = []
semi_greedy = []
local_search = []
grasp = []

with open('results.csv', 'r') as file:
    reader = csv.reader(file)
    header1 = next(reader)  # Skip first header row
    header2 = next(reader)  # Skip second header row

    for i, row in enumerate(reader):
        names.append(row[0])
        randomized.append(int(row[3]))
        greedy.append(int(row[4]))
        semi_greedy.append(int(row[5]))
        local_search.append(int(row[7]))
        grasp.append(int(row[9]))


datasets_per_plot = 9
total_datasets = len(names)

bar_width = 0.15

for plot_num in range(6):
    start_index = plot_num * datasets_per_plot
    end_index = start_index + datasets_per_plot
    
    x = range(start_index, end_index)
    
    plt.figure(figsize=(16, 6))

    plt.bar([i - 2*bar_width for i in x], randomized[start_index:end_index], width=bar_width, label='Randomized')
    plt.bar([i - bar_width for i in x], greedy[start_index:end_index], width=bar_width, label='Greedy')
    plt.bar(x, semi_greedy[start_index:end_index], width=bar_width, label='Semi-Greedy')
    plt.bar([i + bar_width for i in x], local_search[start_index:end_index], width=bar_width, label='Local Search')
    plt.bar([i + 2*bar_width for i in x], grasp[start_index:end_index], width=bar_width, label='GRASP')

    plt.xticks(x, names[start_index:end_index])  
    plt.xlabel('Problem Instances')
    plt.ylabel('Cut Weight')
    plt.title(f'Comparison of Heuristics on Datasets {start_index+1} to {end_index}')
    plt.legend()
    plt.grid(axis='y', linestyle='--', alpha=0.6)
    plt.tight_layout()

    plt.savefig(f'comparison_{start_index+1}_to_{end_index}.png')

    plt.clf()
