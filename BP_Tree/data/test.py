# Generate SOURCE
# import random

# res = random.sample(range(1, 101), 100)
# with open('rng_analysis', 'w') as f:
#     for num in res:
#         f.write("INSERT "+str(num)+"\n")
#     for i in range(1,100):
#         f.write("RANGE "+str(i)+" "+str(i+1)+"\n")

import matplotlib.pyplot as plt
fos = ['3','5','7']

for fo in fos:
    # Plot histograms
    bpt_freq = {}
    heap_freq = {}
    with open(fo+'_access.txt', 'r') as f:
        for row in f.readlines():
            bpt, heap = row[:-1].split(' ')
            if bpt in bpt_freq:
                bpt_freq[bpt] += 1
            else:
                bpt_freq[bpt] = 1
            if heap in heap_freq:
                heap_freq[heap] += 1
            else:
                heap_freq[heap] = 1

    heap_freq['24'] = 0
    heap_freq['26'] = 0
    heap_freq = dict(sorted(heap_freq.items()))
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 4))

    # Plotting the first histogram
    ax1.bar(list(bpt_freq.keys()), list(bpt_freq.values()), color='#fca503')
    ax1.set_title('B plus tree')
    ax1.set_xlabel('Accesses')
    ax1.set_ylabel('Frequency')

    # Plotting the second histogram
    ax2.bar(list(heap_freq.keys()), list(heap_freq.values()), color='lightblue')
    ax2.set_title('Unordered Heap')
    ax2.set_xlabel('Accesses')
    ax2.set_ylabel('Frequency')

    # Adjust layout to prevent overlap
    plt.tight_layout()
    plt.suptitle("Fanout = "+fo)
    # Show the plot
    plt.show()
    # break