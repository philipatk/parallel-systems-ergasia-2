import subprocess
import re
import matplotlib.pyplot as plt
import os
import sys

# --- Ρυθμίσεις για το Exercise 3 ---
EX_ID = "3"  # Αντιστοιχεί στο EX=3 του Makefile
EXECUTABLE_PATH = f"exercise{EX_ID}/build/ex{EX_ID}" # π.χ. exercise3/build/ex3
SIZES = [10_000_000, 100_000_000] # 10^7 και 10^8
THREADS = [1, 2, 4, 8, 16] 
OUTPUT_IMAGES = [f"exercise{EX_ID}/plot_10e7.png", f"exercise{EX_ID}/plot_10e8.png"]

def compile_code():
    """Μεταγλώττιση χρησιμοποιώντας το Makefile."""
    print(f"--> Compiling Exercise {EX_ID} using make...")
    
    # Καθαρισμός προηγούμενων builds
    subprocess.run(["make", "clean", f"EX={EX_ID}"], check=False, capture_output=True)
    
    # Εκτέλεση make EX=3
    cmd = ["make", f"EX={EX_ID}"]
    
    try:
        subprocess.check_call(cmd)
        print("--> Compilation successful!\n")
    except subprocess.CalledProcessError:
        print("Error: Compilation failed via Makefile.")
        sys.exit(1)

def run_benchmark(size, mode, thread_count=1):
    """
    Τρέχει το εκτελέσιμο και επιστρέφει τον χρόνο εκτέλεσης.
    Arguments: ./exercise3/build/ex3 <size> <mode> <threads>
    """
    if not os.path.exists(EXECUTABLE_PATH):
        print(f"Error: Executable {EXECUTABLE_PATH} not found.")
        return None

    cmd = [f"./{EXECUTABLE_PATH}", str(size), str(mode), str(thread_count)]
    
    try:
        # Εκτέλεση και αρπαγή του output
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        output = result.stdout
        
        # Αναζήτηση του χρόνου στο output (π.χ. "Time: 0.123456")
        match = re.search(r"Time:\s*([0-9\.]+)", output)
        if match:
            return float(match.group(1))
        else:
            print(f"Warning: Could not parse time from output.")
            # Εκτύπωση του output για debugging αν δεν βρεθεί χρόνος
            # print(output) 
            return None
    except subprocess.CalledProcessError as e:
        print(f"Error running benchmark: {e}")
        return None

def plot_results(size, serial_time, parallel_times, filename):
    """Δημιουργία και αποθήκευση γραφήματος."""
    plt.figure(figsize=(10, 6))
    
    plt.plot(THREADS, parallel_times, marker='o', label='Parallel (OpenMP)', color='b', linewidth=2)
    plt.axhline(y=serial_time, color='r', linestyle='--', label=f'Serial ({serial_time:.4f}s)')
    
    plt.title(f'Mergesort Execution Time (Array Size: $10^{int(len(str(size))-1)}$)')
    plt.xlabel('Number of Threads')
    plt.ylabel('Time (seconds)')
    plt.xticks(THREADS)
    plt.grid(True, which="both", ls="-", alpha=0.5)
    plt.legend()
    
    plt.savefig(filename)
    print(f"--> Graph saved as {filename}")
    plt.close()

def main():
    # 1. Compile (make EX=3)
    compile_code()
    
    # 2. Loop για κάθε μέγεθος πίνακα
    for i, size in enumerate(SIZES):
        print(f"=== Benchmarking for Array Size: {size} (10^{int(len(str(size))-1)}) ===")
        
        # Α. Μέτρηση Σειριακού (Mode 0)
        print("   Running Serial...", end=" ", flush=True)
        serial_time = run_benchmark(size, 0, 1) # Threads=1 (τυπικό για serial)
        
        if serial_time is None:
            print("Failed.")
            continue
        print(f"Done! ({serial_time:.4f}s)")
        
        # Β. Μέτρηση Παράλληλου (Mode 1)
        parallel_times = []
        for t in THREADS:
            print(f"   Running Parallel with {t} threads...", end=" ", flush=True)
            t_time = run_benchmark(size, 1, t)
            if t_time is not None:
                parallel_times.append(t_time)
                print(f"Done! ({t_time:.4f}s)")
            else:
                parallel_times.append(0)
        
        # 3. Δημιουργία Γραφήματος
        plot_results(size, serial_time, parallel_times, OUTPUT_IMAGES[i])
        print("\n")

if __name__ == "__main__":
    main()