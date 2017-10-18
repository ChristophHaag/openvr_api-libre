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

#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Interface.h>
#include <osvr/ClientKit/InterfaceStateC.h>

#include <osvr/RenderKit/RenderManager.h>

#include <osvr/RenderKit/GraphicsLibraryOpenGL.h>
#include <osvr/RenderKit/RenderKitGraphicsTransforms.h>


using vr::EVRInitError;
using vr::IVRSystem;
using vr::VRInitError_None;

namespace vr
{

bool fulldbg = false;

static void *g_pVRModule = NULL;
osvr::renderkit::RenderManager* render;
osvr::clientkit::ClientContext *context;
int w;
int h;
osvr::renderkit::OSVR_ProjectionMatrix projl;
osvr::renderkit::OSVR_ProjectionMatrix projr;

osvr::clientkit::Interface head;

typedef void* (*VRClientCoreFactoryFn)(const char *pInterfaceName, int *pReturnCode);

static uint32_t g_nVRToken = 0;

uint32_t VR_GetInitToken()
{
    if (fulldbg) printf("inittoken requested\n");
    return g_nVRToken;
}

EVRInitError VR_LoadHmdSystemInternal();
void CleanupInternalInterfaces();

// Callback to set up for rendering into a given eye (viewpoint and projection).
void SetupEye(
    void* userData //< Passed into SetViewProjectionCallback
    , osvr::renderkit::GraphicsLibrary library //< Graphics library context to use
    , osvr::renderkit::RenderBuffer buffers //< Buffers to use
    , osvr::renderkit::OSVR_ViewportDescription
    viewport //< Viewport set by RenderManager
    , osvr::renderkit::OSVR_ProjectionMatrix
    projection //< Projection matrix set by RenderManager
    , size_t whichEye //< Which eye are we setting up for?
) {
    std::cout << "Setting up OSVR Rendermanager Eye..." << std::endl;
    // Make sure our pointers are filled in correctly.  The config file selects
    // the graphics library to use, and may not match our needs.
    if (library.OpenGL == nullptr) {
        std::cerr
        << "SetupEye: No OpenGL GraphicsLibrary, this should not happen"
        << std::endl;
        return;
    }
    if (buffers.OpenGL == nullptr) {
        std::cerr << "SetupEye: No OpenGL RenderBuffer, this should not happen"
        << std::endl;
        return;
    }

    std::cout << "Got eye from OSVR: " << whichEye << std::endl;
    std::cout << "viewport: " << viewport.width << "x" << viewport.height << " " << viewport.left << "" << viewport.lower << std::endl;

    // Set the viewport
    glViewport(static_cast<GLint>(viewport.left),
               static_cast<GLint>(viewport.lower),
               static_cast<GLint>(viewport.width),
               static_cast<GLint>(viewport.height));
}

// Callback to set up a given display, which may have one or more eyes in it
void SetupDisplay(
    void* userData //< Passed into SetDisplayCallback
    , osvr::renderkit::GraphicsLibrary library //< Graphics library context to use
    , osvr::renderkit::RenderBuffer buffers //< Buffers to use
) {
    std::cout << "Setting up OSVR Rendermanager Display..." << std::endl;
    // Make sure our pointers are filled in correctly.  The config file selects
    // the graphics library to use, and may not match our needs.
    if (library.OpenGL == nullptr) {
        std::cerr
        << "SetupDisplay: No OpenGL GraphicsLibrary, this should not happen"
        << std::endl;
        return;
    }
    if (buffers.OpenGL == nullptr) {
        std::cerr
        << "SetupDisplay: No OpenGL RenderBuffer, this should not happen"
        << std::endl;
        return;
    }

    osvr::renderkit::GraphicsLibraryOpenGL* glLibrary = library.OpenGL;

    // Clear the screen to black and clear depth
    //glClearColor(0, 0, 0, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

uint32_t VR_InitInternal( EVRInitError *peError, vr::EVRApplicationType eApplicationType )
{
        printf("initinternal create OSVR context\n");
        context = new osvr::clientkit::ClientContext("de.haagch.SteamVR-OSVR");
        head = context->getInterface("/me/head");


        *peError = VRInitError_None;
	return ++g_nVRToken;
}

void VR_ShutdownInternal()
{
        printf("shutdowninternal, destroy OSVR context\n");
        //context->free();
	++g_nVRToken;
}

EVRInitError VR_LoadHmdSystemInternal()
{
        printf("pretending to load hmdsysteminternal...\n");
	return VRInitError_None;
}

class OSVRHMDRenderModels : vr::IVRRenderModels {
    EVRRenderModelError LoadRenderModel_Async( const char *pchRenderModelName, RenderModel_t **ppRenderModel ) {
        printf("rendermodel async load requested: %s\n", pchRenderModelName);
        //TODO: load
        return VRRenderModelError_None;
    }

    void FreeRenderModel( RenderModel_t *pRenderModel ){
        printf("free render model\n");
        //TODO:
    }

    /** Loads and returns a texture for use in the application. */
    EVRRenderModelError LoadTexture_Async( TextureID_t textureId, RenderModel_TextureMap_t **ppTexture ) {
        printf("load texture async requested: %d\n", textureId);
        //TODO:
        return VRRenderModelError_None;
    }

    void FreeTexture( RenderModel_TextureMap_t *pTexture ) {
        printf("free texture requested\n");
        //TODO:

    }

    EVRRenderModelError LoadTextureD3D11_Async( TextureID_t textureId, void *pD3D11Device, void **ppD3D11Texture2D ) {
        printf("\n");
        //TODO:
        return VRRenderModelError_None;
    }

    EVRRenderModelError LoadIntoTextureD3D11_Async( TextureID_t textureId, void *pDstTexture ) {
        printf("\n");
        //TODO:
        return VRRenderModelError_None;
    }

    void FreeTextureD3D11( void *pD3D11Texture2D ) {
        printf("\n");
        //TODO:
    }

    uint32_t GetRenderModelName( uint32_t unRenderModelIndex, VR_OUT_STRING() char *pchRenderModelName, uint32_t unRenderModelNameLen ) {
        printf("get render model name for %d\n", unRenderModelIndex);
        //TODO:
        char* name = (char*) "OSVRModel";
        *pchRenderModelName = *name;
        return 0;
    }

    uint32_t GetRenderModelCount() {
        printf("get render model count\n");
        //TODO:
        return 1;
    }

    uint32_t GetComponentCount( const char *pchRenderModelName ) {
        printf("get component count, 0 not supported\n");
        //TODO:
        return 0;
    }

    uint32_t GetComponentName( const char *pchRenderModelName, uint32_t unComponentIndex, VR_OUT_STRING( ) char *pchComponentName, uint32_t unComponentNameLen ) {
        printf("get component name for %s\n", pchRenderModelName);
        //TODO: shouldn't happen with componentcount 0
        return 0;
    }

    uint64_t GetComponentButtonMask( const char *pchRenderModelName, const char *pchComponentName ) {
        printf("component button mask\n");
        //TODO:

        return 0;
    }

    uint32_t GetComponentRenderModelName( const char *pchRenderModelName, const char *pchComponentName, VR_OUT_STRING( ) char *pchComponentRenderModelName, uint32_t unComponentRenderModelNameLen ) {
        printf("component render model name\n");
        //TODO:

        return 0;
    }

    bool GetComponentState( const char *pchRenderModelName, const char *pchComponentName, const vr::VRControllerState_t *pControllerState, const RenderModel_ControllerMode_State_t *pState, RenderModel_ComponentState_t *pComponentState ) {
        printf("get component state\n");
        //TODO:
        return false;
    }

    bool RenderModelHasComponent( const char *pchRenderModelName, const char *pchComponentName ) {
        printf("render model has component %s: %s\n", pchRenderModelName, pchComponentName);
        //TODO:
        return false;
    }

    uint32_t GetRenderModelThumbnailURL( const char *pchRenderModelName, VR_OUT_STRING() char *pchThumbnailURL, uint32_t unThumbnailURLLen, vr::EVRRenderModelError *peError ) {
        printf("thumbnail for %s\n", pchRenderModelName);
        //TODO:
        return 0;
    }

    uint32_t GetRenderModelOriginalPath( const char *pchRenderModelName, VR_OUT_STRING() char *pchOriginalPath, uint32_t unOriginalPathLen, vr::EVRRenderModelError *peError ) {
        printf("render model original path for %s\n", pchRenderModelName);
        //TODO:
        return 0;
    }

    const char *GetRenderModelErrorNameFromEnum( vr::EVRRenderModelError error ) {
        printf("string for %d\n", error);
        //TODO:
        return "error" ;
    }
};

class OSVRHMDIVRSystem : IVRSystem
{

public:
    OSVRHMDIVRSystem() {
        render = osvr::renderkit::createRenderManager(context->get(), "OpenGL");
        if ((render == nullptr) || (!render->doingOkay())) {
            std::cerr << "Could not create RenderManager" << std::endl;
            return;
        }

        //render->SetViewProjectionCallback(SetupEye);
        //render->SetDisplayCallback(SetupDisplay);

        osvr::renderkit::RenderManager::OpenResults ret = render->OpenDisplay();
        if (ret.status == osvr::renderkit::RenderManager::OpenStatus::FAILURE) {
            std::cerr << "Could not open display" << std::endl;
            delete render;
            return;
        }
        if (ret.library.OpenGL == nullptr) {
            std::cerr << "Attempted to run an OpenGL program with a config file "
            << "that specified a different rendering library."
            << std::endl;
            return;
        }

        osvr::renderkit::GraphicsLibraryOpenGL* glLibrary = ret.library.OpenGL;
        glEnable(GL_DEPTH_TEST);

        //render->AddRenderCallback("/", DrawWorld);

        //TODO
        context->update();
        std::vector<osvr::renderkit::RenderInfo> renderInfo = render->GetRenderInfo();
        //w = ri.viewport.width;
        //h = ri.viewport.height;
        w = 0;
        h = 0;
        for (size_t i = 0; i < renderInfo.size(); i++) {
            std::cout << i << ": w,h " << renderInfo.at(i).viewport.width << "," << renderInfo.at(i).viewport.height << std::endl;
            h = renderInfo.at(i).viewport.height;
            w += renderInfo.at(i).viewport.width;
        }
        std::cout << "set w,h to " << w << "," << h << std::endl;

        projl = renderInfo.at(0).projection;
        projr = renderInfo.at(1).projection;
    }

    void GetRecommendedRenderTargetSize( uint32_t *pnWidth, uint32_t *pnHeight ) {
        //TODO:
        *pnWidth = w;
        *pnHeight = h;
        printf("recommended render target size: %dx%d\n", *pnWidth, *pnHeight);
    }

    HmdMatrix44_t GetProjectionMatrix( EVREye eEye, float fNearZ, float fFarZ ) {
        printf("projection matrix for eye: ");
        if (eEye == EVREye::Eye_Left) {
            printf("left ");
        } else {
            printf("right ");
        }

        osvr::renderkit::OSVR_ProjectionMatrix osvrprojection = (eEye == EVREye::Eye_Left ? projl : projr);
        GLdouble projection[16];
        osvr::renderkit::OSVR_Projection_to_OpenGL(projection, osvrprojection);

        for (auto const& value : projection) {
            std::cout << value << "; ";
        }
        std::cout << std::endl;

        HmdMatrix44_t matrix;
        matrix.m[0][0] = projection[0];
        matrix.m[0][1] = projection[1];
        matrix.m[0][2] = projection[2];
        matrix.m[0][3] = projection[3];

        matrix.m[1][0] = projection[4];
        matrix.m[1][1] = projection[5];
        matrix.m[1][2] = projection[6];
        matrix.m[1][3] = projection[7];

        matrix.m[2][0] = projection[8];
        matrix.m[2][1] = projection[9];
        matrix.m[2][2] = projection[10];
        matrix.m[2][3] = projection[11];

        matrix.m[3][0] = projection[12];
        matrix.m[3][1] = projection[13];
        matrix.m[3][2] = projection[14];
        matrix.m[3][3] = projection[15];

        return matrix;
    }

    void GetProjectionRaw( EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom ) {
        printf("projection raw\n");
        //TODO:
    }

    bool ComputeDistortion( EVREye eEye, float fU, float fV, DistortionCoordinates_t *pDistortionCoordinates ) {
        //printf("\n");
        //TODO:
        return true;
    }

    HmdMatrix34_t GetEyeToHeadTransform( EVREye eEye ) {
        printf("eye to head transform for eye: ");
        if (eEye == EVREye::Eye_Left) {
            printf("left\n");
        } else {
            printf("right\n");
        }
        //TODO:
        glm::mat4 Projection = glm::mat4(1.0);
        //std::cout << glm::to_string(Projection) << std::endl;

        HmdMatrix34_t matrix;
        matrix.m[0][0] = Projection[0][0];
        matrix.m[0][1] = Projection[0][1];
        matrix.m[0][2] = Projection[0][2];
        matrix.m[0][3] = Projection[0][3];

        matrix.m[1][0] = Projection[1][0];
        matrix.m[1][1] = Projection[1][1];
        matrix.m[1][2] = Projection[1][2];
        matrix.m[1][3] = Projection[1][3];

        matrix.m[2][0] = Projection[2][0];
        matrix.m[2][1] = Projection[2][1];
        matrix.m[2][2] = Projection[2][2];
        matrix.m[2][3] = Projection[2][3];

        return matrix;
    }

    bool GetTimeSinceLastVsync( float *pfSecondsSinceLastVsync, uint64_t *pulFrameCounter ) {
        //printf("\n");
        //TODO:

        pfSecondsSinceLastVsync = 0;
        pulFrameCounter = 0;
        return false;
    }
    int32_t GetD3D9AdapterIndex() {
        return 0;
    }

    void GetDXGIOutputInfo( int32_t *pnAdapterIndex ) {
    }

    void GetOutputDevice( uint64_t *pnDevice, ETextureType textureType ) {
        printf("output device\n");
        //TODO:
        *pnDevice = 0;
    }

    bool IsDisplayOnDesktop() {
        printf("extended mode: \n");
        //TODO:
        return true;
    }

    bool SetDisplayVisibility( bool bIsVisibleOnDesktop ) {
        return true;
    }

    void GetDeviceToAbsoluteTrackingPose( ETrackingUniverseOrigin eOrigin, float fPredictedSecondsToPhotonsFromNow, VR_ARRAY_COUNT(unTrackedDevicePoseArrayCount) TrackedDevicePose_t *pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount ) {
        printf("device to abs tracking pose\n");
        //TODO:
    }

    void ResetSeatedZeroPose() {
        printf("reset seated zero pose\n");
        //TODO:

    }

    HmdMatrix34_t GetSeatedZeroPoseToStandingAbsoluteTrackingPose() {
        printf("get seated zero to standing absolute\n");
        //TODO:

        HmdMatrix34_t matrix;
        return matrix;
    }

    HmdMatrix34_t GetRawZeroPoseToStandingAbsoluteTrackingPose() {
        printf("get rar zero to standing absolute\n");
        //TODO:

        HmdMatrix34_t matrix;
        return matrix;
    }

    uint32_t GetSortedTrackedDeviceIndicesOfClass( ETrackedDeviceClass eTrackedDeviceClass, VR_ARRAY_COUNT(unTrackedDeviceIndexArrayCount) vr::TrackedDeviceIndex_t *punTrackedDeviceIndexArray, uint32_t unTrackedDeviceIndexArrayCount, vr::TrackedDeviceIndex_t unRelativeToTrackedDeviceIndex = k_unTrackedDeviceIndex_Hmd ) {
        printf("get sorted tracked device indices\n");
        //TODO:
        return 0;
    }

    EDeviceActivityLevel GetTrackedDeviceActivityLevel( vr::TrackedDeviceIndex_t unDeviceId ) {
        printf("tracked device activity for %d: ", unDeviceId);
        if (unDeviceId == 0) {
            printf("hmd active\n");
            return EDeviceActivityLevel::k_EDeviceActivityLevel_UserInteraction;
        }
        printf("tracked device idle\n");
        //TODO:
        return EDeviceActivityLevel::k_EDeviceActivityLevel_Unknown;
    }


    void ApplyTransform( TrackedDevicePose_t *pOutputPose, const TrackedDevicePose_t *pTrackedDevicePose, const HmdMatrix34_t *pTransform ) {
        printf("apply transform\n");
        //TODO:
    }

    vr::TrackedDeviceIndex_t GetTrackedDeviceIndexForControllerRole( vr::ETrackedControllerRole unDeviceType ) {
        printf("device for role %d\n", unDeviceType);
        if (unDeviceType == ETrackedControllerRole::TrackedControllerRole_Invalid) {
            printf("invalid\n");
        } else if (unDeviceType == ETrackedControllerRole::TrackedControllerRole_LeftHand) {
            printf("left hand\n");
        } else {
            printf("right hand\n");
        }
        //TODO:
        return 0;
    }

    vr::ETrackedControllerRole GetControllerRoleForTrackedDeviceIndex( vr::TrackedDeviceIndex_t unDeviceIndex ) {
        printf("controller role for %d\n", unDeviceIndex);
        //TODO:
        return ETrackedControllerRole::TrackedControllerRole_Invalid;
    }

    ETrackedDeviceClass GetTrackedDeviceClass( vr::TrackedDeviceIndex_t unDeviceIndex ) {
        printf("get device class for %d: ", unDeviceIndex);
        if (unDeviceIndex == 0) {
            printf("HMD\n");
            return ETrackedDeviceClass::TrackedDeviceClass_HMD;
        } else {
            printf("invalid\n");
            //TODO:
            return ETrackedDeviceClass::TrackedDeviceClass_Invalid;
        }
    }

    bool IsTrackedDeviceConnected( vr::TrackedDeviceIndex_t unDeviceIndex ) {
        if (fulldbg) printf("is device connected %d\n", unDeviceIndex);
        if (unDeviceIndex == 0) {
            printf("hmd connected\n");
            return true;
        }
        //TODO:
        return false;
    }

    bool GetBoolTrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError *pError = 0L ) {
        //printf("bool tracked property\n");
        //TODO:
        return false;
    }

    float GetFloatTrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError *pError = 0L ) {
        //printf("\n");
        //TODO:
        return false;
    }

    int32_t GetInt32TrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError *pError = 0L ) {
        return 0;
    }

    uint64_t GetUint64TrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError *pError = 0L ) {
        return 0;
    }

    HmdMatrix34_t GetMatrix34TrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError *pError = 0L ) {
        printf("get matrix tracked device property");
        HmdMatrix34_t matrix;
        return matrix;
    }

    uint32_t GetStringTrackedDeviceProperty( vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, VR_OUT_STRING() char *pchValue, uint32_t unBufferSize, ETrackedPropertyError *pError = 0L ) {
        const char *str = [&]{
            if (prop == Prop_TrackingSystemName_String)  {
                return "OSVR";
            } else if (prop == Prop_SerialNumber_String) {
                return "OSVR SN"; //ohmd_list_gets(ctx, 0, OHMD_PRODUCT);
            } else {
                printf("requested not implemented %d prop\n", prop);
                return "Not implemented";
            }
        }();

        if (unBufferSize == 0) {
            return strlen(str) + 1;
        } else {
            printf("get prop %d for %d: %s (%lu)\n", prop, unDeviceIndex, str, strlen(str) + 1);
            strncpy(pchValue, str, strlen(str) + 1);
        }
        return sizeof("OSVR Test Display");
    }

    const char *GetPropErrorNameFromEnum( ETrackedPropertyError error ) {
        printf("error %d\n", error);
        //TODO:
        return "error";
    }

    bool PollNextEvent( VREvent_t *pEvent, uint32_t uncbVREvent ) {
        //printf("\n");
        //TODO:
        return false;
    }

    bool PollNextEventWithPose( ETrackingUniverseOrigin eOrigin, VREvent_t *pEvent, uint32_t uncbVREvent, vr::TrackedDevicePose_t *pTrackedDevicePose ) {
        //printf("\n");
        //TODO:
        return false;
    }

    const char *GetEventTypeNameFromEnum( EVREventType eType ) {
        printf("event type from enum %d\n", eType);
        //TODO:
        return "event";
    }

    HiddenAreaMesh_t GetHiddenAreaMesh( EVREye eEye, EHiddenAreaMeshType type = k_eHiddenAreaMesh_Standard ) {
        printf("hidden area mesh\n");
        //TODO:

    }

    bool GetControllerState( vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t *pControllerState, uint32_t unControllerStateSize ) {
        if (fulldbg) printf("get controller state for %d\n", unControllerDeviceIndex);
        //TODO:
        if (unControllerDeviceIndex == 0) {
            pControllerState->unPacketNum++; //TODO
            return true;
        }

        return false;
    }

    bool GetControllerStateWithPose( ETrackingUniverseOrigin eOrigin, vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t *pControllerState, uint32_t unControllerStateSize, TrackedDevicePose_t *pTrackedDevicePose ) {
        //printf("\n");
        //TODO:
        return false;
    }

    void TriggerHapticPulse( vr::TrackedDeviceIndex_t unControllerDeviceIndex, uint32_t unAxisId, unsigned short usDurationMicroSec ) {
        printf("trigger haptic pulse of duration %f ms\n", usDurationMicroSec / 1000.);
        //TODO:

    }

    const char *GetButtonIdNameFromEnum( EVRButtonId eButtonId ) {
        printf("button id name from enum %d\n", eButtonId);
        //TODO:
        return "buttonid";
    }

    const char *GetControllerAxisTypeNameFromEnum( EVRControllerAxisType eAxisType ) {
        printf("controller axis name from enum %d\n", eAxisType);
        //TODO:
        return "axis";
    }

    bool CaptureInputFocus() {
        printf("capture input focus \n");
        //TODO:
        return true;
    }

    void ReleaseInputFocus() {
        printf("release input focus\n");
        //TODO:
    }

    bool IsInputFocusCapturedByAnotherProcess() {
        if (fulldbg) printf("is input focus captured by other process?\n");
        //TODO:
        return false;
    }

    uint32_t DriverDebugRequest( vr::TrackedDeviceIndex_t unDeviceIndex, const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize ) {
        printf("debug request\n");
        //TODO:
        return 0;
    }

    vr::EVRFirmwareError PerformFirmwareUpdate( vr::TrackedDeviceIndex_t unDeviceIndex ) {
        printf("firmware update\n");
        //TODO:
        return EVRFirmwareError::VRFirmwareError_None;
    }

    void AcknowledgeQuit_Exiting() {
        printf("acknowledge quit\n");
        //TODO:
    }

    void AcknowledgeQuit_UserPrompt() {
        printf("acknowledge quit prompt\n");
        //TODO:
    }
};

void checkSDLError(int line = -1)
{
    const char *error = SDL_GetError();
    if (*error != '\0')
    {
        printf("SDL Error: %s\n", error);
        if (line != -1)
            printf(" + line: %i\n", line);
        SDL_ClearError();
    }
}

class OSVRHMDCompositor : IVRCompositor
{
private:
    SDL_Window *clientwindow = NULL;
    SDL_GLContext clientcontext;
    int w;
    int eyew;
    int h;
    SDL_Texture *texture;

public:
        OSVRHMDCompositor() {

        }

	void SetTrackingSpace( ETrackingUniverseOrigin eOrigin ) {
            printf("set tracking space\n");
            //TODO:
        }

	ETrackingUniverseOrigin GetTrackingSpace() {
            printf("get tracking space\n");
            //TODO:
            return ETrackingUniverseOrigin::TrackingUniverseStanding;
        }

	EVRCompositorError WaitGetPoses( VR_ARRAY_COUNT(unRenderPoseArrayCount) TrackedDevicePose_t* pRenderPoseArray, uint32_t unRenderPoseArrayCount,
		VR_ARRAY_COUNT(unGamePoseArrayCount) TrackedDevicePose_t* pGamePoseArray, uint32_t unGamePoseArrayCount ){
            if (fulldbg) printf("wait get poses for %d/%d tracked devices\n", unRenderPoseArrayCount, unGamePoseArrayCount);

            //TODO:
            pRenderPoseArray[0].bPoseIsValid = true;

            //ohmd_ctx_update(ctx);
            float zero[] = {.0, .0, .0, 1};

            //ohmd_device_setf(hmd, OHMD_ROTATION_QUAT, zero);
            //ohmd_device_setf(hmd, OHMD_POSITION_VECTOR, zero);

            OSVR_ReturnCode ret = OSVR_RETURN_FAILURE;
            OSVR_PoseState state;
            OSVR_TimeValue timestamp;
            do {
                if (fulldbg) std::cout << "Trying to get pose..." << std::endl;
                context->update();
                ret = osvrGetPoseState(head.get(), &timestamp, &state);
            } while (OSVR_RETURN_SUCCESS != ret);
            if (fulldbg) std::cout << "Got pose!" << std::endl;

            /*
            std::cout << "Got POSE state: Position = ("
            << state.translation.data[0] << ", "
            << state.translation.data[1] << ", "
            << state.translation.data[2] << "), orientation = ("
            << osvrQuatGetW(&(state.rotation)) << ", ("
            << osvrQuatGetX(&(state.rotation)) << ", "
            << osvrQuatGetY(&(state.rotation)) << ", "
            << osvrQuatGetZ(&(state.rotation)) << ")"
            << std::endl;
            */

            glm::quat rotation(osvrQuatGetW(&(state.rotation)), osvrQuatGetX(&(state.rotation)), osvrQuatGetY(&(state.rotation)), osvrQuatGetZ(&(state.rotation)));
            if (fulldbg) printf("osvr rotation quat %f %f %f %f\n", rotation[0], rotation[1], rotation[2], rotation[3]);

            glm::vec3 translation(state.translation.data[0], state.translation.data[1], state.translation.data[2]);
            if (true) printf("osvr pos %f %f %f\n", translation[0], translation[1], translation[2]);

            //glm::mat3 m = glm::mat3_cast(rotation);

            glm::mat4 m = glm::mat4_cast(rotation);
            glm::translate(m, translation);

            pRenderPoseArray[0].mDeviceToAbsoluteTracking.m[0][0] = m[0][0];
            pRenderPoseArray[0].mDeviceToAbsoluteTracking.m[0][1] = m[0][1];
            pRenderPoseArray[0].mDeviceToAbsoluteTracking.m[0][2] = m[0][2];

            pRenderPoseArray[0].mDeviceToAbsoluteTracking.m[1][0] = m[1][0];
            pRenderPoseArray[0].mDeviceToAbsoluteTracking.m[1][1] = m[1][1];
            pRenderPoseArray[0].mDeviceToAbsoluteTracking.m[1][2] = m[1][2];

            pRenderPoseArray[0].mDeviceToAbsoluteTracking.m[2][0] = m[2][0];
            pRenderPoseArray[0].mDeviceToAbsoluteTracking.m[2][1] = m[2][1];
            pRenderPoseArray[0].mDeviceToAbsoluteTracking.m[2][2] = m[2][2];


            pRenderPoseArray[0].eTrackingResult = ETrackingResult::TrackingResult_Running_OK;

            return VRCompositorError_None;
        }

	EVRCompositorError GetLastPoses( VR_ARRAY_COUNT( unRenderPoseArrayCount ) TrackedDevicePose_t* pRenderPoseArray, uint32_t unRenderPoseArrayCount,
		VR_ARRAY_COUNT( unGamePoseArrayCount ) TrackedDevicePose_t* pGamePoseArray, uint32_t unGamePoseArrayCount ) {
            printf("get last poses\n");
            //TODO:
            return VRCompositorError_None;
        }

	EVRCompositorError GetLastPoseForTrackedDeviceIndex( TrackedDeviceIndex_t unDeviceIndex, TrackedDevicePose_t *pOutputPose, TrackedDevicePose_t *pOutputGamePose ) {
            printf("get last pose for %d\n", unDeviceIndex);
            //TODO:
            return VRCompositorError_None;
        }

	EVRCompositorError Submit( EVREye eEye, const Texture_t *pTexture, const VRTextureBounds_t* pBounds = 0, EVRSubmitFlags nSubmitFlags = Submit_Default ) {
            if (fulldbg) {
                printf("submit frame - ");
                if (pTexture->eType == ETextureType::TextureType_OpenGL) {
                    printf("OpenGL - ");
                    printf(eEye == EVREye::Eye_Left ? "left\n" : "right\n");
                } else {
                    printf(" unsupported texture type. Use OpenGL!\n");
                    return VRCompositorError_IncompatibleVersion; //whatever
                }
            }

            //printf("texture pointer %p  ", pTexture->handle);
            uintptr_t handle = reinterpret_cast<uintptr_t>(pTexture->handle);
            unsigned int cast = (unsigned int) handle;
            GLuint gluint = reinterpret_cast<GLuint>(cast);
            //printf("gluint %d\n", gluint);
/*
            SDL_GL_MakeCurrent(compositorwindow, compositorcontext);

            if (eEye == EVREye::Eye_Left) {
                glClear(GL_COLOR_BUFFER_BIT);
            }

            // use the shader program
            glUseProgram(shader_program);

            // bind texture to texture unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gluint);

            // set texture uniform
            glUniform1i(texture_location, 0);

            if (eEye == EVREye::Eye_Left) {
                // bind the vao
                glBindVertexArray(vaol);
            } else {
                glBindVertexArray(vaor);
            }

            // draw
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // check for errors
            GLenum error = glGetError();
            if(error != GL_NO_ERROR) {
                std::cerr << error << std::endl;
            }

            if (eEye == EVREye::Eye_Right) {
            // finally swap buffers
            SDL_GL_SwapWindow(compositorwindow);
            }


            SDL_GL_MakeCurrent(clientwindow, clientcontext);
            */


            return VRCompositorError_None;
        }

	void ClearLastSubmittedFrame() {
            printf("clear last frame\n");
            //TODO:
        }

	void PostPresentHandoff() {
            printf("postpresenthandoff\n");
            //TODO:
        }

	bool GetFrameTiming( Compositor_FrameTiming *pTiming, uint32_t unFramesAgo = 0 ) {
            printf("get frame timing\n");
            //TODO:
            return false;
        }

	uint32_t GetFrameTimings( Compositor_FrameTiming *pTiming, uint32_t nFrames ) {
            printf("get frame timings\n");
            //TODO:
            return 0;
        }

	float GetFrameTimeRemaining() {
            printf("frame time remaining\n");
            //TODO:

            return 1;
        }

	void GetCumulativeStats( Compositor_CumulativeStats *pStats, uint32_t nStatsSizeInBytes ) {
            printf("get cumulative stats\n");
            //TODO:
        }

	void FadeToColor( float fSeconds, float fRed, float fGreen, float fBlue, float fAlpha, bool bBackground = false ) {
            printf("fade to color %f %f %f\n", fRed, fGreen, fBlue);
            //TODO:
        }

	HmdColor_t GetCurrentFadeColor( bool bBackground = false ) {
            printf("get fade color\n");
            //TODO:
            HmdColor_t hc;
            hc.a = .5;
            hc.r = .5;
            hc.g = .5;
            hc.b = .5;
            return hc;
        }

	void FadeGrid( float fSeconds, bool bFadeIn ) {
            printf("fade grid\n");
            //TODO:
        }

	float GetCurrentGridAlpha() {
            printf("get grid alpha\n");
            //TODO:
            return 0;
        }

	EVRCompositorError SetSkyboxOverride( VR_ARRAY_COUNT( unTextureCount ) const Texture_t *pTextures, uint32_t unTextureCount ) {
            printf("skybox override\n");
            //TODO:
            return VRCompositorError_None;
        }

	void ClearSkyboxOverride() {
            printf("clear skybox override\n");
            //TODO:
        }

	void CompositorBringToFront() {
            printf("bring compositor to front\n");
        }

	virtual void CompositorGoToBack() {
            printf("compositor to back\n");
        }

	void CompositorQuit() {
            printf("compositor quit\n");
            //SDL_DestroyWindow(compositorwindow);
            SDL_Quit();
        }

	bool IsFullscreen() {
            printf("compositor fullscreen\n");
            //TODO:
            return true;
        }

	uint32_t GetCurrentSceneFocusProcess() {
            printf("scene process\n");
            //TODO:
            return -1;
        }

	uint32_t GetLastFrameRenderer() {
            printf("last frame renderer\n");
            //TODO:
            return -1;
        }

	bool CanRenderScene() {
            printf("can render\n");
            //TODO:
            return true;
        }

	void ShowMirrorWindow() {
            printf("show mirror\n");
        }

	void HideMirrorWindow() {
            printf("hide mirror\n");
        }

	bool IsMirrorWindowVisible() {
            printf("mirror shown?\n");
            return false;
        }

	void CompositorDumpImages() {
            printf("dump images\n");
        }

	bool ShouldAppRenderWithLowResources() {
            printf("low resources?\n");
            //TODO:
            return false;
        }

	void ForceInterleavedReprojectionOn( bool bOverride ) {
            printf("force interleaved\n");
        }

	void ForceReconnectProcess() {
            printf("force reconnect\n");
        }

	void SuspendRendering( bool bSuspend ) {
            printf("suspend rendering %d\n", bSuspend);
        }

	vr::EVRCompositorError GetMirrorTextureD3D11( vr::EVREye eEye, void *pD3D11DeviceOrResource, void **ppD3D11ShaderResourceView ) {
            return VRCompositorError_None;
        }
	virtual void ReleaseMirrorTextureD3D11( void *pD3D11ShaderResourceView ) {
        }

	vr::EVRCompositorError GetMirrorTextureGL( vr::EVREye eEye, vr::glUInt_t *pglTextureId, vr::glSharedTextureHandle_t *pglSharedTextureHandle ) {
            printf("get mirror gl\n");
            //TODO:
            return VRCompositorError_None;
        }
	bool ReleaseSharedGLTexture( vr::glUInt_t glTextureId, vr::glSharedTextureHandle_t glSharedTextureHandle ) {
            printf("release gl shared texture\n");
            //TODO:
            return true;
        }
	void LockGLSharedTextureForAccess( vr::glSharedTextureHandle_t glSharedTextureHandle ) {
            printf("lock gl shared texture\n");
            //TODO:
        }
	void UnlockGLSharedTextureForAccess( vr::glSharedTextureHandle_t glSharedTextureHandle ) {
            printf("unlock gl shared texture\n");
            //TODO:
        }

	uint32_t GetVulkanInstanceExtensionsRequired( VR_OUT_STRING() char *pchValue, uint32_t unBufferSize ) {
            printf("vk instance req\n");
            //TODO:
            return 0;
        }

	uint32_t GetVulkanDeviceExtensionsRequired( VkPhysicalDevice_T *pPhysicalDevice, VR_OUT_STRING() char *pchValue, uint32_t unBufferSize ) {
            printf("vk dev req\n");
            //TODO:
            return 0;
        }
};


class OSVRHMDOverlay : IVROverlay
{
public:

    EVROverlayError FindOverlay( const char *pchOverlayKey, VROverlayHandle_t * pOverlayHandle ) {
        printf("FindOverlay\n");
        return EVROverlayError::VROverlayError_RequestFailed;
    }

    EVROverlayError CreateOverlay( const char *pchOverlayKey, const char *pchOverlayName, VROverlayHandle_t * pOverlayHandle ) {
        printf("CreateOverLay\n");
        return EVROverlayError::VROverlayError_None;
    }

    EVROverlayError DestroyOverlay( VROverlayHandle_t ulOverlayHandle ) {
        printf("DestroyOverlay\n");
        return EVROverlayError::VROverlayError_None;
    }

    EVROverlayError SetHighQualityOverlay( VROverlayHandle_t ulOverlayHandle ) {
        printf("SetHighQualityOverlay\n");
        return VROverlayError_None;
    }


    vr::VROverlayHandle_t GetHighQualityOverlay() {
        printf("GetHighQualityOverlay\n");
        return k_ulOverlayHandleInvalid;
    }

    uint32_t GetOverlayKey( VROverlayHandle_t ulOverlayHandle, VR_OUT_STRING() char *pchValue, uint32_t unBufferSize, EVROverlayError *pError = 0L ) {
        printf("GetOverlayKey\n");
        return 0;
    }

    uint32_t GetOverlayName( VROverlayHandle_t ulOverlayHandle, VR_OUT_STRING() char *pchValue, uint32_t unBufferSize, EVROverlayError *pError = 0L ) {
        printf("GetOverlayName\n");
        return 0;
    }

    EVROverlayError SetOverlayName( VROverlayHandle_t ulOverlayHandle, const char *pchName ) {
        printf("SetOverlayName\n");
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayImageData( VROverlayHandle_t ulOverlayHandle, void *pvBuffer, uint32_t unBufferSize, uint32_t *punWidth, uint32_t *punHeight ) {
        printf("GetOverlayImageData\n");
        return VROverlayError_None;
    }

    const char *GetOverlayErrorNameFromEnum( EVROverlayError error ) {
        printf("GetOverlayErrorNameFromEnum\n");
        return "Foobar";
    }

    EVROverlayError SetOverlayRenderingPid( VROverlayHandle_t ulOverlayHandle, uint32_t unPID ) {
        printf("SetOverlayRenderingPid\n");
        return VROverlayError_None;
    }

    uint32_t GetOverlayRenderingPid( VROverlayHandle_t ulOverlayHandle ) {
        printf("GetOverlayRenderingPid\n");
        return 0;
    }

    EVROverlayError SetOverlayFlag( VROverlayHandle_t ulOverlayHandle, VROverlayFlags eOverlayFlag, bool bEnabled ) {
        printf("SetOverlayFlag\n");
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayFlag( VROverlayHandle_t ulOverlayHandle, VROverlayFlags eOverlayFlag, bool *pbEnabled ) {
        printf("GetOverlayFlag\n");
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayColor( VROverlayHandle_t ulOverlayHandle, float fRed, float fGreen, float fBlue ) {
        printf("SetOverlayColor\n");
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayColor( VROverlayHandle_t ulOverlayHandle, float *pfRed, float *pfGreen, float *pfBlue ) {
        printf("GetOverlayColor\n");
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayAlpha( VROverlayHandle_t ulOverlayHandle, float fAlpha ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayAlpha( VROverlayHandle_t ulOverlayHandle, float *pfAlpha ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayTexelAspect( VROverlayHandle_t ulOverlayHandle, float fTexelAspect ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayTexelAspect( VROverlayHandle_t ulOverlayHandle, float *pfTexelAspect ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlaySortOrder( VROverlayHandle_t ulOverlayHandle, uint32_t unSortOrder ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlaySortOrder( VROverlayHandle_t ulOverlayHandle, uint32_t *punSortOrder ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayWidthInMeters( VROverlayHandle_t ulOverlayHandle, float fWidthInMeters ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayWidthInMeters( VROverlayHandle_t ulOverlayHandle, float *pfWidthInMeters ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayAutoCurveDistanceRangeInMeters( VROverlayHandle_t ulOverlayHandle, float fMinDistanceInMeters, float fMaxDistanceInMeters ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayAutoCurveDistanceRangeInMeters( VROverlayHandle_t ulOverlayHandle, float *pfMinDistanceInMeters, float *pfMaxDistanceInMeters ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayTextureColorSpace( VROverlayHandle_t ulOverlayHandle, EColorSpace eTextureColorSpace ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayTextureColorSpace( VROverlayHandle_t ulOverlayHandle, EColorSpace *peTextureColorSpace ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayTextureBounds( VROverlayHandle_t ulOverlayHandle, const VRTextureBounds_t *pOverlayTextureBounds ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayTextureBounds( VROverlayHandle_t ulOverlayHandle, VRTextureBounds_t *pOverlayTextureBounds ) {
        return VROverlayError_None;
    }

    uint32_t GetOverlayRenderModel( vr::VROverlayHandle_t ulOverlayHandle, char *pchValue, uint32_t unBufferSize, HmdColor_t *pColor, vr::EVROverlayError *pError ) {
        return VROverlayError_None;
    }

    vr::EVROverlayError SetOverlayRenderModel( vr::VROverlayHandle_t ulOverlayHandle, const char *pchRenderModel, const HmdColor_t *pColor ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayTransformType( VROverlayHandle_t ulOverlayHandle, VROverlayTransformType *peTransformType ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayTransformAbsolute( VROverlayHandle_t ulOverlayHandle, ETrackingUniverseOrigin eTrackingOrigin, const HmdMatrix34_t *pmatTrackingOriginToOverlayTransform ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayTransformAbsolute( VROverlayHandle_t ulOverlayHandle, ETrackingUniverseOrigin *peTrackingOrigin, HmdMatrix34_t *pmatTrackingOriginToOverlayTransform ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayTransformTrackedDeviceRelative( VROverlayHandle_t ulOverlayHandle, TrackedDeviceIndex_t unTrackedDevice, const HmdMatrix34_t *pmatTrackedDeviceToOverlayTransform ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayTransformTrackedDeviceRelative( VROverlayHandle_t ulOverlayHandle, TrackedDeviceIndex_t *punTrackedDevice, HmdMatrix34_t *pmatTrackedDeviceToOverlayTransform ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayTransformTrackedDeviceComponent( VROverlayHandle_t ulOverlayHandle, TrackedDeviceIndex_t unDeviceIndex, const char *pchComponentName ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayTransformTrackedDeviceComponent( VROverlayHandle_t ulOverlayHandle, TrackedDeviceIndex_t *punDeviceIndex, char *pchComponentName, uint32_t unComponentNameSize ) {
        return VROverlayError_None;
    }

    vr::EVROverlayError GetOverlayTransformOverlayRelative( VROverlayHandle_t ulOverlayHandle, VROverlayHandle_t *ulOverlayHandleParent, HmdMatrix34_t *pmatParentOverlayToOverlayTransform ) {
        return VROverlayError_None;
    }

    vr::EVROverlayError SetOverlayTransformOverlayRelative( VROverlayHandle_t ulOverlayHandle, VROverlayHandle_t ulOverlayHandleParent, const HmdMatrix34_t *pmatParentOverlayToOverlayTransform ) {
        return VROverlayError_None;
    }

    EVROverlayError ShowOverlay( VROverlayHandle_t ulOverlayHandle ) {
        return VROverlayError_None;
    }

    EVROverlayError HideOverlay( VROverlayHandle_t ulOverlayHandle ) {
        return VROverlayError_None;
    }

    bool IsOverlayVisible( VROverlayHandle_t ulOverlayHandle ) {
        return VROverlayError_None;
    }

    EVROverlayError GetTransformForOverlayCoordinates( VROverlayHandle_t ulOverlayHandle, ETrackingUniverseOrigin eTrackingOrigin, HmdVector2_t coordinatesInOverlay, HmdMatrix34_t *pmatTransform ) {
        return VROverlayError_None;
    }

    bool PollNextOverlayEvent( VROverlayHandle_t ulOverlayHandle, VREvent_t *pEvent, uint32_t uncbVREvent ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayInputMethod( VROverlayHandle_t ulOverlayHandle, VROverlayInputMethod *peInputMethod ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayInputMethod( VROverlayHandle_t ulOverlayHandle, VROverlayInputMethod eInputMethod ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayMouseScale( VROverlayHandle_t ulOverlayHandle, HmdVector2_t *pvecMouseScale ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayMouseScale( VROverlayHandle_t ulOverlayHandle, const HmdVector2_t *pvecMouseScale ) {
        return VROverlayError_None;
    }

    bool ComputeOverlayIntersection( VROverlayHandle_t ulOverlayHandle, const VROverlayIntersectionParams_t *pParams, VROverlayIntersectionResults_t *pResults ) {
        return false;
    }

    bool HandleControllerOverlayInteractionAsMouse( VROverlayHandle_t ulOverlayHandle, TrackedDeviceIndex_t unControllerDeviceIndex ) {
        return false;
    }

    bool IsHoverTargetOverlay( VROverlayHandle_t ulOverlayHandle ) {
        return false;
    }

    vr::VROverlayHandle_t GetGamepadFocusOverlay() {
        return 0;
    };

    EVROverlayError SetGamepadFocusOverlay( VROverlayHandle_t ulNewFocusOverlay ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayNeighbor( EOverlayDirection eDirection, VROverlayHandle_t ulFrom, VROverlayHandle_t ulTo ) {
        return VROverlayError_None;
    }

    EVROverlayError MoveGamepadFocusToNeighbor( EOverlayDirection eDirection, VROverlayHandle_t ulFrom ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayTexture( VROverlayHandle_t ulOverlayHandle, const Texture_t *pTexture ) {
        return VROverlayError_None;
    }

    EVROverlayError ClearOverlayTexture( VROverlayHandle_t ulOverlayHandle ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayRaw( VROverlayHandle_t ulOverlayHandle, void *pvBuffer, uint32_t unWidth, uint32_t unHeight, uint32_t unDepth ) {
        return VROverlayError_None;
    }

    EVROverlayError SetOverlayFromFile( VROverlayHandle_t ulOverlayHandle, const char *pchFilePath ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayTexture( VROverlayHandle_t ulOverlayHandle, void **pNativeTextureHandle, void *pNativeTextureRef, uint32_t *pWidth, uint32_t *pHeight, uint32_t *pNativeFormat, ETextureType *pAPIType, EColorSpace *pColorSpace, VRTextureBounds_t *pTextureBounds ) {
        return VROverlayError_None;
    }

    EVROverlayError ReleaseNativeOverlayHandle( VROverlayHandle_t ulOverlayHandle, void *pNativeTextureHandle ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayTextureSize( VROverlayHandle_t ulOverlayHandle, uint32_t *pWidth, uint32_t *pHeight ) {
        return VROverlayError_None;
    }

    EVROverlayError CreateDashboardOverlay( const char *pchOverlayKey, const char *pchOverlayFriendlyName, VROverlayHandle_t * pMainHandle, VROverlayHandle_t *pThumbnailHandle ) {
        return VROverlayError_None;
    }

    bool IsDashboardVisible() {
        return false;
    }

    bool IsActiveDashboardOverlay( VROverlayHandle_t ulOverlayHandle ) {
        return false;
    }

    EVROverlayError SetDashboardOverlaySceneProcess( VROverlayHandle_t ulOverlayHandle, uint32_t unProcessId ) {
        return VROverlayError_None;
    }

    EVROverlayError GetDashboardOverlaySceneProcess( VROverlayHandle_t ulOverlayHandle, uint32_t *punProcessId ) {
        return VROverlayError_None;
    }

    void ShowDashboard( const char *pchOverlayToShow ) {
    }

    vr::TrackedDeviceIndex_t GetPrimaryDashboardDevice() {
        return 0;
    }

    EVROverlayError ShowKeyboard( EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode, const char *pchDescription, uint32_t unCharMax, const char *pchExistingText, bool bUseMinimalMode, uint64_t uUserValue ) {
        return VROverlayError_None;
    }

    EVROverlayError ShowKeyboardForOverlay( VROverlayHandle_t ulOverlayHandle, EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode, const char *pchDescription, uint32_t unCharMax, const char *pchExistingText, bool bUseMinimalMode, uint64_t uUserValue ) {
        return VROverlayError_None;
    }

    uint32_t GetKeyboardText( VR_OUT_STRING() char *pchText, uint32_t cchText ) {
        return 0;
    }

    void HideKeyboard() {
    }

    void SetKeyboardTransformAbsolute( ETrackingUniverseOrigin eTrackingOrigin, const HmdMatrix34_t *pmatTrackingOriginToKeyboardTransform ) {
    }

    void SetKeyboardPositionForOverlay( VROverlayHandle_t ulOverlayHandle, HmdRect2_t avoidRect ) {
    }

    EVROverlayError SetOverlayIntersectionMask( VROverlayHandle_t ulOverlayHandle, VROverlayIntersectionMaskPrimitive_t *pMaskPrimitives, uint32_t unNumMaskPrimitives, uint32_t unPrimitiveSize = sizeof( VROverlayIntersectionMaskPrimitive_t ) ) {
        return VROverlayError_None;
    }

    EVROverlayError GetOverlayFlags( VROverlayHandle_t ulOverlayHandle, uint32_t *pFlags ) {
        return VROverlayError_None;
    }

    VRMessageOverlayResponse ShowMessageOverlay( const char* pchText, const char* pchCaption, const char* pchButton0Text, const char* pchButton1Text = nullptr, const char* pchButton2Text = nullptr, const char* pchButton3Text = nullptr ) {
        return VRMessageOverlayResponse::VRMessageOverlayResponse_ButtonPress_0;
    }
};

bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
    lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void *VR_GetGenericInterface(const char *pchInterfaceVersion, EVRInitError *peError)
{
        printf("Creating OSVR impl of interface: %s \n", pchInterfaceVersion);

        *peError = VRInitError_None;
        if (startsWith("IVRSystem_", pchInterfaceVersion)) {
            OSVRHMDIVRSystem *ivrsystem = new OSVRHMDIVRSystem();
            return ivrsystem;
        } else if (startsWith("IVRRenderModels_", pchInterfaceVersion)) {
            OSVRHMDRenderModels *ivrrenderModels = new OSVRHMDRenderModels();
            return ivrrenderModels;
        } else if (startsWith("IVRCompositor_", pchInterfaceVersion)) {
            OSVRHMDCompositor *ivrcompositor= new OSVRHMDCompositor();
            return ivrcompositor;
        } else if (startsWith("IVROverlay_", pchInterfaceVersion)) {
            OSVRHMDOverlay *ivroverlay = new OSVRHMDOverlay();
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
