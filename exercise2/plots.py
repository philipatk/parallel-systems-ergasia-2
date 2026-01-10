import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

def generate_final_plots():
    # 1. Φόρτωση Δεδομένων
    try:
        df = pd.read_csv('results_exercise_2_2.csv')
        print("Data loaded successfully.")
    except FileNotFoundError:
        print("Error: Το αρχείο 'results_exercise_2_2.csv' δεν βρέθηκε.")
        return

    # 2. Ρυθμίσεις Φιλτραρίσματος
    # Αντιστοίχιση των ζητούμενων τιμών στις υπάρχουσες του CSV
    size_map = {1000: 1000, 5000: 4000, 10000: 10000}
    sparsity_map = {0: 0, 50: 60, 90: 90, 99: 99}
    target_threads = [1, 2, 4, 8, 16]

    # Δημιουργία φιλτραρισμένου αντιγράφου
    df_filtered = df[
        (df['ArraySide'].isin(size_map.values())) &
        (df['Sparsity'].isin(sparsity_map.values())) &
        (df['Threads'].isin(target_threads))
    ].copy()
    
    # Ταξινόμηση για σωστή σειρά στα γραφήματα
    df_filtered.sort_values(by=['ArraySide', 'Sparsity', 'Threads'], inplace=True)

    # 3. Ρυθμίσεις Εμφάνισης
    sns.set_style("whitegrid")
    
    # Επιλογή παλέτας με έντονη αντίθεση (Tab10) για να φαίνεται και το 99%
    # Μπλε=0%, Πορτοκαλί=60%, Πράσινο=90%, Κόκκινο=99%
    custom_palette = sns.color_palette("tab10", 4) 

    metrics = [
        ('CSR_Build', 'CSR Construction Time'),
        ('CSR_Mul', 'CSR Multiplication Time'),
        ('Dense_Mul', 'Dense Multiplication Time')
    ]

    # 4. Δημιουργία των 3 Διαγραμμάτων
    for metric_prefix, title_text in metrics:
        col_parallel = f"{metric_prefix}_Parallel"
        col_serial = f"{metric_prefix}_Serial"
        
        unique_sizes = sorted(df_filtered['ArraySide'].unique())
        
        # Ένα subplot για κάθε μέγεθος πίνακα (ArraySide)
        fig, axes = plt.subplots(1, len(unique_sizes), figsize=(18, 6))
        if len(unique_sizes) == 1: axes = [axes]

        fig.suptitle(f"{title_text}\nSolid Line: Parallel (OpenMP) | Dashed Line: Serial (Baseline)", fontsize=15)

        for i, size in enumerate(unique_sizes):
            ax = axes[i]
            subset = df_filtered[df_filtered['ArraySide'] == size]
            
            # --- Plot Parallel (Συμπαγείς Γραμμές) ---
            sns.lineplot(
                data=subset, 
                x='Threads', 
                y=col_parallel, 
                hue='Sparsity', 
                style='Sparsity', 
                markers=True, 
                dashes=False, 
                palette=custom_palette, 
                linewidth=2.5, 
                ax=ax
            )
            
            # --- Plot Serial (Διακεκομμένες Γραμμές) ---
            unique_sparsities = sorted(subset['Sparsity'].unique())
            
            for j, sp in enumerate(unique_sparsities):
                # Παίρνουμε τον μέσο όρο του σειριακού χρόνου για αυτό το Sparsity
                serial_val = subset[subset['Sparsity'] == sp][col_serial].mean()
                
                # Σχεδιάζουμε οριζόντια γραμμή με το ίδιο χρώμα που έχει το αντίστοιχο Sparsity
                ax.axhline(
                    y=serial_val, 
                    color=custom_palette[j], 
                    linestyle='--', 
                    alpha=0.8, 
                    linewidth=2
                )

            ax.set_title(f"Array Size: {size}")
            ax.set_xlabel("Threads")
            ax.set_ylabel("Time (seconds)")
            ax.set_xticks(target_threads)
            
            # Διαχείριση Legend: Εμφάνιση μόνο στο τελευταίο subplot
            if i != len(unique_sizes) - 1:
                if ax.get_legend(): ax.get_legend().remove()
            else:
                ax.legend(title='Sparsity (%)', loc='upper right')

        plt.tight_layout()
        
        # Αποθήκευση
        filename = f"{metric_prefix}_final_plot.png"
        plt.savefig(filename)
        print(f"Graph saved: {filename}")
        plt.show()

if __name__ == "__main__":
    generate_final_plots()