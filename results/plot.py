import pandas as pd
import matplotlib.pyplot as plt
import os # Used to check if file exists

# --- Configuration ---
csv_file = 'allruns_single.csv'  # Replace with the actual path if needed
num_column = 'vqe_runs'     # The column containing the number to group by
runtime_column = 'runtime(s)' # The column containing the runtime data

# --- Check if CSV file exists ---
if not os.path.exists(csv_file):
    print(f"Error: The file '{csv_file}' was not found.")
    print("Please make sure the CSV file is in the correct location.")
    exit() # Stop the script if the file is missing

# --- Load and Prepare Data ---
try:
    # Load the CSV file into a pandas DataFrame
    data = pd.read_csv(csv_file)

    # Check if the required columns exist
    if num_column not in data.columns or runtime_column not in data.columns:
        print(f"Error: CSV file must contain the columns '{num_column}' and '{runtime_column}'.")
        print(f"Found columns: {list(data.columns)}")
        exit()

    # Convert the runtime column to numeric type.
    # 'coerce' will turn any non-numeric values (like "ERROR") into NaN (Not a Number)
    data[runtime_column] = pd.to_numeric(data[runtime_column], errors='coerce')/1000000

    # Remove rows where runtime could not be converted to a number (NaN values)
    original_rows = len(data)
    data.dropna(subset=[runtime_column], inplace=True)
    removed_rows = original_rows - len(data)
    if removed_rows > 0:
        print(f"Info: Removed {removed_rows} rows where '{runtime_column}' was not a valid number.")

    # Check if any data remains after cleaning
    if data.empty:
        print("Error: No valid numeric runtime data found in the CSV file after cleaning.")
        exit()

    # --- Create Box Plot ---
    print(f"Generating box plot for '{runtime_column}' grouped by '{num_column}'...")

    # Create the figure and axes for the plot
    # Adjust figsize as needed
    fig, ax = plt.subplots(figsize=(12, 7))

    # Use pandas built-in boxplot function, grouping by the 'num' column
    # Pass the axis 'ax' to the plot function
    data.boxplot(column=runtime_column, by=num_column, ax=ax, grid=False)

    # --- Customize Plot ---
    ax.set_title(f'Distribution of {runtime_column.capitalize()} per {num_column.capitalize()}')
    ax.set_xlabel(f'{num_column.capitalize()}')
    ax.set_ylabel(f'{runtime_column.capitalize()}')
    fig.suptitle('') # Remove the default pandas title above the plot

    # Improve layout (especially if x-axis labels overlap)
    plt.tight_layout(rect=[0, 0.03, 1, 0.95]) # Adjust layout to prevent title/label overlap

    # --- Show Plot ---
    print("Saving plot...")
    plot_filename = f'vqe.png'
    plt.savefig(plot_filename)

except FileNotFoundError:
    # This is another catch, although the os.path.exists check should handle it first
    print(f"Error: Could not find the file '{csv_file}'.")
except pd.errors.EmptyDataError:
    print(f"Error: The file '{csv_file}' is empty.")
except Exception as e:
    # Catch any other potential errors during processing or plotting
    print(f"An unexpected error occurred: {e}")
