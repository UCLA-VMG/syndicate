import numpy as np
import os
import scipy
from scipy import interpolate
import matplotlib.pyplot as plt
import json
from six import string_types

import sys
import os
import time
import csv
from datetime import datetime
import pickle

def extract_timestamps(filename):
    file = open(filename, 'r')
    data = file.readlines()

    ts = []
    for i in data:
        time_obj = datetime.strptime(i.rstrip(), '%Y%m%d_%H_%M_%S_%f')
        ts.append(time_obj.timestamp())
    
    return np.array(ts)
      
def find_deltas(sys_stamps, mx_stamps):
    const_num = sys_stamps[0]
    deltas = []
    for i, stamp in enumerate(sys_stamps):
        if(stamp != const_num):
            const_num = stamp
            delta = sys_stamps[i-1] - mx_stamps[i-1]
            if(sys_stamps[i] - sys_stamps[i-1] > 0.3): # comparison for blips
                deltas.append(delta)
    return np.array(deltas)

def find_deltas2(sys_stamps, mx_stamps):
    sys_stamps = sys_stamps[0:200*32]
    mx_stamps  = mx_stamps[0:200*32]

    const_num = sys_stamps[0]
    deltas = []
    for i, stamp in enumerate(sys_stamps):
        if(stamp != const_num):
            const_num = stamp
            delta = sys_stamps[i-1] - mx_stamps[i-1]
            if(sys_stamps[i] - sys_stamps[i-1] > 0.3): # comparison for blips
                deltas.append(delta)
    return np.array(deltas)

def unroll_stamps(mx_stamps, batch_size = int(32), time_diff = 0.256):

    unrolled_stamps = []

    for i in range(int(len(mx_stamps)/batch_size)):
        current_stamp = mx_stamps[i * batch_size]
        for j in range(batch_size):
            unrolled_val = current_stamp - time_diff + time_diff*(j+1)/batch_size
            unrolled_stamps.append(unrolled_val)
    
    return np.array(unrolled_stamps)

def unroll_stamps2(mx_stamps, batch_size = int(32), time_diff = 0.256):

    unrolled_stamps = []

    current_stamp = mx_stamps[0] - time_diff
    for i in range(int(len(mx_stamps)/batch_size)):
        current_stamp += time_diff
        for j in range(batch_size):
            unrolled_val = current_stamp - time_diff + time_diff*(j+1)/batch_size
            unrolled_stamps.append(unrolled_val)
    
    return np.array(unrolled_stamps)

def apply_delta(mx_stamps, sys_mx_time_delta):
    return mx_stamps + sys_mx_time_delta

def timestamp_process(ts):
        f = ((float(ts)/1e6)-int(float(ts)/1e6))*1e6

        ts = int(float(ts)/1e6)
        s = ((float(ts)/1e2)-int(float(ts)/1e2))*1e2
        ts = int(float(ts)/1e2)
        m = ((float(ts)/1e2)-int(float(ts)/1e2))*1e2
        ts = int(float(ts)/1e2)
        h = ((float(ts)/1e2)-int(float(ts)/1e2))*1e2


        temp = (3600*h)+(60*m)+s+(f*1e-6)
        temp = float(int(temp*1e6))/1e6

        return temp

def interpolate_timestamp(vital_sign_dictionary, vid_ts, offset = 25/30):
        #constucting arrays for the data
        vital_interpolated_dict = {}
        for key in vital_sign_dictionary.keys():
            mx_stamps, sys_stamps, data, unroll_flag = vital_sign_dictionary[key]

            delta_array = find_deltas(sys_stamps, mx_stamps)
            sys_mx_time_delta = np.mean(delta_array)
            if(unroll_flag):
                mx_unrolled = unroll_stamps(mx_stamps)
            else:
                mx_unrolled = mx_stamps
            health_ts = apply_delta(mx_unrolled, sys_mx_time_delta)
            health_ts = health_ts - offset

            ##CHECK FOR Data AND TS LENGTHS AND CORRECT
            l1 = len(health_ts)
            l2 = len(data)
            if l1<l2:
                raise Exception("Unequal MX800 Data and Timestamp Lengths!")
            elif l2<l1:
                raise Exception("Unequal MX800 Data and Timestamp Lengths!")
            #interpolation function
            f = interpolate.interp1d(health_ts,data,kind='linear')
            reinterp_data = []
            for t_temp in vid_ts:
                if t_temp<health_ts[0]: #If timestamp is before start of MX800 fill with starting values
                    reinterp_data.append(data[0])
                elif t_temp>health_ts[-1]: #If timestamp is after end of MX800 fill with ending values
                    reinterp_data.append(data[-1])
                else:
                    reinterp_data.append(f(t_temp))
            output = np.array(reinterp_data)
            vital_interpolated_dict[key] = output

            if(unroll_flag):
                plt.figure()
                plt.plot(output)
        
        # plt.show()

        return vital_interpolated_dict

def aslist_cronly(value):
    if isinstance(value, string_types):
        value = filter(None, [x.strip() for x in value.splitlines()])
    return list(value)

def aslist(value, flatten=True):
    """ Return a list of strings, separating the input based on newlines
    and, if flatten=True (the default), also split on spaces within
    each line."""
    values = aslist_cronly(value)
    if not flatten:
        return values
    result = []
    for value in values:
        subvalues = value.split()
        result.extend(subvalues)
    return result

def get_interpolated_vital_dict(vital_dict_path, timestamp_path, save_folder_path, offset = 25/30):
    
    fileObj = open(os.path.join(vital_dict_path), 'rb')
    vital_dict = pickle.load(fileObj)
    fileObj.close()

    vid_ts = extract_timestamps(timestamp_path)

    interpolated_vital_dict = interpolate_timestamp(vital_dict, vid_ts, offset)
    if(save_folder_path is not None):
        save_path = os.path.join(save_folder_path, "vital_interpolated_dictionary.pkl")
        filehandler = open(save_path, 'wb') 
        pickle.dump(interpolated_vital_dict, filehandler)
        filehandler.close()



if __name__ == "__main__":

    root_path = r"D:\BP_RF_RGB_CAM"
    recording_paths = os.listdir(root_path)
    for i in recording_paths:
        print(i)
        save_folder_path = os.path.join("D:\syndicate_tests", i, "MX800")
        video_ts_file_path = os.path.join("D:\syndicate_tests", i, "NIR_Camera", "log_timestamps.txt")
        vital_dict_file_path = os.path.join(save_folder_path, "vital_original_dictionary.pkl")
        get_interpolated_vital_dict(vital_dict_file_path, video_ts_file_path, save_folder_path)


    plt.show()
