import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

DATA_FILE = os.path.join(SCRIPT_DIR, 'results.csv')

OUT_IMG_SPEEDUP = os.path.join(SCRIPT_DIR, 'graph_speedup.png')
OUT_IMG_TIME = os.path.join(SCRIPT_DIR, 'graph_time.png')

def plot_graphs():
    print(f"finding: {DATA_FILE}")


    if not os.path.exists(DATA_FILE):
        print(f"Error: no fil;e.")
        sys.exit(1)

    try:
        df = pd.read_csv(DATA_FILE)
    except Exception as e:
        print(f"Error reading CSV: {e}")
        sys.exit(1)

    df = df.sort_values(by=['N', 'Threads'])

    n_sizes = df['N'].unique()

    plt.figure(figsize=(10, 6))
    
    for n in n_sizes:
        subset = df[df['N'] == n]
        plt.plot(subset['Threads'], subset['Speedup'], marker='o', label=f'N = {n}')

    if not df.empty:
        max_threads = df['Threads'].max()
        plt.plot([1, max_threads], [1, max_threads], 'k--', alpha=0.5, label='Ideal Linear Speedup')

    plt.title('OpenMP Speedup vs Number of Threads')
    plt.xlabel('Number of Threads')
    plt.ylabel('Speedup (Serial Time / Parallel Time)')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    if not df.empty:
        plt.xticks(df['Threads'].unique())
    
    plt.savefig(OUT_IMG_SPEEDUP)
    print(f"create a file: {OUT_IMG_SPEEDUP}")
    plt.close()

    plt.figure(figsize=(10, 6))

    for n in n_sizes:
        subset = df[df['N'] == n]
        plt.plot(subset['Threads'], subset['Avg_Parallel_Time'], marker='s', label=f'N = {n}')

    plt.title('Execution Time vs Number of Threads')
    plt.xlabel('Number of Threads')
    plt.ylabel('Time (seconds)')
    plt.legend(title="Polynomial Degree")
    plt.grid(True, linestyle='--', alpha=0.7)
    if not df.empty:
        plt.xticks(df['Threads'].unique())
    plt.yscale('log') 

    plt.savefig(OUT_IMG_TIME)
    print(f"create file: {OUT_IMG_TIME}")
    plt.close()

if __name__ == "__main__":
    plot_graphs()