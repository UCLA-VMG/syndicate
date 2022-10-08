#include "RealSenseCamera.h"

using namespace cv;

RealSenseCamera::RealSenseCamera(ptree::value_type& sensor_settings, ptree::value_type& global_settings)
    : Syndicate::Camera(sensor_settings, global_settings), cameraID(sensor_settings.second.get<int>("Camera ID"))
{
    std::cout << "Booting the Real Sense Camera";
    rs_pipe.start();
}

RealSenseCamera::~RealSenseCamera()
{
    // Deinitialize camera
    // the camera will be deinitialized automatically in VideoCapture destructor
}

void RealSenseCamera::AcquireSave(double seconds, boost::barrier& startBarrier)
{
    int num_frames = static_cast<int>(seconds*fps);
    startBarrier.wait();
    auto start = std::chrono::steady_clock::now();
    for(int i = 0 ; i < num_frames ; i++) {
        auto frames = rs_pipe.wait_for_frames();
        RecordTimeStamp();
        // auto color = frames.get_color_frame();
        // if (!color)
        //     color = frames.get_infrared_frame();
        auto depth = frames.get_depth_frame();
        // auto depth = color_map.process(frames.get_depth_frame());
        auto image = depth.as<rs2::video_frame>();
        if (image) {
            std::ofstream outfile(rootPath + sensorName + "_Depth_" + std::to_string(i) + ".raw", std::ofstream::binary);
            outfile.write(static_cast<const char*>(image.get_data()), image.get_height()*image.get_width()*image.get_bytes_per_pixel());
            outfile.close();
            logFile << "Depth Frame" << image.get_frame_number();
            std::stringstream csv_file_depth;
            csv_file_depth << rootPath  << "Depth-" << i << "-metadata.csv";
            metadata_to_csv(image, csv_file_depth.str());
        }
        auto color = frames.get_color_frame();
        auto color_image = color.as<rs2::video_frame>();
        if (color_image) {
            std::ofstream outfile(rootPath + sensorName + "_RGB_" + std::to_string(i) + ".raw", std::ofstream::binary);
            outfile.write(static_cast<const char*>(color_image.get_data()), color_image.get_height()*color_image.get_width()*color_image.get_bytes_per_pixel());
            outfile.close();
            logFile << "RGB Frame" << image.get_frame_number();
            std::stringstream csv_file_rgb;
            csv_file_rgb << rootPath  << "RGB-" << i << "-metadata.csv";
            metadata_to_csv(color_image, csv_file_rgb.str());
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::cout <<"Time Taken for acquistion " << sensorName << " " << static_cast<double>((end-start).count())/1'000'000'000 << "\n";
    logFile << "Time Taken for acquistion " << sensorName << " " << static_cast<double>((end-start).count())/1'000'000'000 << "\n";
    SaveTimeStamps();
}

void metadata_to_csv(const rs2::frame& frm, const std::string& filename)
{
    std::ofstream csv;

    csv.open(filename);

    //    std::cout << "Writing metadata to " << filename << endl;
    csv << "Stream," << rs2_stream_to_string(frm.get_profile().stream_type()) << "\nMetadata Attribute,Value\n";

    // Record all the available metadata attributes
    for (size_t i = 0; i < RS2_FRAME_METADATA_COUNT; i++)
    {
        if (frm.supports_frame_metadata((rs2_frame_metadata_value)i))
        {
            csv << rs2_frame_metadata_to_string((rs2_frame_metadata_value)i) << ","
                << frm.get_frame_metadata((rs2_frame_metadata_value)i) << "\n";
        }
    }

    csv.close();
}
