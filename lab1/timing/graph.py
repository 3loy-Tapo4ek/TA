import csv
import matplotlib.pyplot as plt

x_chars = []
y_regex = []
y_smc = []
y_lex = []

filename = 'data.csv'

try:
    with open(filename, 'r', encoding='utf-8') as file:
        reader = csv.reader(file)
        lines = list(reader)

        for row in lines[2:]:
            if len(row) >= 5 and row[1].strip().isdigit():
                x_chars.append(int(row[1]))
                y_regex.append(float(row[2]))
                y_smc.append(float(row[3]))
                y_lex.append(float(row[4]))
except FileNotFoundError:
    print(f"Ошибка: Файл {filename} не найден. Положите его рядом со скриптом.")
    exit(1)

plt.figure(figsize=(10, 6))

plt.plot(x_chars, y_regex, label='Ручной (Regex)', marker='o', linewidth=2, color='#1f77b4')
plt.plot(x_chars, y_smc, label='Автомат (SMC)', marker='s', linewidth=2, color='#d62728')
plt.plot(x_chars, y_lex, label='Flex (Lex)', marker='^', linewidth=2, color='#2ca02c')

plt.title('Зависимость времени выполнения от длины входной строки', fontsize=14, fontweight='bold', pad=15)
plt.xlabel('Количество символов', fontsize=12)
plt.ylabel('Время выполнения (мс)', fontsize=12)

plt.grid(True, linestyle='--', alpha=0.7)

plt.legend(fontsize=12, loc='upper left')

plt.xticks(x_chars, rotation=45)

plt.tight_layout()

plt.savefig('benchmark_result.png', dpi=300)

plt.show()