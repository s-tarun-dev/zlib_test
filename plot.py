import matplotlib.pyplot as plt 
import pandas as pd 
import numpy as np

df1 = pd.read_csv("./gzip_c6.csv")
df2 = pd.read_csv("./gzip_c7.csv")

l1 = 6
l2 = 7
version = '1.2.11'
is_for = 'time'

files = df1["file"].to_list()

if is_for == 'time':
  mode = 'Time Taken'
  unit = 'ms'
  size1 = df1["deflate_compression_time(ms)"].to_list() 
  size2 = df2["deflate_compression_time(ms)"].to_list() 
else:
  mode = 'Compressed Size'
  unit = 'kB'
  size1 = df1["compressed_size(bytes)"].to_list() 
  size2 = df2["compressed_size(bytes)"].to_list() 

rss1 = df1["resident_set_size(bytes)"].to_list()
rss2 = df2["resident_set_size(bytes)"].to_list()

rss_change = []
diff = []

if is_for == 'size':
  for i in range(len(size1)):
      size1[i] = size1[i] / 1024.0

  for i in range(len(size2)):
      size2[i] = size2[i] / 1024.0

for i in range(len(size1)):
  if size1[i] == 0:
    diff.append(0)
  else:
    diff.append(round((((size1[i] - size2[i]) / size1[i])*100), 2))
  rss_change.append(rss2[i] - rss1[i])

net_change = sum(diff) / len(diff)

p10 = np.percentile(diff, 10)
p20 = np.percentile(diff, 20)
p50 = np.percentile(diff, 50)
p90 = np.percentile(diff, 90)
p99 = np.percentile(diff, 99)

fig, ax = plt.subplots(figsize=(20,10))

width = 0.35
x = range(len(files))

rects1 = ax.bar(x, size1, width, label= f'{mode} at Level {l1}')
rects2 = ax.bar([i+width for i in x], size2, width, label = f'{mode} at Level {l2}')
rects3 = ax.bar([i+width*2 for i in x], diff, width, label = f'Percentage change in {is_for}', color = 'lightgray') 

ax.set_xlabel('File Name')
ax.set_ylabel(f'{is_for} ({unit})') 

plt.title('Average change ' + str(round(net_change, 2)) + '%' + '\n\n' +'10th Percentile: ' + str(round(p10, 2)) + '%' + ' | 20th Percentile: ' + str(round(p20, 2)) + '%' + ' | 50th Percentile: ' + str(round(p50, 2)) + '%' + ' | 90th Percentile: ' + str(round(p90, 2)) + '%' +  ' | 99th Percentile: ' + str(round(p99, 2)) + '%' , fontsize = 10)
plt.suptitle(f'{mode}: Compression Level {l1} vs Compression Level {l2}', fontsize = 10) 
ax.set_xticks([i+width/2 for i in x])
ax.set_xticklabels(files, rotation = 45)
ax.legend()
def add_labels(bars):
  for bar in bars:
    height = bar.get_height()
    ax.annotate(f'{height: .2f}', xy = (bar.get_x() + bar.get_width() / 2, height), xytext = (0,3), textcoords = "offset points", ha = "center", va = 'bottom', fontsize=8, rotation = 45)

add_labels(rects1)
add_labels(rects2)
add_labels(rects3)

plt.tight_layout()
plt.savefig(f'c{l1}_vs_c{l2}_v{version}_{is_for}.png') 
plt.show()
