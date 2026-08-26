#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import os

def plot_benchmarks():
    csv_path = 'benchmark_results.csv'
    
    if not os.path.exists(csv_path):
        print(f"Error: {csv_path} not found! Run the benchmark in C++ app first.")
        return

    # 1. Читаем данные из CSV
    df = pd.read_csv(csv_path)

    # 2. Настройка стиля графика
    plt.figure(figsize=(10, 6), dpi=300)
    plt.style.use('seaborn-v0_8-whitegrid' if 'seaborn-v0_8-whitegrid' in plt.style.available else 'default')


    # 3. Отрисовка кривых
    plt.plot(df['Length'], df['Regex'] / 1000.0, marker='o', linewidth=2, color='#e74c3c', label='std::regex')
    plt.plot(df['Length'], df['Flex'] / 1000.0, marker='s', linewidth=2, color='#3498db', label='Flex Lexer')
    plt.plot(df['Length'], df['SMC'] / 1000.0, marker='^', linewidth=2, color='#2ecc71', label='SMC (State Machine)')

    #
    plt.title('Recognizer Performance Benchmark (O3 Optimized)', fontsize=14, fontweight='bold', pad=15)
    plt.xlabel('String Length (characters)', fontsize=12, labelpad=10)
    plt.ylabel('Execution Time (µs)', fontsize=12, labelpad=10)
    
    plt.legend(fontsize=11, frameon=True, shadow=True)
    plt.grid(True, linestyle='--', alpha=0.7)

    #
    output_png = 'benchmark_plot.png'
    plt.tight_layout()
    plt.savefig(output_png)
    print(f"Plot successfully saved to {output_png}")
    plt.show()

if __name__ == '__main__':
    plot_benchmarks()