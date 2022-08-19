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

from PostSyndicate.mx800.mx800_config import mx800_MPDataExport

def extract_data(input_filepath, vital_sign):
    file = open(input_filepath, 'r', encoding='utf-8-sig')
    csvreader = csv.reader(file)
    stamp_list = []
    col_idx = 3
    
    path = os.path.dirname(input_filepath)
    filename_ext = os.path.basename(input_filepath)
    filename = os.path.splitext(filename_ext)[0]

    if(filename == "MPDataExport"):
        # This skips the first row of the CSV file.
        next(csvreader)
        col_idx = 2+vital_sign
    
    for i in csvreader:
        stamp_list.append(i)

    mx_stamps = []
    sys_stamps = []
    data = []

    for j in range(len(stamp_list)):
        time_obj_mx = datetime.strptime(stamp_list[j][0], '%d-%m-%Y %H:%M:%S.%f')
        stamp_mx = time_obj_mx.timestamp()
        mx_stamps.append(stamp_mx)

        time_obj_sys = datetime.strptime(stamp_list[j][2], '%d-%m-%Y %H:%M:%S.%f')
        stamp_sys = time_obj_sys.timestamp()
        sys_stamps.append(stamp_sys)

        try:
            data.append(float(stamp_list[j][col_idx]))
        except:
            data.append(-1)
    #Code for interpolating unknown values that were zeroed 
    zero_flag = 0
    start_flag = 0
    start, start_idx, finish, finish_idx = 0,0,0,0
    for i in range(len(data)):
        if(data[i] == -1 and zero_flag==0):
            start = data[i-1]
            start_idx = i 
            zero_flag = 1
        if(data[i] != -1 and zero_flag==1):
            delta_step = (data[i]-start)/(i+1-start_idx)
            for j in range(start_idx, i):
                data[j] = start + delta_step*(j+1-start_idx)
            zero_flag = 0
        if(data[i] == -1 and zero_flag==0 and i == 0):
            start_flag = 1
        if(data[i] != -1 and zero_flag==1 and start_flag):
            for j in range(0, i):
                data[j] = data[i]
            start_flag = 0
    if(zero_flag == 1):
        for j in range(start_idx, len(data)):
            data[j] = start

        

    return mx_stamps, sys_stamps, data
  
def generate_original_vital_dict(folder_path, save_folder=None, visualize=False):

    vital_dict = {}
    unroll_flag = 1
    source1 = os.path.join(folder_path, "NOM_PLETHWaveExport.csv")
    mx_stamps, sys_stamps, data = extract_data(input_filepath=source1, vital_sign=1)
    vital_dict["PPG"] = [mx_stamps, sys_stamps, data, unroll_flag]
    
    unroll_flag = 0
    source2 =  os.path.join(folder_path, "MPDataExport.csv")
    for i in range(len(mx800_MPDataExport)):
        mx_stamps, sys_stamps, data = extract_data(input_filepath=source2, vital_sign=i+1)
        vital_dict[mx800_MPDataExport[i]["name"]] = [mx_stamps, sys_stamps, data, unroll_flag]

    if(save_folder is not None):
        save_path = os.path.join(save_folder, "vital_original_dictionary.pkl")
        filehandler = open(save_path, 'wb') 
        pickle.dump(vital_dict, filehandler)
        filehandler.close()
    
    if(visualize):
        for key in vital_dict:
            _, _, data, _ = vital_dict[key]
            plt.figure()
            plt.plot(data)
            plt.title(key)
        plt.show()
    
    return vital_dict


if __name__ == "__main__":

    min_id = 1
    max_id = 8
    prefix_id = 't'
    for i in range(min_id, max_id+1):
        print(i)
        folder_path = os.path.join("D:\syndicate_tests", prefix_id+str(i), "MX800")
        generate_original_vital_dict(folder_path=folder_path, save_folder=folder_path, visualize=False)


    ### Uncomment to test extraction->
    # fileObj = open(os.path.join(folder_path,  "vital_original_dictionary.pkl"), 'rb')
    # exampleObj = pickle.load(fileObj)
    # fileObj.close()
    # print(exampleObj)