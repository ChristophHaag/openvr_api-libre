# What is this

The aim is to implement libopenvr_api.dll/libopenvr_api.so on top of a libre runtime like OpenHMD or OSVR SDK.

Games and applications using the OpenVR API ship the libopenvr_api.so library or rely on it to be present in /usr/lib/.

Valve's libopenvr_api.so is a wrapper. It loads the implementation of the OpenVR classes from their closed source and x86-only SteamVR runtime. However, libopenvr_api is documented and "open source", so other backends than SteamVR can be implemented like OpenHMD or OSVR.

Once this library is finished, the libopenvr_api shipped in any SteamVR application or game can be swapped out with this one and the application can be used with only the OSVR or OpenHMD drivers, Steam and SteamVR are not needed.

# Status

Note: hellovr_opengl draws upside down in its preview window. That's not a bug of this libopenvr_api, hellovr_opengl upstream should fix that.

## OpenHMD

### Rendering

Creates fullscreen window with the HMD's resolution via SDL and displays the textures side by side.

### Orientation Tracking

The HMD is hardcoded as deviceindex 0, controller support has to wait until the HMD tracking works.

Three matrices are used by hellovr_opengl:

General setup:
GetProjectionMatrix() is glm::perspective()
GetEyeToHeadTransform() - currently identity matrix

HMD orientation that gets updated every frame:
WaitGetPoses() - hmd quaternion converted to matrix

### TODO
* apply OpenHMD's distortion shader before displaying the textures
* Check all the matrices (I think the cubes are too big but could be coincidence)
* Controllers, wait for OpenHMD's controller API

## OSVR

### Rendering

not much yet

### Orientation Tracking

### TODO

Check positional tracking in WaitGetPoses.

# How to use and hack it

git clone --recursive https://github.com/ChristophHaag/openvr_api-libre
cd openvr_api-libre
cmake .
make -j12
./hellovr_opengl

hellovr_opengl is linked to libopenvr_api.so in the same directory, no special setup needed when you already have it in /usr/lib.

You can customize the build with `cmake -DOPENHMD_LIBOPENVR=OFF` and `cmake -DOSVR_LIBOPENVR=OFF`. At runtime you can choose one of the backends with the environment variable `OPENVR_BACKEND=OPENHMD` or `OPENVR_BACKEND=OSVR`.

openvr_api_public.cpp is the main file of the library. The implementation of the classes is in `openvr_osvr.h` and `openvr_openhmd.h`. Not all classes and many methods are not implemented.
