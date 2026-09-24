import subprocess
import re
import csv
import sys
from pathlib import Path

# Список исполняемых файлов для запуска
EXECUTABLES = [
    "./build/main_naive",
    "./build/main_ikj",
    "./build/main_sse",
    "./build/main_cuda"
]

# Настройки бенчмарка
NUM_RUNS = 5
OUTPUT_FILE = "benchmark_results.csv"

def run_executable(executable_path):
    """
    Запускает бинарник и извлекает числовые данные из строки вида 'RESULT: 123.456'
    """
    try:
        # Запускаем процесс и считываем stdout
        result = subprocess.run(
            [executable_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=True
        )
        
        # Ищем число с плавающей точкой после слова RESULT:
        match = re.search(r"RESULT:\s*([\d\.]+)", result.stdout)
        if match:
            return float(match.group(1))
        else:
            print(f"  [!] Ошибка: Не удалось распарсить вывод программы {executable_path}")
            print(f"      Вывод: {result.stdout.strip()}")
            return None

    except FileNotFoundError:
        print(f"  [!] Ошибка: Файл '{executable_path}' не найден. Вы скомпилировали проект?")
        return None
    except subprocess.CalledProcessError as e:
        print(f"  [!] Ошибка при выполнении {executable_path}: {e.stderr.strip()}")
        return None

def main():
    print("=" * 60)
    print(f"Старт бенчмарка умножения матриц ({NUM_RUNS} прогонов на программу)")
    print("=" * 60)

    # Словарь для хранения всех запусков: {имя_файла: [время_1, время_2, ...]}
    results = {}

    for exe in EXECUTABLES:
        exe_path = Path(exe)
        
        # На Windows бинарники могут иметь расширение .exe
        if sys.platform == "win32" and exe_path.suffix != ".exe":
            exe_path = exe_path.with_suffix(".exe")

        print(f"\nТестирование: {exe_path.name}")
        results[exe_path.name] = []

        for run in range(1, NUM_RUNS + 1):
            print(f"  Запуск {run}/{NUM_RUNS}...", end=" ", flush=True)
            execution_time = run_executable(str(exe_path))
            
            if execution_time is not None:
                results[exe_path.name].append(execution_time)
                print(f"{execution_time:.2f} ms")
            else:
                print("Сбой!")

    # Запись результатов в CSV-файл
    print("\n" + "=" * 60)
    print(f"Сохранение результатов в {OUTPUT_FILE}...")
    
    with open(OUTPUT_FILE, mode="w", newline="", encoding="utf-8") as csv_file:
        writer = csv.writer(csv_file, delimiter=";")
        
        # Заголовки столбцов
        headers = ["Исполняемый файл"] + [f"Запуск {i}" for i in range(1, NUM_RUNS + 1)] + ["Среднее время (мс)"]
        writer.writerow(headers)

        # Данные по каждому бинарнику
        for exe_name, times in results.items():
            if times:
                avg_time = sum(times) / len(times)
                row = [exe_name] + [f"{t:.2f}" for t in times] + [f"{avg_time:.2f}"]
                writer.writerow(row)
            else:
                writer.writerow([exe_name] + ["N/A"] * NUM_RUNS + ["N/A"])

    print(f"Готово! Результаты сохранены в '{OUTPUT_FILE}'.")
    print("=" * 60)

if __name__ == "__main__":
    main()
