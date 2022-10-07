import numpy as np
from scipy.io import wavfile
test_str = "D:/syndicate_tests_preetham_mic/Microphone/Microphone.raw"
wav_path = "D:/syndicate_tests_preetham_mic/Microphone/Test_Out.wav"
byte_string = open(test_str, "rb").read()
#byte_string = open("D:/syndicate_tests/Microphone/Microphone.raw", "rb").read()
decoded_data = np.frombuffer(byte_string, dtype=np.float32).reshape(-1, 8)
print(decoded_data.shape, decoded_data.max(), decoded_data.min())
wavfile.write(wav_path, 44100, decoded_data)
#wavfile.write("D:/syndicate_tests/Microphone/Out.wav", 44100, decoded_data)hon3