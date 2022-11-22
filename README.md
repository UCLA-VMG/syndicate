# C++ codebase for synchronized data capture from a "syndicate" of multimodal sensors.

**Contributing Authors**: [Adnan Armouti](https://github.com/adnan-armouti), [Anirudh Bindiganavale Harish](https://github.com/Anirudh0707), [Alexander (Sasha) Vilesov](https://github.com/asvilesov/).

**Please note**: This code respository is currently under development, and may therefore have bugs. Some of the libraries may have specific CPU and OS dependencies (Windows only). We will release detailed documentation to successfully operate the repository, as well as video tutorials over time.

**Important branches**: main (general definition of all sensors) and osa_config (specific low-light sensors with optimized XML configuration files). Device configurations such as fps, resolution, exposure time, gain, black_level, etc... are set in the XML file.

All **relevant packages** are installed with vcpkg. Please follow this [video](https://www.youtube.com/watch?v=FeBzSYiWkEU) for more detailed instructions.

**Commands to build and execute the files**: <br/>
Ctrl + Shift + P to access command palette -> delete cache and reconfigure -> build all -> cd to Bin\Debug path to run capture_config.exe (main branch has one fixed XML file for sensor configuration, while osa_config branch allows you to choose XML file as command line argument i.e. XML path must be specified).
