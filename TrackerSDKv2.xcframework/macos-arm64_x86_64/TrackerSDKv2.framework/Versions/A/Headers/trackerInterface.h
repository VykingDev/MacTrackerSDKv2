//
//  trackerInterface.h
//
// A simple 'C' interface to the Tracker processing.
// Allows access from non C++  software, and use from .net (P/Invoke) i.e. Unity
//
//  Created by ccladmin on 12/11/2018.
//  Copyright © 2016 vyking. All rights reserved.
//

#ifndef trackerInterface_h
#define trackerInterface_h

#include <stdint.h>

#if PRINT_DEBUG
#define VYKING_TRACKER_SDK_VERSION "2023-04-03DAR"
#else
#if ENABLE_ARKIT_TRK
#define VYKING_TRACKER_SDK_VERSION "2023-04-03AR"
#else
#define VYKING_TRACKER_SDK_VERSION "2023-06-01R"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Simple Debug function pointer
typedef void (*LogPtr)(const char *msg);

#define TRACKER_RAW_METHOD(METHOD_NAME) VykingTrackerInterface_##METHOD_NAME

#if defined(_WIN32) || defined(WIN32)
  #if _MODULELIB_
    #define MODULEAPI  __declspec(dllimport)
  #else
    #define MODULEAPI  __declspec(dllexport)
  #endif
#elif defined(__ANDROID__) || defined(__APPLE__)
  #define MODULEAPI __attribute__ ((visibility ("default")))
#else
  #define MODULEAPI
#endif

// NOTE: as this interface has to be used by many different languages .net, Java, Kotlin, Swift, Objective-c etc ..
//       the data passing needs to be a simple as possible, so NO unions (difficult for transfering between languages)
//       hence the simple tag/value for the event callbacks
// -----------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------
// Tags used by JSON configuration
#define tkBaseDir        "baseDir"                   // specifies the base directory, String
#define tkDisplaySize    "displaySize"               // Display size, float[2] width = 0, height = 1
#define tkImageSize      "imageSize"                 // Image size, float[2] width = 0, height = 1
#define tkCameraFacing   "facing"                    // which directon the camera is facing, bool -> true = front facing, false = back facing
#define tkTracker        "tracker"                   // which tracker to use (face/feet)
#define tkImageStackSize "imageStackSize"            // size of the image stack required
#define tkConfigCfg      "config"                    // config block to be passed to setValues from access and/or start
#define tkCameraCfg      "camera"                    // config block to be used for Camera
#define tkBoxerCfg       "boxer"                     // config block to be used for Boxer
#define tkTrackerCfg     "tracker"                   // config block to be used for tracker
#define tkSegmenterCfg   "segmenter"                 // config block to be used for segmenter
#define tkStatsCfg       "stats"                     // general stats from SDK
#define tkVersionCfg     "Version"                   // used to collect SDK version

// -----------------------------------------------------------------------------------------------------------

typedef uint32_t vkRect[4];                          // top = 0,left = 1,bottom = 2, right = 3
typedef float vkBox[4];                              // camera facebox
typedef float vkMatrix4x4[16];                       // matrix 4x4
typedef float vkRotation[9];                         // 3 x 3 rotation values
typedef float vkTranslation[3];                      // x,y,z translation
typedef float vkScale[3];                            // x,y,z scale
typedef float vkFeatures[70];                        // feature points (X and Y)

// callback interface and structure
typedef struct _vkFrameData {                        // Main structure for Callback
  uint32_t frameID;                                  // identifier for the frame data
  uint64_t frameNumber;                              // frame number
  uint64_t timeStamp;                                // frame timestamp
  float    imageWidth;
  float    imageHeight;
  float    focalLength;
  float    sensorOriantation;                        // in radians
  int      availableObjects;                         // number of object detected
  bool     needsMirroring;                           // image will need to be mirrored
} vkFrameData;

typedef struct _vkImageRef {                         // Struture used to get the image (as chroma and luma YuV format)
  const void *luma;                                  // luma (texture/image) (Y)
  const void *chroma;                                // chroma (texture/image) (CbCr)
  #if __ANDROID__
  const void *lumaImagePtr;                          // pointer to the luma image directly from the camera
  const void *chromaImagePtr;                        // pointer to the chroma image directly from the camera
  int   lumaContextReferenceID = -1;             // the reference id for the context of the luma
  int   chromaContextReferenceID = -1;           // the reference id for the context of the chroma
  int   lumaRowStride ;                             // the stride of the luma data in bytes
  int   chromaRowStride ;                           // stride between rows in chroma data
  #endif
} vkImageRef;

enum vkTrackerTag : uint32_t {
  VK_TRK_UNKNOWN    = 0,
  VK_TRK_NEW_FRAME  = 1                              // new frame has arrived, frameID is in the val.u32 field
};

enum vkTrackerObjectType : uint32_t {
  VK_TRK_OBJ_UNKNOWN = 0,
  VK_TRK_OBJ_FACE = 1,
  VK_TRK_OBJ_LEFT_FOOT = 2,
  VK_TRK_OBJ_RIGHT_FOOT = 3,
  VK_TRK_OBJ_LEFT_WRIST = 4,
  VK_TRK_OBJ_RIGHT_WRIST = 5,
};

enum vkTrackerBox : uint32_t {
  VK_TRK_BOX_UNKNOWN  = 0,
  VK_TRK_BOX_HARDWARE = 1,
  VK_TRK_BOX_EXTRACT  = 2,
  VK_TRK_BOX_FACE     = 3,
  VK_TRK_BOX_TRACK    = 4,
};

enum vkUIOrientation: uint32_t {
  vkUIInterfaceOrientationPortrait           = 1,
  vkUIInterfaceOrientationPortraitUpsideDown = 2,
  vkUIInterfaceOrientationLandscapeleft      = 3,
  vkUIInterfaceOrientationLandscapeRight     = 4
};

enum vkDeviceOrientation: uint32_t {
   vkDeviceOrientationUnknown            = 0,
   vkDeviceOrientationPortrait           = 1,
   vkDeviceOrientationPortraitUpsideDown = 2,
   vkDeviceOrientationLandscapeLeft      = 3,
   vkDeviceOrientationLandscapeRight     = 4,
   vkDeviceOrientationFaceUp             = 5,
   vkDeviceOrientationFaceDown           = 6
};

typedef void vkTrackerNotifyFn(bool good);                            // event to know when camera is initalised/destroyed
typedef void vkTrackerEventFn(enum vkTrackerTag tag,uint32_t value);  // function typedef for frame callback
typedef void vkTrackerLogEventFn( const char *loggingMessage );       // callback function to capture events logged

// Available on ALL platforms, only functional on Android
// Access function for use with NativeActivity on android platform
// Must be called before MODULEAPI TRACKER_RAW_METHOD(initialise) method to ensure software has access
// to the JavaVM, and the main Activity.
bool MODULEAPI TRACKER_RAW_METHOD(preInitialise)(void *ptr1,void *ptr2);

// Main initalisation functions, usually only called once
void MODULEAPI TRACKER_RAW_METHOD(SetDebugErrorFunctions)(LogPtr dbgFN,LogPtr errFN);
bool MODULEAPI TRACKER_RAW_METHOD(initialise)(const char *jsonConfig,vkTrackerNotifyFn readyFN);
bool MODULEAPI TRACKER_RAW_METHOD(destroy)(vkTrackerNotifyFn completeFN);
void MODULEAPI TRACKER_RAW_METHOD(registerLogEventCallback)(vkTrackerLogEventFn logEventFN );

// Main control functions
bool MODULEAPI TRACKER_RAW_METHOD(access)(const char *jsonConfig,vkTrackerEventFn frameFN);
bool MODULEAPI TRACKER_RAW_METHOD(unaccess)(void);
bool MODULEAPI TRACKER_RAW_METHOD(start)(const char *jsonConfig);
bool MODULEAPI TRACKER_RAW_METHOD(stop)(void);
bool MODULEAPI TRACKER_RAW_METHOD(getARKitMatricies)( unsigned int searchframeID,
                                                      void        *orientation,
                                                      int          viewPortWidth,
                                                      int          viewPortHeight,
                                                      void       **arRefPtr  );
long MODULEAPI TRACKER_RAW_METHOD(externalSendARFrame)( void *arFramePtr );
long MODULEAPI TRACKER_RAW_METHOD(externalSendFrame)( void *framePtr, float fov );
void MODULEAPI *TRACKER_RAW_METHOD(convertURLtoCVPixelBuffer)( const char *fileName, int modeBits );

// Detector Control/Access Functions
bool MODULEAPI TRACKER_RAW_METHOD(reset)(bool full);
bool MODULEAPI TRACKER_RAW_METHOD(getMeshSize)(enum vkTrackerObjectType type,int *triangleCount,int *verticiesCount,int *uvCount);
bool MODULEAPI TRACKER_RAW_METHOD(getMeshData)(enum vkTrackerObjectType type,unsigned short *triangleArray,int triangleSize,float *verticesArray,int verticesSize,float *uvArray,int uvSize);

// Value/Paramete/Object set/get functions
const void MODULEAPI *TRACKER_RAW_METHOD(getObject)(const char *tag);
void MODULEAPI TRACKER_RAW_METHOD(releaseObject)(const char *tag,const void *object);
const void MODULEAPI *TRACKER_RAW_METHOD(getObjectEx)(const char *tag,const uint32_t iVal);
void MODULEAPI TRACKER_RAW_METHOD(releaseObjectEx)(const char *tag,const uint32_t iVal,const void *object);
const char MODULEAPI *TRACKER_RAW_METHOD(getValues)(const char *jsonArray);
const char MODULEAPI *TRACKER_RAW_METHOD(setValues)(const char *jsonObject);

// Frame access functions
bool MODULEAPI TRACKER_RAW_METHOD(frameRelease)(const uint32_t frameID);
bool MODULEAPI TRACKER_RAW_METHOD(frameGetImage)(const uint32_t frameID,vkImageRef *imgInfo);
#if __ANDROID__
bool MODULEAPI TRACKER_RAW_METHOD(frameGetImageAsData)(const uint32_t frameID,vkImageRef *imgInfoRaw);
#endif
bool MODULEAPI TRACKER_RAW_METHOD(frameGetMask)(const uint32_t frameID,vkImageRef *imgInfo);
bool MODULEAPI TRACKER_RAW_METHOD(frameReleaseImage)(vkImageRef *imgInfo);
bool MODULEAPI TRACKER_RAW_METHOD(frameGetData)(const uint32_t frameID,vkFrameData *info);

bool MODULEAPI TRACKER_RAW_METHOD(frameGetObjectBox)(const uint32_t frameID,int objectIndex,enum vkTrackerBox boxType,bool normalise,enum vkTrackerObjectType *objType,vkBox *box);
bool MODULEAPI TRACKER_RAW_METHOD(frameGetObjectPose)(const uint32_t frameID,int objectIndex,enum vkTrackerObjectType *objType,vkMatrix4x4 *pose);
bool MODULEAPI TRACKER_RAW_METHOD(frameGetObjectPoseComponents)(const uint32_t frameID,int objectIndex,enum vkTrackerObjectType *objType,vkRotation *rotation,vkTranslation *translation,vkScale *scale);
bool MODULEAPI TRACKER_RAW_METHOD(frameGetObjectFeatures)(const uint32_t frameID,int objectIndex,bool normalise,enum vkTrackerObjectType *objType,int *numFeatures,vkFeatures *featuresX,vkFeatures *featuresY);
bool MODULEAPI TRACKER_RAW_METHOD(frameGetObjectMeshVertices)(const uint32_t frameID,int objectIndex,bool normalise,enum vkTrackerObjectType *objType,int numVerticies,float *verticiesArray);
bool MODULEAPI TRACKER_RAW_METHOD(frameGetObjectMask)(const uint32_t frameID,int objectIndex,enum vkTrackerObjectType *objType,vkImageRef *imgInfo,vkMatrix4x4 *pose, float *zAxisRotationOccluder);
#if __ANDROID__
bool MODULEAPI TRACKER_RAW_METHOD(frameGetObjectMaskAsData)(const uint32_t frameID,int objectIndex,enum vkTrackerObjectType *objType,vkImageRef *imgInfo,vkMatrix4x4 *pose, float *zAxisRotationOccluder);
bool MODULEAPI TRACKER_RAW_METHOD(frameGetCameraAsRGBAData)( const uint32_t  frameID, 
          vkImageRef     *imgInfo,
          void           *rgbaDataPtr, 
          int             rgba_width,
          int             rgba_height,
          float           rotation ) ;
const unsigned char MODULEAPI *TRACKER_RAW_METHOD(RGBAdataFrameItemForReading)( uint32_t frameID, int width, int height, float rotation, float poseRotation, float maskRotation );
#endif
bool MODULEAPI TRACKER_RAW_METHOD(frameGetDbgImage)(const uint32_t frameID,vkImageRef *imgInfo);


bool MODULEAPI TRACKER_RAW_METHOD(cameraDisplayTransformForViewPort)(const uint32_t frameID,const float viewPortWidth,const float viewPortHeight,const enum vkUIOrientation uiOrientation,vkMatrix4x4 *outMatrix4x4);
bool MODULEAPI TRACKER_RAW_METHOD(cameraProjectionTransformForViewPort)(const uint32_t frameID,const float viewPortWidth,const float viewPortHeight,const float zNear,const float zFar,vkMatrix4x4 *outMatrix4x4);
bool MODULEAPI TRACKER_RAW_METHOD(cameraProjectionTransformForViewPortOverride)(const uint32_t frameID,const float viewPortWidth,const float viewPortHeight,const float zNear,const float zFar,vkMatrix4x4 *outMatrix4x4, bool *overrideOrientation);
bool MODULEAPI TRACKER_RAW_METHOD(deviceOrientationChange)(const enum vkDeviceOrientation deviceOrientation);

// external APP (Unity) helper functions
void MODULEAPI *TRACKER_RAW_METHOD(collectGLContextFN)(void);   // used to collect the GL Context for for rendering

bool MODULEAPI TRACKER_RAW_METHOD(setImageRegionOfInterest)( vkBox roi );
bool MODULEAPI TRACKER_RAW_METHOD(getImageRegionOfInterest)( const uint32_t frameID, vkBox *roi );
void MODULEAPI TRACKER_RAW_METHOD(set3DModelSerialNumber)( const char *serialString, const char *sneakerKitReference );
long MODULEAPI TRACKER_RAW_METHOD(getSDKexpiry)( );

#undef MODULEAPI
#ifdef __cplusplus
}
#endif

#endif /* trackerInterface_h */
