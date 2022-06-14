import numpy as np
import matplotlib.pyplot as plt

height = 480
width = 640

byte_string = open("D:\syndicate_tests_pc_32\Coded_Light_Depth_Camera/Coded_Light_Depth_Camera_RGB_0.raw", "rb").read()
decoded_data = np.frombuffer(byte_string, dtype=np.uint8).reshape(1080, 1920, 3)
plt.imshow(decoded_data)
plt.show()


byte_string = open("D:\syndicate_tests_pc_32\Coded_Light_Depth_Camera/Coded_Light_Depth_Camera_Depth_0.raw", "rb").read()
X = np.arange(width)
Y = np.arange(height)
X, Y = np.meshgrid(X, Y)
decoded_data = np.frombuffer(byte_string, dtype=np.uint16).reshape(height, width)
decoded_data = np.array(decoded_data)
decoded_data = decoded_data
decoded_data[decoded_data == 0] = np.max(decoded_data[decoded_data != 0])
plt.imshow(decoded_data)
plt.show()

# Plot the surface.
from matplotlib import cm
from matplotlib.ticker import LinearLocator
fig, ax = plt.subplots(subplot_kw={"projection": "3d"})
surf = ax.plot_surface(X, Y, -decoded_data, cmap=cm.coolwarm,
                       linewidth=0, antialiased=False)

# Customize the z axis.
# Add a color bar which maps values to colors.
fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()