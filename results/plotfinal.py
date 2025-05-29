# CODE TO PLOT ACTUAL MEDIAN VS IDEAL SCALED MEDIAN RUNTIME AS GROUPED BARS

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np # For array manipulation and checking min value
import os

# --- Configuration ---
# CSV file path
csv_file = 'allruns.csv' # Assumes this file has varying num_threads data

# --- IMPORTANT: Column names MUST match your CSV headers ---
runtime_col = 'runtime(sec)'
threads_col = 'vqe_runs' # The column representing num_threads
threads_label = 'num_threads' # Label for the axis

# --- Output Filename Configuration ---
output_filename_svg = "runtime_vs_scaled.svg"
output_filename_pdf = "runtime_vs_scaled.pdf"
output_filename_png = "vqebargraph.png"

# Plotting aesthetics
color_actual = 'dodgerblue'
color_scaled = 'darkorange'
bar_width = 0.35

file1 = 'runs_mod.csv'         # Should contain: Fixed vqe_runs=6, varying num_threads
file2 = 'allruns_single.csv'  # Should contain: Fixed num_threads=1, varying vqe_runs # << RE-ADDED

# --- << RE-ADDED: Output Filename Configuration >> ---
output_filename_svg = "combined_runtime_distribution.svg"
output_filename_pdf = "combined_runtime_distribution.pdf"
# Plotting aesthetics
color1 = 'skyblue'   # Bar color for data from file1
color2 = 'lightcoral'  # Bar color for data from file2 # << RE-ADDED
bar_width = 0.35       # Width for bars and invisible boxes (adjusted back) # << ADJUSTED
line_color = 'black'   # Color for the boxplot lines
median_linewidth = 1.5

# --- Helper Function to Load Data and Calculate Medians (Unchanged from previous overlay version) ---
def load_prepare_data_median(filename, runtime_col, varying_col, description):
    """Loads CSV, prepares data lists for boxplot, calculates medians."""
    print(f"Processing {description} file: {filename}...")
    plot_data = []
    positions = []
    medians = []

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
            medians.append(data_group.mean())

        print(f"  Successfully prepared data for positions: {positions}")
        return plot_data, positions, medians, description

    except Exception as e:
        print(f"  An error occurred processing '{filename}': {e}")
        return None, None, None, None

# --- Load Data for Both Files --- # << RE-ADDED loading for file2
# Data Set 1: Fixed VQE Runs=6 (from file1)
plot_data1, positions1, medians1, desc1 = load_prepare_data_median(file1, runtime_col, threads_label, f'With Multi-QPU Offload')
print(medians1)

# Data Set 2: Fixed Num Threads=1 (from file2)
plot_data2, positions2, medians2, desc2 = load_prepare_data_median(file2, runtime_col, threads_col, f'Without Multi-QPU Offload')


# --- Verification and Error Handling --- # << RE-ADDED handling for file2
if plot_data1 is None and plot_data2 is None:
    print("Error: Could not load data from either file. Exiting.")
    exit()
if plot_data1 is None: plot_data1, positions1, medians1 = [], [], []
if plot_data2 is None: plot_data2, positions2, medians2 = [], [], []
if not plot_data1 and not plot_data2:
    print("Error: No data available to plot after processing files. Exiting.")
    exit()

# --- Create Plot ---
print("Generating plot...")
fig, ax = plt.subplots(figsize=(14, 8)) # Adjusted figure size

# --- Plotting Parameters --- # << RE-ADDED offsets
offset1 = -bar_width / 2
offset2 = bar_width / 2

# --- Plot Set 1 (Bars + Invisible Box) ---
if plot_data1:
    print(f"Plotting Set 1 ({desc1}) at positions: {positions1}")
    bar_positions1 = [p + offset1 for p in positions1] # Use offset
    # Plot Bars
    ax.bar(bar_positions1, medians1, bar_width, label=desc1, color=color1, alpha=0.7, zorder=2)
    # Plot Boxplot lines
    ax.boxplot(plot_data1,
                positions=bar_positions1, # Use offset positions
                widths=bar_width,
                patch_artist=True,
                showfliers=False, # Keep outliers visible
                boxprops=dict(facecolor='none', edgecolor='none'), # Invisible box
                medianprops=dict(color=line_color, linewidth=median_linewidth),
                whiskerprops=dict(color=line_color, linewidth=1),
                capprops=dict(color=line_color, linewidth=1),
                flierprops=dict(marker='o', markeredgecolor=line_color, markerfacecolor='none', markersize=5),
                zorder=3)

# --- Plot Set 2 (Bars + Invisible Box) --- # << RE-ADDED plotting block for file2
if plot_data2:
    print(f"Plotting Set 2 ({desc2}) at positions: {positions2}")
    bar_positions2 = [p + offset2 for p in positions2] # Use offset
    # Plot Bars
    ax.bar(bar_positions2, medians2, bar_width, label=desc2, color=color2, alpha=0.7, zorder=2)
    # Plot Boxplot lines
    ax.boxplot(plot_data2,
                positions=bar_positions2, # Use offset positions
                widths=bar_width,
                patch_artist=True,
                showfliers=False,
                boxprops=dict(facecolor='none', edgecolor='none'), # Invisible box
                medianprops=dict(color=line_color, linewidth=median_linewidth),
                whiskerprops=dict(color=line_color, linewidth=1),
                capprops=dict(color=line_color, linewidth=1),
                flierprops=dict(marker='o', markeredgecolor=line_color, markerfacecolor='none', markersize=5),
                zorder=3)


# --- Customize Plot --- # << ADJUSTED title and labels
ax.set_title('Mean Runtime Bar with Boxplot Distribution showing Multi-QPU Efficacy', fontsize=24)
ax.set_xlabel(f'Number of VQE Runs', fontsize=24) # Combined X-axis label
ax.set_ylabel("Runtime(sec)", fontsize=24)# Assume runtime meaning is same
ax.yaxis.grid(True, linestyle='-', which='major', color='lightgrey', alpha=0.5, zorder=1)
ax.tick_params(axis='y', labelsize=24)
# --- Create Legend ---
ax.legend(loc='best', fontsize=24)

# --- Adjust X-axis Ticks --- # << RE-ADDED logic for combined positions
all_positions = sorted(list(set(positions1 + positions2)))
if all_positions:
    ax.set_xticks(all_positions)
    ax.set_xticklabels([str(p) for p in all_positions], fontsize=24)
    ax.set_xlim(min(all_positions) - 0.75, max(all_positions) + 0.75)
else:
    ax.set_xticks([])

plt.tight_layout()

# --- Save Plot to Files --- # << ADJUSTED (Same save logic, different filenames maybe)
print("Saving plots...")
plt.savefig(output_filename_png, dpi=300, bbox_inches='tight')
# Save as SVG
try:
    plt.savefig(output_filename_svg, format='svg', bbox_inches='tight')
    print(f"Successfully saved plot as {output_filename_svg}")
except Exception as e:
    print(f"Error saving as SVG: {e}")

# Save as PDF
try:
    plt.savefig(output_filename_pdf, format='pdf', bbox_inches='tight')
    print(f"Successfully saved plot as {output_filename_pdf}")
except Exception as e:
    print(f"Error saving as PDF: {e}")

# --- Do not show the plot interactively ---
# plt.show() # Keep commented out to only save

