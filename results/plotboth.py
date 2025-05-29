# CODE TO GENERATE BARS (MEDIAN) OVERLAID WITH BOXPLOT LINES (NO BOX FILL)

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches # For potential custom legend
import numpy as np # For median/percentile calculations if needed (pandas has median)
import os

# --- Configuration ---
# CSV file paths
file1 = 'allruns.csv'         # Should contain: Fixed vqe_runs=6, varying num_threads
file2 = 'allruns_single.csv'  # Should contain: Fixed num_threads=1, varying vqe_runs

# --- IMPORTANT: Column names MUST match your CSV headers ---
# File 1 (allruns.csv)
runtime_col1 = 'runtime(s)'      # Runtime data column in file1
# varying_param_col1= 'vqe_runs' # Column in file1 representing the varying parameter (e.g., num_threads)
# param1_name= 'num_threads'  # Label for this parameter set
param1_name= 'vqe_runs' # Column in file1 representing the varying parameter (e.g., num_threads)
varying_param_col1= 'num_threads'  # Label for this parameter set

# File 2 (allruns_single.csv)
runtime_col2 = 'runtime(s)'      # Runtime data column in file2
varying_param_col2 = 'vqe_runs' # Column in file2 representing the varying parameter (e.g., vqe_runs)
param2_name = 'num_threads'     # Label for this parameter set

# Plotting aesthetics
color1 = 'skyblue'   # Bar color for data from file1
color2 = 'lightcoral'  # Bar color for data from file2
bar_width = 0.35       # Width for bars and the invisible boxes
line_color = 'black'   # Color for the boxplot lines (median, whiskers, caps)
median_linewidth = 1.5 # Thickness of the median line

# --- Helper Function to Load Data and Calculate Medians ---
# This function remains largely the same
def load_prepare_data_median(filename, runtime_col, varying_col, description):
    """Loads CSV, prepares data lists for boxplot, calculates medians."""
    print(f"Processing {description} file: {filename}...")
    plot_data = [] # List of lists for boxplot raw data
    positions = [] # X-axis positions
    medians = []   # List of medians for bar heights
    averages = []

    if not os.path.exists(filename):
        print(f"  Error: File '{filename}' not found.")
        return None, None, None, None

    try:
        df = pd.read_csv(filename)
        if not all(col in df.columns for col in [runtime_col, varying_col]):
            print(f"  Error: File '{filename}' missing required columns '{runtime_col}' or '{varying_col}'.")
            print(f"  Found columns: {list(df.columns)}")
            return None, None, None, None

        df[runtime_col] = pd.to_numeric(df[runtime_col], errors='coerce')/1e6
        df[varying_col] = pd.to_numeric(df[varying_col], errors='coerce')
        df.dropna(subset=[runtime_col, varying_col], inplace=True)
        df[varying_col] = df[varying_col].astype(int)

        if df.empty:
            print(f"  Warning: No valid numeric data found in '{filename}'.")
            return [], [], [], description

        grouped_data = df.groupby(varying_col)[runtime_col]
        positions = sorted(grouped_data.groups.keys())

        for pos in positions:
            data_group = grouped_data.get_group(pos)
            plot_data.append(data_group.tolist())
            medians.append(data_group.median())
            averages.append(data_group.mean())

        print(f"  Successfully prepared data for positions: {positions}")
        return plot_data, positions, averages, description

    except Exception as e:
        print(f"  An error occurred processing '{filename}': {e}")
        return None, None, None, None

# --- Load Data for File 1 Only --- # << CHANGED section title
# Data Set 1: Fixed VQE Runs=6 (from file1)
plot_data1, positions1, medians1, desc1 = load_prepare_data_median(file1, runtime_col1, varying_param_col1, f'Fixed VQE Runs=6 (vs {varying_param_col1})')
# --- REMOVED loading for file2 ---


# --- Verification and Error Handling --- # << CHANGED section title
# Check if data loading failed
if plot_data1 is None:
    print("Error: Could not load data from the file. Exiting.")
    exit()
# Check if *any* data is available to plot
if not plot_data1:
    print("Error: No data available to plot after processing file. Exiting.")
    exit()
# --- REMOVED error handling specific to file2 ---

# --- Create Plot ---
print("Generating plot...")
fig, ax = plt.subplots(figsize=(12, 7)) # << CHANGED (Adjusted figure size potentially)

# --- Plotting Parameters --- # << CHANGED section title
# No offsets needed when plotting only one set centered
# --- REMOVED offset1 and offset2 calculation ---

# --- Plot Set 1 (Bars + Invisible Box) --- # << CHANGED section title
if plot_data1:
    print(f"Plotting Set 1 ({desc1}) at positions: {positions1}")
    # Use positions1 directly for centering
    bar_positions1 = positions1 -.2 # << CHANGED (No offset needed)

    # 1a. Plot Bars (using medians, placed behind boxplot lines)
    ax.bar(bar_positions1, medians1, bar_width, label=desc1, color=color1, alpha=0.7, zorder=2)

    # 1b. Plot Boxplot (transparent box, visible lines on top)
    ax.boxplot(plot_data1,
                positions=bar_positions1, # Use SAME positions as bars for alignment
                widths=bar_width,
                patch_artist=True,
                showfliers=False,
                boxprops=dict(facecolor='none', edgecolor='none'), # Invisible box
                medianprops=dict(color=line_color, linewidth=median_linewidth),
                whiskerprops=dict(color=line_color, linewidth=1),
                capprops=dict(color=line_color, linewidth=1),
                flierprops=dict(marker='o', markeredgecolor=line_color, markerfacecolor='none', markersize=5),
                zorder=3)

# --- REMOVED plotting block for Set 2 ---

# --- Customize Plot --- # << CHANGED section title
ax.set_title(f'Mean Runtime (Fixed VQE Runs=6) vs {varying_param_col1}') # << CHANGED Title
ax.set_xlabel(f'{varying_param_col1}') # << CHANGED X-axis label
ax.set_ylabel('Runtime(s)')
ax.yaxis.grid(True, linestyle='-', which='major', color='lightgrey', alpha=0.5, zorder=1)

# --- Create Legend ---
# ax.legend(loc='best') # Legend will now only contain the entry for file1

# --- Adjust X-axis Ticks --- # << CHANGED section title
all_positions = positions1 # << CHANGED (Only need positions from file1)
if all_positions:
    ax.set_xticks(all_positions)
    ax.set_xticklabels([str(p) for p in all_positions])
    # Adjust limits based on the single set of positions
    ax.set_xlim(min(all_positions) - 0.75, max(all_positions) + 0.75) # << CHANGED
else:
    ax.set_xticks([])

plt.tight_layout()

plot_filename = f'vqe_final.svg'
print("Saving plot... to f{plot_filename}")
plt.savefig(plot_filename, format='svg')

