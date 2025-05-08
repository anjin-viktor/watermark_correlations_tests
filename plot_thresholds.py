# -*- coding: cp1251 -*-

import matplotlib.pyplot as plt
import numpy as np
import sys
import csv

if len(sys.argv) != 3:
    print("`correlations.csv` and `correlations_wo_embedding.csv` arguments expected")
    exit(1)

y = []
x = []

idx = 3
n_bins = 1000
xmin = 0
xmax = 0.01

with open(sys.argv[1]) as csvfile:
    reader = csv.reader(csvfile, quoting=csv.QUOTE_MINIMAL)
    for row in reader:
        y.append(float(row[idx]))

with open(sys.argv[2]) as csvfile:
    reader = csv.reader(csvfile, quoting=csv.QUOTE_MINIMAL)
    for row in reader:
        y.append(float(row[idx]))

y = np.array(y)

fig, axs = plt.subplots(1, 1, sharey=True, tight_layout=True)

axs.hist(y, bins=n_bins)

if idx == 1:
    plt.title("Linear Correlation")
elif idx == 2:
    axs.set(xlim=(0, 4000))
    plt.title("Correlation Coefficient")
elif idx == 3:
    axs.set(xlim=(0, 0.3))
    plt.title("Pearson Coefficient")

plt.xlabel("Correlation")
plt.ylabel("Number of images")

plt.show()
