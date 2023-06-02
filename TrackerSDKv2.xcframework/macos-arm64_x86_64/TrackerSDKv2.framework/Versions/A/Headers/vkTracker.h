//  vkTracker.m
//  vykingAdSDK
//
//  Created by ccladmin on 25/10/2016.
//  Copyright © 2016 vyking. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SceneKit/SceneKit.h>
#import <simd/simd.h>

#if TARGET_OS_IOS
#include <UIKit/UIKit.h>

#if ENABLE_ARKIT_TRK
#import <ARKit/ARKit.h>
#else
#import <ARKit/ARKit.h> // WARNING CK 2021-10-22
#endif


#endif

#include "trackerInterface.h"    // access the structs/enums etc.. required by the interface

#if !defined(MODULEAPI)
#define MODULEAPI __attribute__ ((visibility ("default")))
#endif

#if TARGET_OS_IOS
MODULEAPI @interface GroupAnchorMtrx : NSObject {
  @public NSString      *anchorName;
  @public simd_float4x4  aMatrix;
}

@property(nonatomic, readwrite) NSString *anchorName;  // Property
@property(nonatomic, readwrite) simd_float4x4 aMatrix;  // Property
@end

typedef enum vkParameterTy {
  paramNullType,
  paramFloatType,
  paramIntType,
  paramBooType,
  paramStrngType
} vkParameterType;

@interface ParameterTripple :NSObject {
  // @public NSString       *pName;
  // @public bool            useScientific;
  //         vkParameterType paramTy;
  void                   *address;
}
@property(nonatomic, readwrite) NSString       *pName;  // Property
@property(nonatomic, readwrite) bool            useScientific;
@property(nonatomic, readwrite) vkParameterType paramTy;
- (void *)getAddress;
@end

MODULEAPI @interface GroupARFrameCache : NSObject {
  @public unsigned int                  frameNumerID;    // the frame numer associated with the arkit frame
  #if true // ENABLE_ARKIT_TRK
  @public ARFrame                      *arFrame;
  @public NSArray<ARAnchor *>          *anchors;
  #endif
  @public simd_float4x4                 cameraMatrix;
  @public simd_float4x4                 cameraViewMatrix;
  @public simd_float4x4                 cameraProjectionMatrix;
  @public NSMutableArray<GroupAnchorMtrx *> *anchorMatrixList;
  @public CVPixelBufferRef              cvPixelBuffer;
//  @public CVPixelBufferRef              cvPixelBuffer2;
//  @public CVPixelBufferRef              cvPixelBuffer3;
//  @public CVPixelBufferRef              cvPixelBuffer4;
}

@property(nonatomic, readwrite) unsigned int frameNumerID;  // Property
@property(nonatomic, readwrite) simd_float4x4 cameraMatrix;  // Property
@property(nonatomic, readwrite) simd_float4x4 cameraViewMatrix;  // Property
@property(nonatomic, readwrite) simd_float4x4 cameraProjectionMatrix;  // Property
@property(nonatomic, readwrite) NSMutableArray<GroupAnchorMtrx *> *anchorMatrixList;  // Property
@property(nonatomic, readwrite) CVPixelBufferRef cvPixelBuffer;
//@property(nonatomic, readwrite) CVPixelBufferRef cvPixelBuffer2;
//@property(nonatomic, readwrite) CVPixelBufferRef cvPixelBuffer3;
//@property(nonatomic, readwrite) CVPixelBufferRef cvPixelBuffer4;
-(void )setaMtrx: (simd_float4x4 )mtrx name: (NSString *)name ;
@end

#endif // TARGET_OS_IOS

@protocol vkTrackerInterface
- (void) vkReady: (bool) good;
- (void) vkShutdown: (bool) good;
- (void) vktEvent: (enum vkTrackerTag) tag value: (uint32_t) value;
@optional
- (void) vkLogEvent: (NSString *)logMessage;
- (void) vkReporterFn: (uint32_t )frameID;
@required
@end

MODULEAPI @interface vkTracker : NSObject 
{
}


// Main initalisation functions, usually only called once
- (void) setDebugErrorFunctions: (LogPtr) dbgFN errFN: (LogPtr) errFN;
- (bool) initialise: (NSDictionary *) params delegate: (id<vkTrackerInterface>) delegate;
- (bool) destroy;

// Main control functions
- (bool) access:(NSDictionary *) params;

#if TARGET_OS_IOS
-(bool )getARKitMatricies: (uint                    )searchframeID
              orientation: (UIInterfaceOrientation *)orientation
            viewPortWidth: (int                     )viewPortWidth
           viewPortHeight: (int                     )viewPortHeight
                 ArRefPtr: (GroupARFrameCache     **)ArRefPtr ;

- (CVPixelBufferRef )xxXconvertURLtoCVPixelBuffer:(NSString *)fileName modeBits: (int )modeBits;

- (void )xxXreleaseConvertURLtoCVPixelBuffer:(CVPixelBufferRef )cvPxBfrRef ;

-(NSArray *)GetVKeditableParameters; // function to recoved the list of parameters which can be edited

// function to recover parameter address and type
- (bool) vkGetParameterAddress:(NSString        *)parameterName
                  parameterPtr:(void           **)parameterPtr
                     paramType:(vkParameterType *)paramType;

#if ENABLE_ARKIT_TRK
- (long) externalSendARFrame:(ARFrame *) arFrame;  // returns the frame id
#endif

#endif // TARGET_OS_IOS

#if TARGET_OS_MAC || TARGE_OS_IPHONE
- (long) externalSendFrame:(CVPixelBufferRef )frame fov:(float )fov;
#endif

- (bool) unaccess;
- (bool) start: (NSDictionary *) params;
- (bool) stop;

// Detector Control/Access Functions
- (bool) reset: (bool) full;
- (bool) getMeshSize:(enum vkTrackerObjectType) type triangleCount: (int *) triangleCount verticiesCount: (int *) verticiesCount componentCount: (int *)componentCount;
- (bool) getMeshData:(enum vkTrackerObjectType) type triangleArray: (unsigned short *) triangleArray triangleSize: (int) triangleSize verticesArray: (float *) verticesArray verticesSize: (int) verticesSize uvArray:(float *) uvArray uvSize: (int) uvSize;

// Value/Paramete/Object set/get functions
- (const void *) getObject: (NSString *) tag;
- (void) releaseObject: (NSString *) tag object: (const void *) object;
- (const void *) getObjectEx: (NSString *) tag iVal: (const uint32_t) iVal;
- (void) releaseObjectEx: (NSString *) tag iVal: (const uint32_t) iVal object: (const void *) object;
- (NSDictionary *) getValues: (NSDictionary *) params;
- (NSDictionary *) setValues: (NSDictionary *) params;

// Frame access functions
- (bool) frameRelease: (const uint32_t) frameID;
- (bool) frameGetImage: (const uint32_t) frameID imgInfo: (vkImageRef *)imgInfo;
- (bool) frameGetMask: (const uint32_t) frameID imgInfo: (vkImageRef *)imgInfo;
- (bool) frameGetDbgImage: (const uint32_t) frameID imgInfo: (vkImageRef *)imgInfo;
- (bool) frameReleaseImage: (vkImageRef *) imgInfo;
- (bool) frameGetData: (const uint32_t) frameID info: (vkFrameData *) info;
- (bool) frameGetObjectBox: (const uint32_t) frameID objectIndex: (int) objectIndex boxType: (enum vkTrackerBox) boxType normalise: (bool) normalise objType: (enum vkTrackerObjectType *) objType box: (vkBox *) box;
- (bool) frameGetObjectPose: (const uint32_t) frameID objectIndex: (int) objectIndex objType: (enum vkTrackerObjectType *) objType pose: (vkMatrix4x4 *) pose;
- (bool) frameGetObjectPoseAsSCNMatrix4: (const uint32_t) frameID objectIndex: (int) objectIndex objType: (enum vkTrackerObjectType *) objType pose: (SCNMatrix4 *) pose;
- (bool) frameGetObjectMask: (const uint32_t) frameID objectIndex: (int) objectIndex objType: (enum vkTrackerObjectType *) objType imgInfo: (vkImageRef *)imgInfo pose: (vkMatrix4x4 *) pose zAxisRotationOccluder: (float *)zAxisRotationOccluder;
- (bool) frameGetObjectMaskAsSCNMatrix4: (const uint32_t) frameID objectIndex: (int) objectIndex objType: (enum vkTrackerObjectType *) objType imgInfo: (vkImageRef *)imgInfo pose: (SCNMatrix4 *) pose zAxisRotationOccluder: (float *)zAxisRotationOccluder;
- (bool) frameGetObjectFeatures: (const uint32_t) frameID objectIndex: (int) objectIndex normalise: (bool) normalise objType: (enum vkTrackerObjectType *) objType numFeatures: (int *) numFeatures featuresX: (vkFeatures *) featuresX featuresY: (vkFeatures *) featuresY;
- (bool) frameGetObjectMeshVertices: (const uint32_t) frameID objectIndex: (int) objectIndex normalise: (bool) normalise objType: (enum vkTrackerObjectType *) objType numVerticies: (int) numVerticies verticiesArray: (float *) verticiesArray;

- (bool) cameraDisplayTransformForViewPort: (const uint32_t) frameID viewPortWidth:(const float) viewPortWidth viewPortHeight: (const float) viewPortHeight uiOrientation: (const enum vkUIOrientation) uiOrientation outMatrix: (vkMatrix4x4 *) outMatrix4x4;
- (bool) cameraDisplayTransformForViewPortAsSCNMatrix4: (const uint32_t) frameID viewPortWidth:(const float) viewPortWidth viewPortHeight: (const float) viewPortHeight uiOrientation: (const enum vkUIOrientation) uiOrientation outMatrix: (SCNMatrix4 *) outMatrix4x4;
- (bool) cameraProjectionTransformForViewPort: (const uint32_t) frameID viewPortWidth:(const float) viewPortWidth viewPortHeight: (const float) viewPortHeight zNear: (const float) zNear zFar: (const float) zFar outMatrix: (vkMatrix4x4 *) outMatrix4x4;
- (bool) cameraProjectionTransformForViewPortAsSCNMatrix4: (const uint32_t) frameID viewPortWidth:(const float) viewPortWidth viewPortHeight: (const float) viewPortHeight zNear: (const float) zNear zFar: (const float) zFar outMatrix: (SCNMatrix4 *) outMatrix4x4 isLandscape: (bool *) isLandscape;
- (bool) cameraProjectionTransformForViewPortAsSCNMatrix4: (const uint32_t) frameID viewPortWidth:(const float) viewPortWidth viewPortHeight: (const float) viewPortHeight zNear: (const float) zNear zFar: (const float) zFar outMatrix: (SCNMatrix4 *) outMatrix4x4 ;
#if TARGET_OS_IOS
- (bool) deviceOrientationChange: (UIDeviceOrientation) deviceOrientation;
#else
- (bool) deviceOrientationChange: (enum vkDeviceOrientation) deviceOrientation;
#endif

// function to set the region of interest for the image processing in image coordinates
- (bool )setImageRegionOfInterest:(float )top
                        roiBottom:(float )bottom
                          roiLeft:(float )left
                         roiRight:(float )right;

// function to get the region of interest for the image processing in image coordinates
- (bool )getImageRegionOfInterest:(const uint32_t)frameID
                           roiTop:(float *)top
                        roiBottom:(float *)bottom
                          roiLeft:(float *)left
                         roiRight:(float *)right ;

- (void ) set3DModelSerialNumber:(NSString *)serialString sneakerKitRef:(NSString *)sneakerKitRef ;

- (long ) getSDKexpiry;

@end
