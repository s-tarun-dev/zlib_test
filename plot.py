import matplotlib.pyplot as plt 
import pandas as pd 
import numpy as np

df1 = pd.read_csv("/path/to/csv") #Path A
df2 = pd.read_csv("/path/to/csv") #Path B

l1 = 6 #Level A
l2 = 7 #Level B
version = '1.3.1' #Version
is_for = 'time' #Parameter: time/size

files = df2["Original File Name"].to_list()

if is_for == 'time':
  mode = 'Time Taken'
  unit = 'ms'
  value1 = df1["Time Taken in Compression(ms)"].to_list() 
  value2 = df2["Time Taken in Compression(ms)"].to_list() 
else:
  mode = 'Compressed Size'
  unit = 'kB'
  value1 = df1["Compressed File Size(B)"].to_list() 
  value2 = df2["Compressed File Size(B)"].to_list() 

diff = []

if is_for == 'size':
  for i in range(len(value1)):
      value1[i] = value1[i] / 1024.0

  for i in range(len(value2)):
      value2[i] = value2[i] / 1024.0

for i in range(len(value1)):
  if value1[i] == 0:
    diff.append(0)
  else:
    diff.append(round((((value1[i] - value2[i]) / value1[i])*100), 2))

net_change = sum(diff) / len(diff)

p10 = np.percentile(diff, 10)
p20 = np.percentile(diff, 20)
p50 = np.percentile(diff, 50)
p90 = np.percentile(diff, 90)
p99 = np.percentile(diff, 99)

fig, ax = plt.subplots(figsize=(20,10))

width = 0.35
x = range(len(files))

rects1 = ax.bar(x, value1, width, label= f'{mode} at Level {l1}') 
rects2 = ax.bar([i+width for i in x], value2, width, label = f'{mode} at Level {l2}') 
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
    if is_for == 'time':
      height = int(height) 
    else:
      height = round(height, 2) 
    ax.annotate(f'{height}', xy = (bar.get_x() + bar.get_width() / 2, height), xytext = (0,3), textcoords = "offset points", ha = "center", va = 'bottom', fontsize=8, rotation = 45)

add_labels(rects1)
add_labels(rects2)
add_labels(rects3)

plt.tight_layout()
plt.savefig(f'c{l1}_vs_c{l2}_v{version}_{is_for}.png') 
plt.show()
