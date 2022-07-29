import imageio as io
import numpy as np 
import os 

def get_video(folder_path : str, file_type : str, dtype=np.uint8) -> np.array:
    #TODO someone should make this more general, e.g. various video formats

    if(file_type in [".png", ".bmp"]):
        image_files = [f for f in os.listdir(folder_path) if file_type in f]
        num_frames = len(image_files)
        pos_last = image_files[0].rfind('_')
        base_string = image_files[0][0:pos_last+1]
        video = []
        for i in range(num_frames):
            image_path = os.path.join(folder_path, base_string+str(i)+file_type)
            video.append(io.imread(image_path))
        return np.array(video, dtype=dtype)
    else:
        raise ValueError 