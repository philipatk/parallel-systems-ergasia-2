import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Ρυθμίσεις εμφάνισης για ακαδημαϊκό στυλ
sns.set_style("whitegrid")
plt.rcParams['font.size'] = 12

def load_and_clean_data(filename):
    try:
        # Διάβασμα CSV και αφαίρεση κενών από τα ονόματα των στηλών
        df = pd.read_csv(filename)
        df.columns = df.columns.str.strip()
        return df
    except Exception as e:
        print(f"Σφάλμα κατά το άνοιγμα του {filename}: {e}")
        return None

def find_column(df, keyword):
    # Βοηθητική συνάρτηση για εύρεση στήλης ανεξαρτήτως πεζών/κεφαλαίων
    for col in df.columns:
        if keyword.lower() in col.lower():
            return col
    return None

# --- ΚΥΡΙΩΣ ΠΡΟΓΡΑΜΜΑ ---

# 1. Φόρτωση δεδομένων
df_pth = load_and_clean_data('results_ex1_pthreads.csv')
df_omp = load_and_clean_data('results_omp.csv')

if df_pth is not None and df_omp is not None:
    
    # 2. Εντοπισμός Στηλών (N, Threads, Time)
    # Ψάχνουμε για στήλες που περιέχουν "N" ή "Size", "Thread", "Time"
    n_col_pth = find_column(df_pth, 'N') or find_column(df_pth, 'Size')
    t_col_pth = find_column(df_pth, 'Thread')
    time_col_pth = find_column(df_pth, 'Time')

    n_col_omp = find_column(df_omp, 'N') or find_column(df_omp, 'Size')
    t_col_omp = find_column(df_omp, 'Thread')
    time_col_omp = find_column(df_omp, 'Time')

    # 3. Επιλογή του Μεγέθους (N) για σύγκριση
    # Βρίσκουμε το μεγαλύτερο κοινό N για να φανεί καλά η διαφορά
    common_n = set(df_pth[n_col_pth]).intersection(set(df_omp[n_col_omp]))
    
    if common_n:
        target_n = max(common_n)
        print(f"Δημιουργία γραφήματος για Πολυώνυμα βαθμού N = {target_n}")
    else:
        # Αν δεν υπάρχει κοινό, παίρνουμε το μέγιστο του καθενός (fallback)
        target_n = df_omp[n_col_omp].max()
        print(f"Προσοχή: Δεν βρέθηκε κοινό N. Χρησιμοποιείται N={target_n}")

    # Φιλτράρισμα των δεδομένων για το συγκεκριμένο N
    data_pth = df_pth[df_pth[n_col_pth] == target_n].sort_values(by=t_col_pth)
    data_omp = df_omp[df_omp[n_col_omp] == target_n].sort_values(by=t_col_omp)

    # 4. Σχεδίαση Γραφήματος
    plt.figure(figsize=(10, 6))

    # Γραμμή Pthreads (Μπλε κύκλοι)
    plt.plot(data_pth[t_col_pth], data_pth[time_col_pth], 
             marker='o', markersize=8, linestyle='-', linewidth=2.5, label='Pthreads (Ex 1.1)')

    # Γραμμή OpenMP (Πορτοκαλί τετράγωνα)
    plt.plot(data_omp[t_col_omp], data_omp[time_col_omp], 
             marker='s', markersize=8, linestyle='--', linewidth=2.5, label='OpenMP (Ex 2.1)')

    # Διαμόρφωση αξόνων και τίτλων
    plt.title(f'Performance Comparison: Pthreads vs OpenMP\nPolynomial Multiplication (Degree N={target_n})', fontsize=14)
    plt.xlabel('Number of Threads', fontsize=12)
    plt.ylabel('Execution Time (seconds)', fontsize=12)
    plt.legend(fontsize=11, loc='best')
    
    # Ρύθμιση αξόνων (για να δείχνει ακέραια νήματα)
    all_threads = pd.concat([data_pth[t_col_pth], data_omp[t_col_omp]]).unique()
    plt.xticks(sorted(all_threads))
    
    plt.grid(True, which='major', linestyle='-', alpha=0.8)
    plt.grid(True, which='minor', linestyle=':', alpha=0.4)
    plt.minorticks_on()

    plt.tight_layout()
    plt.savefig('comparison_pthreads_openmp.png', dpi=300)
    plt.show()
    print("Το γράφημα αποθηκεύτηκε ως 'comparison_pthreads_openmp.png'.")