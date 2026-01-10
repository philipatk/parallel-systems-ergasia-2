import subprocess
import csv
import re
import statistics
import os
import sys

# Προσπάθεια εισαγωγής του matplotlib για τα γραφήματα
try:
    import matplotlib.pyplot as plt
except ImportError:
    print("❌ Error: 'matplotlib' library is missing.")
    print("   Please install it using: pip install matplotlib")
    sys.exit(1)

# --- ΡΥΘΜΙΣΕΙΣ ---
# Διαδρομές βάσει του Makefile
EX_NUM = "2"
EXECUTABLE = f"./exercise{EX_NUM}/build/ex{EX_NUM}"
OUTPUT_CSV = "results_exercise_2_2.csv"
PLOT_FILENAME = "performance_plots.png"
NUM_RUNS = 4  # Πόσες φορές τρέχει το κάθε πείραμα για μέσο όρο

# --- ΟΙ ΠΑΡΑΜΕΤΡΟΙ ΠΟΥ ΖΗΤΗΣΕΣ ---
TEST_PARAMS = {
    "array_sides": [1000, 4000, 7000, 10000],
    "sparsity": [0, 20, 40, 60, 80, 90, 99],
    "iterations": [20],
    "threads": [1, 2, 4, 8, 16, 32]
}

def compile_code():
    print("🔨 Building project using Make...")
    # Καθαρισμός και compile
    try:
        # Τρέχουμε make clean και make EX=2
        subprocess.run(["make", "clean", f"EX={EX_NUM}"], check=True, stdout=subprocess.DEVNULL)
        subprocess.run(["make", f"EX={EX_NUM}"], check=True)
        print("✅ Compilation Successful!")
    except subprocess.CalledProcessError:
        print("❌ Make Failed! Check your C files and Makefile.")
        sys.exit(1)

def parse_output(output_str):
    data = {}
    
    # Mapping: Όνομα στη C -> Όνομα στο CSV
    # Το regex ταιριάζει με τα printf του main.c σου
    patterns = {
        'CSR_Build_Serial': r"csr Initialization Serial:\s+([0-9.]+)",
        'CSR_Build_Parallel': r"csr Initialization Parallel:\s+([0-9.]+)",
        'CSR_Mul_Serial': r"csr Multiplication Serial:\s+([0-9.]+)",
        'CSR_Mul_Parallel': r"csr Multiplication Parallel:\s+([0-9.]+)",
        'Dense_Mul_Serial': r"Initial Array Multiplication Serial:\s+([0-9.]+)",
        'Dense_Mul_Parallel': r"Initial Array Multiplication Parallel:\s+([0-9.]+)",
    }

    for key, regex in patterns.items():
        match = re.search(regex, output_str)
        if match:
            data[key] = float(match.group(1))
            
    return data

def run_experiment():
    with open(OUTPUT_CSV, mode='w', newline='') as csv_file:
        fieldnames = [
            'ArraySide', 'Sparsity', 'Iterations', 'Threads',
            'CSR_Build_Serial', 'CSR_Build_Parallel',
            'CSR_Mul_Serial', 'CSR_Mul_Parallel',
            'Dense_Mul_Serial', 'Dense_Mul_Parallel',
            'Verification'
        ]
        writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        writer.writeheader()

        # Υπολογισμός συνολικών συνδυασμών
        total_configs = (len(TEST_PARAMS["array_sides"]) * len(TEST_PARAMS["sparsity"]) * len(TEST_PARAMS["iterations"]) * len(TEST_PARAMS["threads"]))
        current_exp = 0

        print(f"🚀 Starting {total_configs} configurations (x{NUM_RUNS} repeats each)... This may take a while.")

        for n in TEST_PARAMS["array_sides"]:
            for sp in TEST_PARAMS["sparsity"]:
                for iters in TEST_PARAMS["iterations"]:
                    for th in TEST_PARAMS["threads"]:
                        current_exp += 1
                        print(f"[{current_exp}/{total_configs}] N={n}, Sparsity={sp}%, Threads={th} ", end="", flush=True)
                        
                        timings = {k: [] for k in ['CSR_Build_Serial', 'CSR_Build_Parallel', 'CSR_Mul_Serial', 'CSR_Mul_Parallel', 'Dense_Mul_Serial', 'Dense_Mul_Parallel']}
                        verification_status = "PASS"

                        for r in range(NUM_RUNS):
                            try:
                                # ./exercise2/build/ex2 <size> <sparsity> <iter> <threads>
                                cmd = [EXECUTABLE, str(n), str(sp), str(iters), str(th)]
                                
                                # Timeout στα 3 λεπτά ανά run για ασφάλεια (αυξήθηκε λόγω μεγάλων πινάκων)
                                result = subprocess.run(
                                    cmd, capture_output=True, text=True, timeout=180
                                )
                                
                                # Αν το C πρόγραμμα επιστρέψει != 0 (π.χ. λάθος αποτελέσματα)
                                if result.returncode != 0:
                                    verification_status = "FAIL"
                                    print("x", end="", flush=True)
                                    continue

                                data = parse_output(result.stdout)
                                if not data: 
                                    print("?", end="", flush=True)
                                    continue

                                for key in timings:
                                    if key in data: timings[key].append(data[key])
                                print(".", end="", flush=True)
                                    
                            except subprocess.TimeoutExpired:
                                verification_status = "TIMEOUT"
                                print("T", end="", flush=True)
                            except Exception as e:
                                print(f"Err: {e}", end="")
                                verification_status = "ERR"

                        # Υπολογισμός Μέσου Όρου
                        avg_results = {}
                        if verification_status == "PASS" and len(timings['CSR_Mul_Parallel']) > 0:
                            for key, val_list in timings.items():
                                if val_list:
                                    avg_results[key] = statistics.mean(val_list)
                                else:
                                    avg_results[key] = 0.0
                        else:
                            for key in timings: avg_results[key] = 0.0

                        row = {'ArraySide': n, 'Sparsity': sp, 'Iterations': iters, 'Threads': th, 'Verification': verification_status, **avg_results}
                        writer.writerow(row)
                        csv_file.flush()
                        print(" Done.")
                        
    print(f"\n✅ Experiments finished. CSV saved to {OUTPUT_CSV}.")

def generate_plots():
    print("📊 Generating plots...")
    
    data = []
    if not os.path.exists(OUTPUT_CSV):
        print("❌ CSV file not found.")
        return

    with open(OUTPUT_CSV, mode='r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            for key in row:
                if key != 'Verification':
                    try: row[key] = float(row[key])
                    except: pass
            data.append(row)

    # Φιλτράρισμα: Παίρνουμε το μεγαλύτερο ArraySide (10000) για τα γραφήματα
    target_N = TEST_PARAMS["array_sides"][-1] 
    
    subset = [d for d in data if d['ArraySide'] == target_N and d['Verification'] == 'PASS']

    if not subset:
        print(f"⚠️ No valid data found for ArraySide={target_N}. Trying smaller size...")
        target_N = TEST_PARAMS["array_sides"][0]
        subset = [d for d in data if d['ArraySide'] == target_N and d['Verification'] == 'PASS']
        if not subset:
            print("❌ No valid data found at all.")
            return

    # --- Setup Figure ---
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 7))
    fig.suptitle(f'Exercise 2.2 Performance Analysis (Matrix Size: {target_N}x{target_N})', fontsize=16)

    # --- PLOT 1: Speedup vs Threads (CSR Multiplication) ---
    # Δείχνουμε γραμμές για επιλεγμένα sparsities για να μην "πήξει" το γράφημα
    sparsities_to_plot = [0, 40, 90, 99] 
    
    for sp in sparsities_to_plot:
        # Φιλτράρισμα βάσει sparsity
        sp_data = [d for d in subset if d['Sparsity'] == sp]
        sp_data.sort(key=lambda x: x['Threads'])
        
        threads = [d['Threads'] for d in sp_data]
        speedups = []
        for d in sp_data:
            if d['CSR_Mul_Parallel'] > 0 and d['CSR_Mul_Serial'] > 0:
                speedups.append(d['CSR_Mul_Serial'] / d['CSR_Mul_Parallel'])
            else:
                speedups.append(0)
        
        if threads:
            ax1.plot(threads, speedups, marker='o', label=f'Sparsity {int(sp)}%')

    ax1.set_title('Parallel Scalability (CSR Mul Speedup)')
    ax1.set_xlabel('Number of Threads')
    ax1.set_ylabel('Speedup (Serial / Parallel)')
    ax1.grid(True, linestyle='--', alpha=0.6)
    ax1.legend()
    
    # Ιδανική γραμμή scaling
    max_th = max(TEST_PARAMS["threads"])
    ax1.plot([1, max_th], [1, max_th], 'k--', alpha=0.3, label='Ideal')

    # --- PLOT 2: Execution Time vs Sparsity (CSR vs Dense) ---
    # Σύγκριση στους μέγιστους πυρήνες (Threads = 32)
    max_threads = max(TEST_PARAMS["threads"])
    
    perf_data = [d for d in subset if d['Threads'] == max_threads]
    perf_data.sort(key=lambda x: x['Sparsity'])
    
    if perf_data:
        sparsities = [d['Sparsity'] for d in perf_data]
        csr_times = [d['CSR_Mul_Parallel'] for d in perf_data]
        dense_times = [d['Dense_Mul_Parallel'] for d in perf_data]
        
        ax2.plot(sparsities, csr_times, marker='s', color='blue', linewidth=2, label=f'CSR Parallel ({max_threads} thr)')
        ax2.plot(sparsities, dense_times, marker='^', color='red', linestyle='--', linewidth=2, label=f'Dense Parallel ({max_threads} thr)')
        
        ax2.set_title(f'Execution Time Comparison: CSR vs Dense')
        ax2.set_xlabel('Sparsity (% Zeros)')
        ax2.set_ylabel('Time (seconds)')
        ax2.grid(True, linestyle='--', alpha=0.6)
        ax2.legend()
    
    plt.tight_layout()
    plt.savefig(PLOT_FILENAME)
    print(f"✅ Plots saved to '{PLOT_FILENAME}'")

if __name__ == "__main__":
    compile_code()
    run_experiment()
    generate_plots()