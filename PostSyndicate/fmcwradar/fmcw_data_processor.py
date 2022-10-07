from PostSyndicate.fmcwradar.pcapreader import PcapReader
import numpy as np 
import struct
import matplotlib.pyplot as plt
import PostSyndicate.fmcwradar.packet_organizer as po

class Fmcw_Data_Processor:

    def __init__(self, path, radar_config={"chirps": 128, "tx": 3, "rx": 4, "samples" : 256}, angle_granularity=128):
        orgi = po.Organizer(path, radar_config=radar_config)
        self.data = orgi.organize()

        self.range_bins = np.fft.fft(self.data, axis=3)
        self.range_doppler = np.fft.fft(self.range_bins, axis=1)

        angle_granularity = 128
        padding = ((0,0), (0,0), (0,angle_granularity-self.range_bins.shape[2]), (0,0))
        self.theta_bins=np.fft.fft( np.pad(self.range_doppler, padding), axis=2)

    def get_data(self):
        return self.data

    def get_range_doppler_array(self):
        return self.range_doppler

    def get_angle_array(self):
        return self.theta_bins
    
    def get_range_resolution(self, bandwidth=3600.72):
        bandwidth *= 1e6
        range_res = 3e8 / (2 * bandwidth)
        range_bin_vals = np.arange(self.data.shape[3])*range_res
        return range_bin_vals
    
    def get_velocity_resolution(self, params={}):
        defaults={
        'sample_rate': 5000, # Rate at which the radar samples from ADC (ksps - kilosamples per second)
        'freq_slope': 60.012, # Frequency slope of the chirp (MHz/us)
        'adc_samples': 256, # Number of samples from a single chirp
        'start_freq': 77, # Starting frequency of the chirp (GHz)
        'idle_time': 100, # Time before starting next chirp (us)
        'ramp_end_time': 60, # Time after sending each chirp (us)
        'num_chirps': 128, # Number of chirps per frame
        'num_tx': 3} # Number of transmitters})
        for key in defaults.keys():
            if key not in params:
                params[key] = defaults[key]
        
        velocity_res = 3e8 / (2 * params['start_freq'] * 1e9 * (params['idle_time'] + params['ramp_end_time']) * 1e-6 * params['num_chirps'] * params['num_tx'])
        velocities = np.arange(params['num_chirps']) - (params['num_chirps'] // 2)
        velocities = velocities * velocity_res
        return velocities

    def get_angle_resolution(self):
        omega = np.fft.fftfreq(self.theta_bins.shape[2])*2
        angle_bin_vals = np.arcsin(omega)
        return angle_bin_vals

    
    def generate_image_angle_range(self, frame=1, convert=True):
        angles = self.get_angle_resolution()
        ranges = self.get_range_resolution()

        if(convert):
            polar_grid = np.log((np.abs(self.theta_bins))[frame].sum(0).T)
            # print(angles*180/np.pi)
            fig = plt.figure(figsize=[5,5])
            ax = fig.add_axes([0.1,0.1,0.8,0.8],polar=True)
            ax.set_xlim(-3.14159/2, 3.14159/2)
            ax.pcolormesh(angles,ranges,polar_grid,edgecolors='face')
            
            #ec='face' to avoid annoying gridding in pdf
            # plt.savefig('polar.png')
            # plt.imshow(, extent=[angles.min(), angles.max(), ranges.max(), ranges.min()])
            # plt.xlabel('Angle (Rad)')
            # plt.ylabel('Range (meters)')
            plt.title('Range and Angle')
            plt.show()
        else: 
            plt.imshow(np.log(np.fft.fftshift(np.abs(self.theta_bins), axes=2)[frame].sum(0).T))
            plt.xlabel('Angle Bins')
            plt.ylabel('Range Bins')
            plt.title('Range and Angle')
            plt.show()

    def generate_image_velocity_range(self, frame=1, convert=True, params={}):
        ranges = self.get_range_resolution()
        velocities = self.get_velocity_resolution(params)
        if(convert):
            plt.imshow((np.fft.fftshift(np.abs(self.range_doppler[frame,::,::,::].sum(1)), axes=0).T), extent=[velocities.min(), velocities.max(), ranges.max(), ranges.min()])
            plt.xlabel('Velocity (m/s)')
            plt.ylabel('Range (m)')
            plt.title('Range and Velocity')
            plt.show()
        else: 
            plt.imshow((np.fft.fftshift(np.abs(self.range_doppler[frame,::,::,::].sum(1)), axes=0).T))
            plt.xlabel('Velocity Bins')
            plt.ylabel('Range Bins')
            plt.title('Range and Velocity')
            plt.show()

    def generate_image_angle_velocity(self, frame=1, convert=True, params={}):
        angles = self.get_angle_resolution()
        velocities = self.get_velocity_resolution(params)
        if(convert):
            plt.imshow((np.fft.fftshift(np.fft.fftshift(np.abs(self.range_doppler[frame,::,::,::].sum(2)), axes=0), axes=1)), extent=[velocities.min(), velocities.max(), angles.max(), angles.min()])
            plt.xlabel('Angle (Rad)')
            plt.ylabel('Velocity (m/s)')
            plt.title('Angle and Velocity')
            plt.show()
        else: 
            plt.imshow((np.fft.fftshift(np.fft.fftshift(np.abs(self.range_doppler[frame,::,::,::].sum(2)), axes=0), axes=1)))
            plt.xlabel('Angle Bins')
            plt.ylabel('Velocity Bins')
            plt.title('Angle and Velocity')
            plt.show()
