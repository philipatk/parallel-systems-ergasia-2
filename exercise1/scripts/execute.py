import subprocess
import csv
import os
import statistics

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

PARENT_DIR = os.path.dirname(SCRIPT_DIR)

PROGRAM_NAME = os.path.join(PARENT_DIR, "poly_mul")
SOURCE_FILE = os.path.join(PARENT_DIR, "poly_mul.c")

OUTPUT_FILE = os.path.join(SCRIPT_DIR, "results.csv")

REPETITIONS = 4

N_VALUES = [10000, 20000, 40000] 

THREAD_VALUES = [1, 2, 4, 8, 16]

def parse_output(output_str):
    serial_time = 0.0
    parallel_time = 0.0
    
    lines = output_str.split('\n')
    for line in lines:
        if "serial time:" in line:
            try:
                parts = line.split()
                serial_time = float(parts[2])
            except ValueError:
                pass
        if "parallel time:" in line:
            try:
                parts = line.split()
                parallel_time = float(parts[2])
            except ValueError:
                pass
            
    return serial_time, parallel_time

def run_experiments():
    
    if not os.path.exists(PROGRAM_NAME):
        print(f"Error: no program file {PROGRAM_NAME}.")
        return

    print(f"Start {OUTPUT_FILE}...")
    
    # Άνοιγμα αρχείου CSV για εγγραφή
    with open(OUTPUT_FILE, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["N", "Threads", "Avg_Serial_Time", "Avg_Parallel_Time", "Speedup"])

        for n in N_VALUES:
            for t in THREAD_VALUES:
                serial_times = []
                parallel_times = []
                
                print(f"Running: N={n}, Threads={t} ... ", end="", flush=True)
                
                for i in range(REPETITIONS):
                    # Καλούμε το πρόγραμμα με την απόλυτη διαδρομή
                    result = subprocess.run(
                        [PROGRAM_NAME, str(n), str(t)], 
                        capture_output=True, 
                        text=True
                    )
                    
                    if result.returncode != 0:
                        print(f"\nError execution: {result.stderr}")
                        continue
                        
                    s_time, p_time = parse_output(result.stdout)
                    serial_times.append(s_time)
                    parallel_times.append(p_time)

                if serial_times and parallel_times:
                    avg_serial = statistics.mean(serial_times)
                    avg_parallel = statistics.mean(parallel_times)
                    
                    speedup = 0
                    if avg_parallel > 0:
                        speedup = avg_serial / avg_parallel
                    
                    writer.writerow([n, t, f"{avg_serial:.6f}", f"{avg_parallel:.6f}", f"{speedup:.4f}"])
                    print(f"Done. (Speedup: {speedup:.2f}x)")
                else:
                    print(" Failed.")

    print("\Done!")

if __name__ == "__main__":
    run_experiments()