import os
import numpy as np
import matplotlib.pyplot as plt
import pickle

import fmcw_data_processor as fmcw

source_path = r"D:\BP_RF_RGB_CAM"
rf_files = os.listdir(source_path)

# path = "D:\syndicate_tests_velocity_detection_1\FMCW_Radar\FMCW_Radar.raw"
radar_config={"chirps": 1, "tx": 3, "rx": 4, "samples" : 256}
# radar_data = []

for i in rf_files:
    if(i == "test8"):
        print(f"Sample {i} processing")
        # try:
        path = os.path.join(source_path, i, "FMCW_Radar", "FMCW_radar.raw")
        data = fmcw.Fmcw_Data_Processor(path, radar_config=radar_config)
        print(data.range_bins.shape)
        # radar_data.append((i, data))
        with open(os.path.join(source_path, i, 'radar_bins.pickle'), 'wb') as handle:
            pickle.dump(data.range_bins, handle, protocol=pickle.HIGHEST_PROTOCOL)
        # except:
        #     print(f"Sample {i} failed. Does not exist.")