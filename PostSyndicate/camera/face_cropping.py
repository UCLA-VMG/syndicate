from email.mime import image
from json import tool
from numbers import Rational
from threading import currentThread
import cv2 as cv
import numpy as np

from facenet_pytorch import MTCNN
import os
import matplotlib.pyplot as plt
import torch

from PostSyndicate.camera.utils import get_video

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

        frame = video_input[0]
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



if __name__ == "__main__":
    det = Single_Face_Cropper()
    source = r'D:\BP_RF_CAM'

    min_id = 1
    max_id = 20
    prefix_id = ''

    for i in range(min_id, max_id+1):
        print(i)

        source0 = os.path.join(source, prefix_id+str(i), "Polarized_Camera")
        video_input0 = get_video(source0, file_type=".bmp")
        det.crop_video(video_input=video_input0, destination=source0, evaluation_freq=-1)

        # source1 = os.path.join("D:\syndicate_tests", prefix_id+str(i), "NIR_Camera")
        # video_input1 = get_video(source1, file_type=".bmp")
        # det.crop_video(video_input=video_input1, destination=source1, evaluation_freq=-1)

        # source2 = os.path.join("D:\syndicate_tests", prefix_id+str(i), "NIR_Vimba_Camera")
        # video_input2 = get_video(source2, file_type=".bmp")
        # det.crop_video(video_input=video_input2, destination=source2, evaluation_freq=-1)
        

