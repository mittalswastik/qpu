import pandas as pd
import matplotlib.pyplot as plt
import matplotlib

matplotlib.rcParams.update({'font.size': 24})

try:
    df = pd.read_csv("total.csv")
    # Check if DataFrame is empty or columns are missing
    if df.empty or not all(col in df.columns for col in ['num_threads', 'runtime(s)']):
        print("CSV data is missing required columns ('num_threads', 'runtime(s)') or is empty.")
    else:
        # Ensure 'num_threads' and 'runtime(s)' are numeric
        df['num_threads'] = pd.to_numeric(df['num_threads'], errors='coerce')
        df['runtime(s)'] = pd.to_numeric(df['runtime(s)'], errors='coerce')/1000000
        df.dropna(subset=['num_threads', 'runtime(s)'], inplace=True)

        # Proceed with plotting if data is valid
        if not df.empty:
            # --- Plotting Code ---
            # Increase figure size to accommodate larger fonts
            plt.figure(figsize=(12, 8))

            # Group data by 'num_threads' and create a list of runtimes for each group
            boxplot_data = [df[df['num_threads'] == x]['runtime(s)'].tolist() for x in sorted(df['num_threads'].unique())]
            thread_labels = sorted(df['num_threads'].unique())

            # Create the boxplot
            plt.boxplot(boxplot_data, labels=thread_labels, showfliers=False)

            # Set title and labels with specified fontsize
            plt.title('Runtime as a Function of Number of Threads', fontsize=24)
            plt.xlabel('Number of Threads', fontsize=24)
            plt.ylabel('Runtime (s)', fontsize=24)

            # Set tick label font sizes
            plt.xticks(fontsize=24)
            plt.yticks(fontsize=24)

            plt.grid(axis='y', linestyle='--') # Add grid lines for the y-axis
            plt.tight_layout() # Adjust layout to prevent labels overlapping
            plt.show()
            print("Saving plot...")
            plot_filename = f'vqe_final.png'
            plt.savefig(plot_filename)
            # --- End Plotting Code ---
        else:
             print("No valid numeric data to plot after cleaning.")

except pd.errors.EmptyDataError:
    print("The provided CSV data is empty.")
except Exception as e:
    print(f"An error occurred: {e}")

