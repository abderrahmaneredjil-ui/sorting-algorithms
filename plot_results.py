"""Optional bonus: turns results/results.csv into one chart per input type.
Usage:  pip install matplotlib   then   python plot_results.py"""
import csv
from collections import defaultdict
import matplotlib.pyplot as plt

data = defaultdict(lambda: defaultdict(list))   
with open("results/results.csv", newline="") as f:
    for row in csv.DictReader(f):
        data[row["input"]][row["algorithm"]].append((int(row["size"]), float(row["wall_ms"])))

for input_type, algos in data.items():
    plt.figure(figsize=(7, 4.5))
    for name, points in algos.items():
        plt.plot(*zip(*points), marker="o", label=name)
    plt.xscale("log"); plt.yscale("log")
    plt.xlabel("Array size n"); plt.ylabel("Time (ms)")
    plt.title(f"Sorting time - {input_type} data"); plt.legend(); plt.grid(True, which="both", alpha=0.3)
    plt.tight_layout()
    plt.savefig(f"results/chart_{input_type}.png", dpi=130)
    plt.close()
print("Charts saved in results/")
