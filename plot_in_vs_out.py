
import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

samplerate_in, data_in = wavfile.read('in.wav')
samplerate_out, data_out = wavfile.read('out.wav')


red_patch = mpatches.Patch(color='red', label='Input file')
blue_patch = mpatches.Patch(color='blue', label='Output file')
fig, axs = plt.subplots(2)

axs[0].plot(data_in, 'r--')
axs[0].set_ylabel("amplitude")
axs[1].plot( data_out, 'b-')
fig.suptitle("Input and output (cancelled silences)")
plt.xlabel("time (s)")
plt.ylabel("amplitude")
plt.legend(handles=[red_patch, blue_patch])
plt.show()