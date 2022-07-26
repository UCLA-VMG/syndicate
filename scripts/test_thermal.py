# # #capture.py
# # import cv2
# # device_index = 1

# # # for i in range(-2,10,1):

# # device_index = 0
# # cap = cv2.VideoCapture(device_index)
# # print(cap)
# # cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 512)
# # cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
# # cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter.fourcc('Y','1','6',' '))
# # cap.set(cv2.CAP_PROP_CONVERT_RGB, 0)

# # print(device_index)
# # print()
# # framecount=50
# # frame_buf=[]
# # for _ in range(framecount): #record indefinitely (until user presses q), replace with "while True"
# #     stream_ret, frame = cap.read()
# #     if stream_ret:
# #         cv2.imshow("image", frame)
# #         if cv2.waitKey(33) == ord('q'):
# #             break;
# #         frame_buf.append(frame)
        
# # cv2.destroyAllWindows()

# #################### IMAGE IO ###################################################

# import imageio as iio 
# import matplotlib.pyplot as plt
# import numpy as np 

# import math

# def from_string_or_float(value):
#     if type(value) is float:
#         return value
#     else:
#         return float(value.strip().split(" ")[0])

# def raw2temp(raw, meta=None):
#     """
#     Convert raw pixel values to temperature, if calibration coefficients are known. The
#     equations for atmospheric and window transmission are found in Minkina and Dudzik, as 
#     well as some of FLIR's documentation.
#     Roughly ported from ThermImage: https://github.com/gtatters/Thermimage/blob/master/R/raw2temp.R
#     """

#     ATA1 = 0.006569
#     ATA2 = 0.01262
#     ATB1 = -0.002276
#     ATB2 = -0.00667
#     ATX = 1.9
#     PR1 = 21106.77
#     PR2 = 0.012545258
#     PO = -7340
#     PB = 1501
#     PF = 1
#     E = 1
#     IRT = 1
#     IRWTemp = 20
#     OD = 0.5
#     ATemp = 20
#     RTemp = 20
#     humidity = 0.5

#     # Equations to convert to temperature
#     # See http://130.15.24.88/exiftool/forum/index.php/topic,4898.60.html
#     # Standard equation: temperature<-PB/log(PR1/(PR2*(raw+PO))+PF)-273.15
#     # Other source of information: Minkina and Dudzik's Infrared Thermography: Errors and Uncertainties

#     window_emissivity = 1 - IRT
#     window_reflectivity = 0

#     # Converts relative humidity into water vapour pressure (mmHg)
#     water = (humidity/100.0)*math.exp(1.5587+0.06939*(ATemp)-0.00027816*(ATemp)**2+0.00000068455*(ATemp)**3)

#     #tau1 = ATX*np.exp(-np.sqrt(OD/2))
#     tau1 = ATX*np.exp(-np.sqrt(OD/2)*(ATA1+ATB1*np.sqrt(water)))+(1-ATX)*np.exp(-np.sqrt(OD/2)*(ATA2+ATB2*np.sqrt(water)))
#     tau2 = tau1

#     # transmission through atmosphere - equations from Minkina and Dudzik's Infrared Thermography Book
#     # Note: for this script, we assume the thermal window is at the mid-point (OD/2) between the source
#     # and the camera sensor

#     raw_refl = PR1/(PR2*(np.exp(PB/(RTemp+273.15))-PF))-PO   # radiance reflecting off the object before the window
#     raw_refl_attn = (1-E)/E*raw_refl   # attn = the attenuated radiance (in raw units) 

#     raw_atm1 = PR1/(PR2*(np.exp(PB/(ATemp+273.15))-PF))-PO # radiance from the atmosphere (before the window)
#     raw_atm1_attn = (1-tau1)/E/tau1*raw_atm1 # attn = the attenuated radiance (in raw units) 

#     raw_window = PR1/(PR2*(np.exp(PB/(IRWTemp+273.15))-PF))-PO
#     einv = 1./E/tau1/IRT
#     raw_window_attn = window_emissivity*einv*raw_window

#     raw_refl2 = raw_refl   
#     raw_refl2_attn = window_reflectivity*einv*raw_refl2

#     raw_atm2 = raw_atm1
#     ediv = einv/tau2
#     raw_atm2_attn = (1-tau2)*ediv*raw_atm2

#     # These last steps are pretty slow and 
#     # could probably be sped up a lot
#     raw_sub = -raw_atm1_attn-raw_atm2_attn-raw_window_attn-raw_refl_attn-raw_refl2_attn
#     raw_object = np.add(np.multiply(raw, ediv), raw_sub)

#     raw_object = np.add(raw_object, PO)
#     raw_object = np.multiply(raw_object, PR2)
#     raw_object_inv = np.multiply(np.reciprocal(raw_object), PR1)
#     raw_object_inv = np.add(raw_object_inv, PF)    
#     raw_object_log = np.log(raw_object_inv)
#     temp = np.multiply(np.reciprocal(raw_object_log), PB)

#     return temp - 273.15

# reader = iio.get_reader('<video0>', format = "FFMPEG", dtype = "uint16", fps = 30)

# #test self.reader ambiguity
# test_img = reader.get_data(0)
# print(test_img.dtype)
# # Thermal sensor test image is changing between trials.
# width   = int(test_img.shape[1]/2)
# height  = int(test_img.shape[0]/2)
# print(test_img.shape)
# print(np.max(test_img), np.min(test_img), np.std(test_img))
# print(np.mean(test_img)*0.04 -  273.15)
# print(np.mean(test_img, axis=(0,1)))
# plt.imshow(test_img/2**16)
# plt.figure()
# plt.imshow(test_img[:,:,0]/2**16)
# plt.figure()
# plt.imshow(test_img[:,:,1]/2**16)
# plt.figure()
# plt.imshow(test_img[:,:,2]/2**16)
# plt.show()

# print(test_img[:,:,0])

# print(np.mean(test_img))
# reader.close()
# # print(test_img[:,:,1])
# # print(test_img[:,:,2])

# # print(raw2temp(test_img))

# ######################## FLIRPY ################################################
# # from flirpy.camera.boson import Boson

# # cam = Boson()
# # image = cam.grab()
# # print(image)
# # print(cam.frame_count)
# # print(cam.frame_mean)
# # print(cam.ffc_temp_k)
# # print(cam.fpa_temp_k)
# # cam.close()


import cv2
import numpy as np
import matplotlib.pyplot as plt

# open video0
# print(cv2.CAP_MSMF)
cap = cv2.VideoCapture(0)
print(cap)
print(cap.get(cv2.CAP_PROP_FOURCC))
# out = cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter.fourcc('Y','1','6',' ')) 
print(cap.get(cv2.CAP_PROP_FOURCC))
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 256)
cap.set(cv2.CAP_PROP_CONVERT_RGB, 0) # turn off RGB conversion
# cap.set(cv2.CAP_PROP_CONVERT_RGB, 0)
# cap.set(cv2.CAP_PROP_FORMAT, -1)
if cap.isOpened():
    #for i in range(10):
    # Capture frame-by-frame
        ret, frame = cap.read()
else: 
    #if not ret:
    #break
        ret = False

print()
print(ret)
if ret:    
    print('frame.shape = {}    frame.dtype = {}'.format(frame.shape, frame.dtype))
    plt.imshow(frame)
    plt.show()
cap.release()