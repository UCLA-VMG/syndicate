from __future__ import print_function
import cv2 as cv
import argparse
from email.mime import image
from json import tool
from numbers import Rational
from pickletools import uint8
from threading import currentThread
import cv2 as cv
import numpy as np

from facenet_pytorch import MTCNN
import os
import matplotlib.pyplot as plt
import torch
import colour
import cv2 as cv
from colour_demosaicing import (
    EXAMPLES_RESOURCES_DIRECTORY,
    demosaicing_CFA_Bayer_bilinear,
    demosaicing_CFA_Bayer_Malvar2004,
    demosaicing_CFA_Bayer_Menon2007,
    mosaicing_CFA_Bayer)

from tqdm import tqdm

from utils import get_video

def bin_image(img, size=2):
    bin_imgs = []
    for i in range(size):
        for j in range(size):
            bin_imgs.append(img[i::size,j::size])
    binned_img = np.mean(np.array(bin_imgs), axis = 0)
    return binned_img

def demosaic_bin_image(img, size = 2):
    binned_img = bin_image(img.astype(np.float32), size=size)
    img_demosaic = cv.cvtColor(binned_img.astype(np.uint8), cv.COLOR_BAYER_RG2RGB)
    return img_demosaic

def demosaic_bin_video(vid, size=2):
    img_arr = []
    for img in tqdm(vid, total=len(vid)):
        img_arr.append(demosaic_bin_image(img))
    return np.array(img_arr)

class Single_Face_Cropper:
    def __init__(self):
        device = 'cuda' if torch.cuda.is_available() else 'cpu'
        self.mtcnn = MTCNN(device=device)
    

    def crop_image_array(self, image, ratio=-1):
        # Take:
        #     1 or 3 channel image
        #     optional: 
        #         expected output dimensions ratio (width/height)
        
        # output:
        #     cropped image, cropped dimensions, center
        # print(image.shape)
        try:
            image = np.array(image)
        except:
            raise Exception("image cannot be converted to array")

        og_dims = image.shape

        if len(image.shape) == 2:
            image = np.repeat(image[:, :, np.newaxis], 3, axis=2)
        elif image.shape[2] == 1:
            image = image.repeat(image, 3, axis=2)
        elif image.shape[2] != 3:
            raise Exception("Image must have 1 or 3 channels")

        # plt.figure()
        # plt.imshow(image)
        # plt.show()
        # print(np.max(image), np.min(image), image.dtype)
        face = self.mtcnn.detect(image)

        try:
            print("MTCNN used.")
            center = {
                "x": int((face[0][0][0]+face[0][0][2])/2),
                "y": int((face[0][0][1]+face[0][0][3])/2)
                }
            
            box = {
                    "x_left": int(min(face[0][0][0], face[0][0][2])),
                    "x_right": int(max(face[0][0][0], face[0][0][2])),
                    "y_top": int(min(face[0][0][1], face[0][0][3])),
                    "y_bottom": int(max(face[0][0][1], face[0][0][3]))
                }
        except:
            face_cascade = cv.CascadeClassifier()
            #-- 1. Load the cascades
            if not face_cascade.load(cv.samples.findFile(r'C:\Users\111\Desktop\repos\syndicate\PostSyndicate\camera\haarcascade_frontalface_alt.xml')):
                print('--(!)Error loading face cascade')
                exit(0)
            print("HAAR used. ")
            frame_gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
            frame_gray = cv.equalizeHist(frame_gray)
            #-- Detect faces
            faces = face_cascade.detectMultiScale(frame_gray)
            x,y,w,h = faces[0]
            center = (x + w//2, y + h//2)

            center = {
                "x": center[0],
                "y": center[1]
            }

            box = {
                "x_left": x,
                "x_right": x+w,
                "y_top": y,
                "y_bottom": y+h
            }

        if ratio > 0:
            width = (box["x_right"] - box["x_left"])
            height = (box["y_bottom"] - box["y_top"])

            if width/height > ratio:
                height = width/ratio
            elif width/height < ratio:
                width = height*ratio

            box = {
                "x_left": max(0, int(center['x']-width/2)),
                "x_right": min(og_dims[1], int(center['x']+width/2)),
                "y_top": max(0, int(center['y']-height/2)),
                "y_bottom": min(og_dims[0], int(center['y']+height/2))
                }
        image = image[ box['y_top']:box['y_bottom'], box['x_left']:box['x_right'] ]

        return image, box, center



    def crop_video(self, video_input, destination="", evaluation_freq=-1, height=128, width=128, format=""):
        # Inputs:
        #     source: a path to a video
        #     destination: path to send the video too
        #     evaluation_freq: How often the the program runs MTCNN. if -1 only runs on first frame
        #     hieght/width: dimensions of output video
        # Returns:
        #     cropped video as an array
        
        # cap = cv.VideoCapture(source)
        # frame_num = int(cap.get(cv.CAP_PROP_FRAME_COUNT))

        # ret, frame = cap.read()

        frame = video_input[600] #TODO
        frame_num = video_input.shape[0]

        if(len(frame.shape) == 3):
            channels=frame.shape[2]
        else:
            channels = 0

        if channels == 0:
            video = np.zeros((frame_num, height, width), dtype=np.float32)
        else:
            video = np.zeros((frame_num, height, width, channels), dtype=np.float32)

        _, box, _ = self.crop_image_array(frame, ratio=width/height)
        cropped = frame[box["y_top"]:box["y_bottom"], box["x_left"]:box["x_right"]]
        video[0] = cv.resize(cropped, (width, height))

        since_last_mtcnn = 1

        for i in range(1, frame_num):
            if(since_last_mtcnn == evaluation_freq):
                _, box, _ = self.crop_image_array(frame, ratio=width/height)
                since_last_mtcnn = 0
            since_last_mtcnn += 1
            frame = video_input[i]

            cropped = frame[box["y_top"]:box["y_bottom"], box["x_left"]:box["x_right"]]
            video[i] = cv.resize(cropped, (width, height))
        
        if destination != "":
            num=0
            for frame in video:
                cv.imwrite(destination+"/img_"+str(num)+".png", frame.astype('uint8'))
                num+= 1

        return video



def detectAndDisplay(frame):
    frame_gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    frame_gray = cv.equalizeHist(frame_gray)
    #-- Detect faces
    faces = face_cascade.detectMultiScale(frame_gray)
    for (x,y,w,h) in faces:
        center = (x + w//2, y + h//2)
        frame = cv.ellipse(frame, center, (w//2, h//2), 0, 0, 360, (255, 0, 255), 4)
        faceROI = frame_gray[y:y+h,x:x+w]

    cv.imshow('Capture - Face detection', frame)

if __name__ == "__main__":
    det = Single_Face_Cropper()
    source = r'D:\BP_RF_RGB_CAM'

    # min_id = 1
    # max_id = 10
    # prefix_id = 'r'
    file_list = os.listdir(source)

    for i in file_list:
        print(i)
        try:
            source0 = os.path.join(source, i, "RGB_Polarized_Camera")
            if("img_0.png" not in os.listdir(source0)):
                video_input0 = get_video(source0, file_type=".bmp")
                video_input_binned = demosaic_bin_video(video_input0)
                det.crop_video(video_input=video_input_binned, destination=source0, evaluation_freq=-1)
        except:
            print("MTCNN Failed Detecting Face.")
