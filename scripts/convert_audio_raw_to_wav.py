import numpy as np
from scipy.io import wavfile

byte_string = open("D:/syndicate_tests/Microphone/Microphone.raw", "rb").read()
decoded_data = np.frombuffer(byte_string, dtype=np.float32).reshape(-1, 8)
print(decoded_data.shape, decoded_data.max(), decoded_data.min())
wavfile.write("D:/syndicate_tests/Microphone/Out.wav", 44100, decoded_data)