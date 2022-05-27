import numpy as np
from scipy.io import wavfile

byte_string = open("recorded.raw", "rb").read()
decoded_data = np.frombuffer(byte_string, dtype=np.float32).reshape(-1, 8)
wavfile.write("out.wav", 44100, decoded_data)