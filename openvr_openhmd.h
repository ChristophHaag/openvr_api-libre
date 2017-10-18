#include <openhmd.h>

namespace vr {
ohmd_context* ctx;
ohmd_device* hmd;

// gets float values from the device and prints them
void print_infof(ohmd_device* hmd, const char* name, int len, ohmd_float_value val)
{
    float f[len];
    ohmd_device_getf(hmd, val, f);
    printf("%-25s", name);
    for(int i = 0; i < len; i++)
        printf("%f ", f[i]);
    printf("\n");
}

// gets int values from the device and prints them
void print_infoi(ohmd_device* hmd, const char* name, int len, ohmd_int_value val)
{
    int iv[len];
    ohmd_device_geti(hmd, val, iv);
    printf("%-25s", name);
    for(int i = 0; i < len; i++)
        printf("%d ", iv[i]);
    printf("\n");
}

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

class OpenHMDRenderModels : public IVRRenderModels {
    EVRRenderModelError LoadRenderModel_Async( const char *pchRenderModelName, RenderModel_t **ppRenderModel ) {
        printf("rendermodel async load requested: %s\n", pchRenderModelName);
        //TODO: load
        return VRRenderModelError_None;
    }

    void FreeRenderModel( RenderModel_t *pRenderModel ) {
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
        char* name = (char*) "OpenHMDModel";
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

class OpenHMDIVRSystem : public IVRSystem
{
    int w;
    int h;
public:
    OpenHMDIVRSystem() {

        printf("initinternal create openhmd context\n");
        ctx = ohmd_ctx_create();

        // Probe for devices
        int num_devices = ohmd_ctx_probe(ctx);
        if(num_devices < 0){
            printf("failed to probe devices: %s\n", ohmd_ctx_get_error(ctx));
            ohmd_ctx_destroy(ctx);
            return;
        }

        printf("num devices: %d\n\n", num_devices);

        // Print device information for default device (change i < num_devices for all)
        for(int i = 0; i < 1; i++){
            printf("device %d\n", i);
            printf("  vendor:  %s\n", ohmd_list_gets(ctx, i, OHMD_VENDOR));
            printf("  product: %s\n", ohmd_list_gets(ctx, i, OHMD_PRODUCT));
            printf("  path:    %s\n\n", ohmd_list_gets(ctx, i, OHMD_PATH));
        }

        hmd = ohmd_list_open_device(ctx, 0); // default device (0)

        if(!hmd){
            printf("failed to open device: %s\n", ohmd_ctx_get_error(ctx));
            ohmd_ctx_destroy(ctx);
            return;
        }

        // Print hardware information for the opened device
        int ivals[2];
        ohmd_device_geti(hmd, OHMD_SCREEN_HORIZONTAL_RESOLUTION, ivals);
        ohmd_device_geti(hmd, OHMD_SCREEN_VERTICAL_RESOLUTION, ivals + 1);
        printf("resolution:              %i x %i\n", ivals[0], ivals[1]);

        if (fulldbg) {
            print_infof(hmd, "hsize:",            1, OHMD_SCREEN_HORIZONTAL_SIZE);
            print_infof(hmd, "vsize:",            1, OHMD_SCREEN_VERTICAL_SIZE);
            print_infof(hmd, "lens separation:",  1, OHMD_LENS_HORIZONTAL_SEPARATION);
            print_infof(hmd, "lens vcenter:",     1, OHMD_LENS_VERTICAL_POSITION);
            print_infof(hmd, "left eye fov:",     1, OHMD_LEFT_EYE_FOV);
            print_infof(hmd, "right eye fov:",    1, OHMD_RIGHT_EYE_FOV);
            print_infof(hmd, "left eye aspect:",  1, OHMD_LEFT_EYE_ASPECT_RATIO);
            print_infof(hmd, "right eye aspect:", 1, OHMD_RIGHT_EYE_ASPECT_RATIO);
            print_infof(hmd, "distortion k:",     6, OHMD_DISTORTION_K);

            print_infoi(hmd, "digital button count:", 1, OHMD_BUTTON_COUNT);
        }

        ohmd_device_geti(hmd, OHMD_SCREEN_HORIZONTAL_RESOLUTION, &w);
        ohmd_device_geti(hmd, OHMD_SCREEN_VERTICAL_RESOLUTION, &h);
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


        float fov;
        float aspect;
        if (eEye == EVREye::Eye_Left) {
            ohmd_device_getf(hmd, OHMD_LEFT_EYE_FOV, &fov);
            ohmd_device_getf(hmd, OHMD_LEFT_EYE_ASPECT_RATIO, &aspect);
        } else {
            ohmd_device_getf(hmd, OHMD_RIGHT_EYE_FOV, &fov);
            ohmd_device_getf(hmd, OHMD_RIGHT_EYE_ASPECT_RATIO, &aspect);
        }
        printf(" with fov %f, aspect %f: ", fov, aspect);
        glm::mat4 Projection = glm::perspective(fov, aspect, fNearZ, fFarZ);
        printf("%s\n", glm::to_string(Projection).c_str());

        HmdMatrix44_t matrix;
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

        matrix.m[3][0] = Projection[3][0];
        matrix.m[3][1] = Projection[3][1];
        matrix.m[3][2] = Projection[3][2];
        matrix.m[3][3] = Projection[3][3];

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
        const char *str = [&] {
            if (prop == Prop_TrackingSystemName_String)  {
                return "OpenHMD";
            } else if (prop == Prop_SerialNumber_String) {
                return ohmd_list_gets(ctx, 0, OHMD_PRODUCT);
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
        return sizeof("OpenHMD Test Display");
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

class OpenHMDCompositor : public IVRCompositor
{
private:
    SDL_Window *compositorwindow = NULL;
    SDL_Window *clientwindow = NULL;
    SDL_GLContext clientcontext;
    SDL_GLContext compositorcontext;
    SDL_Renderer *renderer;
    int w;
    int eyew;
    int h;
    SDL_Texture *texture;
    GLuint shader_program;
    GLint texture_location;
    GLuint vaol, vbol, vaor, vbor;

    void mkvaovbo(bool leftEye) {
        if (leftEye) {
            glGenVertexArrays(1, &vaol);
            glBindVertexArray(vaol);

            glGenBuffers(1, &vbol);
            glBindBuffer(GL_ARRAY_BUFFER, vbol);

            GLfloat vertexData[] = {
                //  X     Y     Z           U     V
                0.0f, 1.0f, 0.0f,       1.0f, 1.0f, // vertex 0
                -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, // vertex 1
                0.0f,-1.0f, 0.0f,       1.0f, 0.0f, // vertex 2
                -1.0f,-1.0f, 0.0f,       0.0f, 0.0f, // vertex 3
            }; // half fullscreen quad, 4 vertices with 5 components (floats) each
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*5, vertexData, GL_STATIC_DRAW);

        } else {
            glGenVertexArrays(1, &vaor);
            glBindVertexArray(vaor);

            glGenBuffers(1, &vbor);
            glBindBuffer(GL_ARRAY_BUFFER, vbor);

            GLfloat vertexData[] = {
                //  X     Y     Z           U     V
                1.0f, 1.0f, 0.0f,       1.0f, 1.0f, // vertex 0
                0.0f, 1.0f, 0.0f,       0.0f, 1.0f, // vertex 1
                1.0f,-1.0f, 0.0f,       1.0f, 0.0f, // vertex 2
                0.0f,-1.0f, 0.0f,       0.0f, 0.0f, // vertex 3
            }; // half fullscreen quad, 4 vertices with 5 components (floats) each
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*5, vertexData, GL_STATIC_DRAW);

        }


        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));

        GLuint ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        GLuint indexData[] = {
            0,1,2, // first triangle
            2,1,3, // second triangle
        };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*2*3, indexData, GL_STATIC_DRAW);
        glBindVertexArray(0);
    }
public:
    OpenHMDCompositor() {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);


        ohmd_device_geti(hmd, OHMD_SCREEN_HORIZONTAL_RESOLUTION, &w);
        eyew = w/2;
        ohmd_device_geti(hmd, OHMD_SCREEN_VERTICAL_RESOLUTION, &h);

        clientwindow = SDL_GL_GetCurrentWindow();
        clientcontext = SDL_GL_GetCurrentContext();
        printf("current GL context %p\n", clientcontext);


        uint32_t windowflags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS;
        compositorwindow = SDL_CreateWindow("libopenVR Compositor (OpenHMD)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, windowflags);
        if (compositorwindow == NULL) {
            printf("Could not create window: %s\n", SDL_GetError());
        }
        checkSDLError(__LINE__);

        // current context is the client application rendering
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
        compositorcontext = SDL_GL_CreateContext(compositorwindow);


        SDL_GL_MakeCurrent(compositorwindow, compositorcontext);

        // credit: https://github.com/progschj/OpenGL-Examples/blob/master/03texture.cpp

        std::string vertex_source =
            "#version 330\n"
            "layout(location = 0) in vec4 vposition;\n"
            "layout(location = 1) in vec2 vtexcoord;\n"
            "out vec2 ftexcoord;\n"
            "void main() {\n"
            "   ftexcoord = vtexcoord;\n"
            "   gl_Position = vposition;\n"
            "}\n";

        std::string fragment_source =
            "#version 330\n"
            "uniform sampler2D tex;\n" // texture uniform
            "in vec2 ftexcoord;\n"
            "layout(location = 0) out vec4 FragColor;\n"
            "void main() {\n"
            "   FragColor = texture(tex, ftexcoord);\n"
            "}\n";

        GLuint vertex_shader, fragment_shader;

        // we need these to properly pass the strings
        const char *source;
        int length;

        // create and compiler vertex shader
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        source = vertex_source.c_str();
        length = vertex_source.size();
        glShaderSource(vertex_shader, 1, &source, &length);
        glCompileShader(vertex_shader);
        //TODO: error checking

        // create and compiler fragment shader
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        source = fragment_source.c_str();
        length = fragment_source.size();
        glShaderSource(fragment_shader, 1, &source, &length);
        glCompileShader(fragment_shader);
        //TODO: error checking


        // create program
        shader_program = glCreateProgram();

        // attach shaders
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);

        // link the program and check for errors
        glLinkProgram(shader_program);
        //TODO: error checking

        texture_location = glGetUniformLocation(shader_program, "tex");

        mkvaovbo(true);
        mkvaovbo(false);

        SDL_GL_MakeCurrent(clientwindow, clientcontext);

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
                                     VR_ARRAY_COUNT(unGamePoseArrayCount) TrackedDevicePose_t* pGamePoseArray, uint32_t unGamePoseArrayCount ) {
        if (fulldbg) printf("wait get poses for %d/%d tracked devices\n", unRenderPoseArrayCount, unGamePoseArrayCount);

        //TODO:
        pRenderPoseArray[0].bPoseIsValid = true;

        ohmd_ctx_update(ctx);
        float zero[] = {.0, .0, .0, 1};

        ohmd_device_setf(hmd, OHMD_ROTATION_QUAT, zero);
        ohmd_device_setf(hmd, OHMD_POSITION_VECTOR, zero);

        float quat[4];
        ohmd_device_getf(hmd, OHMD_ROTATION_QUAT, quat);
        if (fulldbg) printf("ohmd rotation quat %f %f %f %f\n", quat[0], quat[1], quat[2], quat[3]);

        // CAREFUL: w x y z
        glm::quat rotation(quat[3], quat[0], quat[1], quat[2]);
        glm::mat3 m = glm::mat3_cast(rotation);

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
        SDL_DestroyWindow(compositorwindow);
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

class OpenHMDOverlay : public IVROverlay
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

}
