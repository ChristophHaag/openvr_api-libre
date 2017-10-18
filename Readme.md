# What is this

The aim is to implement libopenvr_api.dll/libopenvr_api.so on top of OSVR.

Games and applications using OpenVR/SteamVR will ship this library (or rely on the library to be present in /usr/lib/).

Valve's libopenvr_api is just a wrapper around their proprietary and x86-only SteamVR runtime. However, libopenvr_api is documented and "open source", so other backends than SteamVR can be implemented, in this case OpenHMD.


Once this library is finished, the libopenvr_api shipped in any SteamVR application or game can be swapped out and the application can be used with only the OSVR drivers, Steam and SteamVR are not needed.

# Status


## Rendering

hellovr_opengl runs and renders something to its preview window.

The frames for each eye are submitted to libopenvr as OpenGL texture ids. The VR compositor is an SDL fullscreen window  with an OpenGL context that is shared with the rendering application, currently it displays these textures as they are.

Compositor TODO: Apply OpenHMD's universal distortion shader before displaying.

## Orientation Tracking

The HMD is hardcoded as deviceindex 0, controller support has to wait until the HMD tracking works.

Three matrices are used by hellovr_opengl:

General setup:
GetProjectionMatrix() is glm::perspective()
GetEyeToHeadTransform() - currently identity matrix

HMD orientation that gets updated every frame:
WaitGetPoses() - hmd quaternion converted to matrix

Current status: orientation works and scale looks okay, but cubes look huge.

# How to use and hack it

Build the openvr branch of https://github.com/ChristophHaag/OpenHMD/tree/openvr with cmake (automake suppport will be added later) and by default it will build libopenvr_api.so.

For easy testing hellovr_opengl is built.

The entire implementation of the relevant classes is contained in openvr_api_public.cpp. Only the three main classes are implemented, and many methods are still unimplemented.

There are many files copied from Valve's openvr repository. Most will be deleted once there is no dependency to them left.
