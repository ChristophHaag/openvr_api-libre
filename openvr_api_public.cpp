//========= Copyright Valve Corporation ============//
#define VR_API_EXPORT 1
#include "openvr.h"
#include "hmderrors_public.h"
#include <string.h>

#include <iostream>
#include <vector>

//for the compositor
#include <SDL.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_inverse.hpp>

//TODO what is this
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

bool fulldbg = false;

#ifdef BUILD_OSVR
#include "openvr_osvr.h"
#endif

#ifdef BUILD_OPENHMD
#include "openvr_openhmd.h"
#endif

using vr::EVRInitError;
using vr::IVRSystem;
using vr::VRInitError_None;

namespace vr
{

static void *g_pVRModule = NULL;

typedef void* (*VRClientCoreFactoryFn)(const char *pInterfaceName, int *pReturnCode);

static uint32_t g_nVRToken = 0;

uint32_t VR_GetInitToken()
{
    if (fulldbg) printf("inittoken requested\n");
    return g_nVRToken;
}

EVRInitError VR_LoadHmdSystemInternal();
void CleanupInternalInterfaces();


uint32_t VR_InitInternal( EVRInitError *peError, vr::EVRApplicationType eApplicationType )
{
    printf("initinternal\n");
    *peError = VRInitError_None;
    return ++g_nVRToken;
}

void VR_ShutdownInternal()
{
    printf("shutdowninternal\n");
    //context->free();
    ++g_nVRToken;
}

EVRInitError VR_LoadHmdSystemInternal()
{
    printf("pretending to load hmdsysteminternal...\n");
    return VRInitError_None;
}

bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

IVRSystem *ivrsystem;
IVRCompositor *ivrcompositor;
IVRRenderModels *ivrrendermodels;
IVROverlay *ivroverlay;
char* backend;
void *VR_GetGenericInterface(const char *pchInterfaceVersion, EVRInitError *peError)
{
    if (!ivrsystem) {
        backend = getenv ("OPENVR_BACKEND");
        if (backend==NULL) {
            printf ("Choose backend OSVR or OPENHMD with OPENVR_BACKEND=[OSVR|OPENHMD]\n");
#ifdef BUILD_OSVR
            backend = strdup("OSVR"); // default
#elif BUILD_OPENHMD
            backend = strdup("OPENHMD");
#else
#error "No backend built!"
#endif
        }
        printf ("Using backend: %s\n", backend);
        if (strcmp(backend, "OSVR") == 0) {
#if !(defined(BUILD_OSVR))
            printf("Error: OSVR backend chosen but not built!\n");
            return nullptr;
#endif
            ivrsystem = new OSVRHMDIVRSystem(); //make sure ivrsystem is always going to be initialized first so implementations can do their base init here
            ivrcompositor = new OSVRHMDCompositor();
            ivrrendermodels = new OSVRHMDRenderModels();
            ivroverlay = new OSVRHMDOverlay();
        } else if (strcmp(backend, "OPENHMD") == 0) {
#if !(defined(BUILD_OPENHMD))
            printf("Error: OpenHMD backend chosen but not built!\n");
            return nullptr;
#endif
            ivrsystem = new OpenHMDIVRSystem();
            ivrcompositor = new OpenHMDCompositor();
            ivrrendermodels = new OpenHMDRenderModels();
            ivroverlay = new OpenHMDOverlay();
        }
    }

    printf("Creating %s impl of interface: %s\n", backend, pchInterfaceVersion);

    *peError = VRInitError_None;
    if (startsWith("IVRSystem_", pchInterfaceVersion)) {
        return ivrsystem;
    } else if (startsWith("IVRRenderModels_", pchInterfaceVersion)) {
        return ivrrendermodels;
    } else if (startsWith("IVRCompositor_", pchInterfaceVersion)) {
        return ivrcompositor;
    } else if (startsWith("IVROverlay_", pchInterfaceVersion)) {
        return ivroverlay;
    } else {
        printf("interface %s failed\n", pchInterfaceVersion);
        //ohmd_ctx_destroy(ctx);
        *peError = VRInitError_Unknown;
        return nullptr;
    }
}

bool VR_IsInterfaceVersionValid(const char *pchInterfaceVersion)
{
    if (fulldbg) printf("interfaceversion %s is valid requested, confirming..\n", pchInterfaceVersion);
    return true;
}

bool VR_IsHmdPresent()
{
    printf("IsHmdPresent requested, confirming..\n");
    return true;
}

/** Returns true if the OpenVR runtime is installed. */
bool VR_IsRuntimeInstalled()
{
    printf("Runtime installed requested, confirming..\n");
    return true;
}


/** Returns where OpenVR runtime is installed. */
const char *VR_RuntimePath()
{
    printf("Runtime path requested, pretending \"\"\n");
    return "";
}


/** Returns the symbol version of an HMD error. */
const char *VR_GetVRInitErrorAsSymbol( EVRInitError error )
{
    return GetIDForVRInitError( error );
}


/** Returns the english string version of an HMD error. */
const char *VR_GetVRInitErrorAsEnglishDescription( EVRInitError error )
{

    return GetEnglishStringForHmdError( error );
}


VR_INTERFACE const char *VR_CALLTYPE VR_GetStringForHmdError( vr::EVRInitError error );

/** Returns the english string version of an HMD error. */
const char *VR_GetStringForHmdError( EVRInitError error )
{
    return VR_GetVRInitErrorAsEnglishDescription( error );
}

}
