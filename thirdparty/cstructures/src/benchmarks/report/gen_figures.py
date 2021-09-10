from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
import json
import re
from matplotlib.ticker import FormatStrFormatter


def parse_bench_name(name):
    key_size, value_size, insertions = re.match(r"^.*char\[1<<(\d+)\], char\[1<<(\d+)\]>\/(\d+)$", name).groups()
    return 1 << int(key_size), 1 << int(value_size), int(insertions)


def filter_kv(tuples, k, v):
    x, y = zip(*[(x[2], x[3]) for x in tuples if x[0] == k and x[1] == v])
    return np.array(x), np.array(y)


def filter_valuesize_insertions(tuples, valuesize, insertions):
    x, y = zip(*[(x[0], x[3]) for x in tuples if x[1] == valuesize and x[2] == insertions])
    return np.array(x), np.array(y)


def filter_same_kv(tuples):
    x, y, z = zip(*[(x[1], x[2], x[3]) for x in tuples if x[0] == x[1]])
    return np.log10(np.array(x)), np.log10(np.array(y)), np.log10(np.array(z))


def filter_insertions_3d(tuples, insertions):
    x, y, z = zip(*[(x[0], x[1], x[3]) for x in tuples if x[2] == insertions])
    return np.log10(np.array(x)), np.log10(np.array(y)), np.log10(np.array(z))


def filter_valuesize_3d(tuples, value_size):
    x, y, z = zip(*[(x[0], x[2], x[3]) for x in tuples if x[1] == value_size])
    return np.log10(np.array(x)), np.log10(np.array(y)), np.log10(np.array(z))


def filter_insertions_2d(tuples, insertions):
    x, y = zip(*[(x[0], x[3]) for x in tuples if x[1] == 8 and x[2] == insertions])
    return x, y


bench = json.loads(open("benchmarks.json", "rb").read().decode("utf-8"))

hashmap_insertions = [x for x in bench["benchmarks"] if x["name"].startswith("BM_HashmapInsert")]
std_unordered_map_insertions = [x for x in bench["benchmarks"] if x["name"].startswith("BM_StdUnorderedMap")]

hashmap_tuples = [(*parse_bench_name(x["name"]), float(x["cpu_time"])) for x in hashmap_insertions]
unordered_map_tuples = [(*parse_bench_name(x["name"]), float(x["cpu_time"])) for x in std_unordered_map_insertions]

fig = plt.figure()
ax = fig.add_subplot(221, projection="3d", proj_type="persp")
ax.plot_trisurf(*filter_valuesize_3d(hashmap_tuples, 1<<8))
ax.set_xlabel("Key Size")
ax.set_ylabel("Insertions")
ax.set_zlabel("Insertion time")
ticks = 2**np.linspace(2, 8, 7)
ax.xaxis.set_ticks(np.log10(ticks))
ax.xaxis.set_ticklabels(int(x) for x in ticks)
ticks = 2**(np.linspace(1, 8, 8)*2)
ax.yaxis.set_ticks(np.log10(ticks))
ax.yaxis.set_ticklabels(int(x) for x in ticks)
ax.set_title("hashmap_t")

ax = fig.add_subplot(222, projection="3d", proj_type="persp")
ax.plot_trisurf(*filter_valuesize_3d(unordered_map_tuples, 1<<8))
ax.set_xlabel("Key Size")
ax.set_ylabel("Insertions")
ax.set_zlabel("Insertion time")
ticks = 2**np.linspace(2, 8, 7)
ax.xaxis.set_ticks(np.log10(ticks))
ax.xaxis.set_ticklabels(int(x) for x in ticks)
ticks = 2**(np.linspace(1, 8, 8)*2)
ax.yaxis.set_ticks(np.log10(ticks))
ax.yaxis.set_ticklabels(int(x) for x in ticks)
ax.set_title("std::unordered_map")

ax = fig.add_subplot(223)
ax.loglog(*filter_valuesize_insertions(hashmap_tuples, 1<<8, 1<<0))
ax.loglog(*filter_valuesize_insertions(unordered_map_tuples, 1<<8, 1<<0))
ax.set_xlabel("Key Size")
ax.set_ylabel("Insertion Time")
ax.legend(["hashmap_t", "std::unordered_map"])

ax = fig.add_subplot(224)
ax.loglog(*filter_valuesize_insertions(hashmap_tuples, 1<<8, 1<<8))
ax.loglog(*filter_valuesize_insertions(unordered_map_tuples, 1<<8, 1<<8))
ax.set_xlabel("Key Size")
ax.set_ylabel("Insertion Time")
ax.legend(["hashmap_t", "std::unordered_map"])

#ax = fig.add_subplot(211, projection="3d", proj_type="persp")
#ax.plot_trisurf(*filter_insertions(hashmap_tuples, 1<<4))
#ax.set_xlabel("Key Size")
#ax.set_ylabel("Value Size")
#ax.set_title("hashmap_t")

#ax = fig.add_subplot(212, projection="3d", proj_type="persp")
#ax.plot_trisurf(*filter_insertions(unordered_map_tuples, 1<<4))
#ax.set_xlabel("Key Size")
#ax.set_ylabel("Value Size")
#ax.set_title("std::unordered_map")

plt.show()
