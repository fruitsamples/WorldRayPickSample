/*  *	WRay_Document.c * *	QuickDraw 3D 1.6 Sample *	Robert Dierkes * *	 07/28/98	RDD		Created. *//*------------------*//*	Include Files	*//*------------------*/#include "QD3D.h"#include "QD3DCamera.h"#include "QD3DGroup.h"#include "QD3DMath.h"#include "QD3DTransform.h"#include "QD3DView.h"#include "WRay_Document.h"#include "WRay_Error.h"#include "WRay_Main.h"#include "WRay_Memory.h"#include "WRay_Scene.h"#include <math.h>/*----------------------*//*		Macros			*//*----------------------*/#define	uIsZero(v)				(((v) >= -kQ3RealZero) && ((v) <= kQ3RealZero))/*------------------*//*	  Constants		*//*------------------*//*----------------------*//*	Extern Declarations	*//*----------------------*//*----------------------*//*	Global Declarations	*//*----------------------*//*----------------------*//*	Local Prototypes	*//*----------------------*/staticTQ3Status	Document_InitializeWindow(				TDocumentPtr		pDocument);staticTQ3Status	Document_DisposeWindow(				TDocumentPtr		pDocument);/* *	Document_Initialize * *	Returns kQ3False if anything fails while creating the document. */TQ3Boolean Document_Initialize(			TDocumentPtr	pDocument) {	DEBUG_ASSERT(pDocument != NULL, Document_Initialize);	/*------------------------------*	 * Common to this document only *	 *------------------------------*/	pDocument->fWindow    = NULL;	pDocument->fView      = NULL;	pDocument->fModel     = NULL;	Q3Matrix4x4_SetIdentity(&pDocument->fMatrix);	/*================*	 * Create objects *	 *================*/	/* Create window */	if (Document_InitializeWindow(pDocument) == kQ3Failure) {		return kQ3False;	}	/* Create view(s) */	if ((pDocument->fView = Scene_NewView(pDocument->fWindow)) == NULL) {		return kQ3False;	}	/* Create fModel */	if ((pDocument->fModel = Scene_NewModel()) == NULL) {		return kQ3False;	}	/* Center the model */	if (Document_UpdateCamera(pDocument) == kQ3Failure) {		return kQ3False;	}	return kQ3True;}/* *	Document_Exit */TQ3Boolean Document_Exit(			TDocumentPtr	pDocument){	DEBUG_ASSERT(pDocument != NULL, Document_Exit);	Object_Dispose_NULL(&pDocument->fView);	Object_Dispose_NULL(&pDocument->fModel);	return (Document_DisposeWindow(pDocument) == kQ3Success)				? kQ3True				: kQ3False;}#pragma mark -/* *	Document_Draw */TQ3Status Document_Draw(			TDocumentPtr	pDocument){	TQ3ViewObject	aView;	TQ3Status		status;	TQ3ViewStatus	viewStatus;	DEBUG_ASSERT(pDocument != NULL, Document_Draw);	aView = pDocument->fView;	if (Q3View_StartRendering(aView) == kQ3Success) {		do {			status = Document_Submit_Objects(pDocument, aView);			viewStatus = Q3View_EndRendering(aView);		} while (viewStatus == kQ3ViewStatusRetraverse);		if (viewStatus != kQ3ViewStatusDone) {			return kQ3Failure;		}	} else {		ERROR_DEBUG_STR("Document_Draw: Q3View_StartRendering failed.");		return kQ3Failure;	}	return kQ3Success;}/* *	Document_Submit_Objects * * This assumes the model is already centered at the origin. * No parameter checking is done. */TQ3Status Document_Submit_Objects(			TDocumentPtr		pDocument,			TQ3ViewObject		view){	DEBUG_ASSERT(pDocument != NULL, Document_Submit_Objects);	DEBUG_ASSERT(view != NULL,      Document_Submit_Objects);	Q3MatrixTransform_Submit(&pDocument->fMatrix, view);	return Q3DisplayGroup_Submit(pDocument->fModel, view);}#pragma mark -/* *	Document_InitializeWindow */staticTQ3Status Document_InitializeWindow(			TDocumentPtr	pDocument){	WindowPtr	pWindow;	DEBUG_ASSERT(pDocument != NULL, Document_InitializeWindow);	pWindow = GetNewCWindow(kWindowRsrcID, NULL, kWindowOnTop);	pDocument->fWindow = pWindow;	if (pWindow == NULL) {		ERROR_DEBUG_STR("Document_InitializeWindow: GetNewCWindow failed.");		return kQ3Failure;	}	SetPort((GrafPtr) pWindow);	ShowWindow(pWindow);	return kQ3Success;}/* *	Document_DisposeWindow */staticTQ3Status Document_DisposeWindow(			TDocumentPtr	pDocument){	DEBUG_ASSERT(pDocument != NULL, Document_DisposeWindow);	if (pDocument->fWindow != NULL) {		DisposeWindow (pDocument->fWindow);		pDocument->fWindow = NULL;		return kQ3Failure;	}	return kQ3Success;}#pragma mark -/* *	Document_UpdateCameraAspectRatio */TQ3Status Document_UpdateCameraAspectRatio(			TDocumentPtr		pDocument,			Rect				*pPortRect){	TQ3CameraObject		camera;	TQ3Status			status;	float				aspectRatioXToY;	status = Q3View_GetCamera(pDocument->fView, &camera);	if ((status == kQ3Failure)  ||		(camera == NULL)) {		ERROR_DEBUG_STR("Document_UpdateCameraAspectRatio: Q3View_GetCamera failed.");		return status;	}	status = Q3ViewAngleAspectCamera_GetAspectRatio(camera, &aspectRatioXToY);	if (status == kQ3Success) {		aspectRatioXToY	=	(float) (pPortRect->right  - pPortRect->left) / 							(float) (pPortRect->bottom - pPortRect->top);		status = Q3ViewAngleAspectCamera_SetAspectRatio(camera, aspectRatioXToY);	}	Object_Dispose_NULL(&camera);	return status;}/* *	Document_UpdateCamera */TQ3Status Document_UpdateCamera(			TDocumentPtr		pDocument){	TQ3CameraObject				camera;	TQ3CameraPlacement			placement;	TQ3CameraRange				range;	TQ3BoundingBox 				boundingBox;	float 						maxDimension;	float 						fieldOfView;	TQ3Status					status;	status = Q3View_GetCamera(pDocument->fView, &camera);	if ((status == kQ3Failure)  ||		(camera == NULL)) {		ERROR_DEBUG_STR("Document_UpdateCamera: Q3View_GetCamera failed.");		return kQ3Failure;	}	if (Q3Object_IsType(camera, kQ3CameraTypeViewAngleAspect) == kQ3False) {		return kQ3Failure;	}	/* Set FOV */	fieldOfView = kDefaultFieldOfView;	Q3ViewAngleAspectCamera_SetFOV(camera, fieldOfView);	/* Get maxDimension and bounding box */	if (Document_GetMaximumDimension(pDocument, &maxDimension, &boundingBox) == kQ3Failure) {		return kQ3Failure;	}	/* Set cameraLocation and pointOfInterest */	Q3Camera_GetPlacement(camera, &placement);	Q3Point3D_Set(&placement.pointOfInterest,					(boundingBox.max.x - boundingBox.min.x) / 2.0f + boundingBox.min.x,					(boundingBox.max.y - boundingBox.min.y) / 2.0f + boundingBox.min.y,					(boundingBox.max.z - boundingBox.min.z) / 2.0f + boundingBox.min.z);	Q3Point3D_Set(&placement.cameraLocation,					placement.pointOfInterest.x,					placement.pointOfInterest.y,					((maxDimension / 2.5) / atan(fieldOfView / 2.0)));/* Using 2.5 brings camera closer */	if ((status = Q3Camera_SetPlacement(camera, &placement)) == kQ3Success) {		/* Set hither and yon */		#define	kRangeMargin	(maxDimension / 2.0)		range.hither = placement.cameraLocation.z - placement.pointOfInterest.z - kRangeMargin;		range.yon    = range.hither + maxDimension + kRangeMargin;		status = Q3Camera_SetRange(camera, &range);	}	Object_Dispose_NULL(&camera);	return status;}/* *	Document_BoundingBox * *	Computes bounding box for fModel. */TQ3Status Document_BoundingBox(			TDocumentPtr		pDocument,			TQ3BoundingBox 		*pBoundingBox){	TQ3Status		status;	TQ3ViewStatus	viewStatus;	TQ3ViewObject	aView;	if (pDocument == NULL) {		ERROR_DEBUG_STR("Document_BoundingBox: pDocument == NULL.");		return kQ3Failure;	}	if (pBoundingBox == NULL) {		ERROR_DEBUG_STR("Document_BoundingBox: pBoundingBox == NULL.");		return kQ3Failure;	}	aView = pDocument->fView;	status = kQ3Success;	if (Q3View_StartBoundingBox(aView, kQ3ComputeBoundsExact) == kQ3Failure) {		ERROR_DEBUG_STR("Document_BoundingBox: Q3View_StartBoundingBox failed.");		return kQ3Failure;	}	do {		Document_Submit_Objects(pDocument,aView);		viewStatus = Q3View_EndBoundingBox(aView, pBoundingBox);	}	while (viewStatus == kQ3ViewStatusRetraverse);	if (viewStatus != kQ3ViewStatusDone) {		ERROR_DEBUG_STR("Document_BoundingBox: viewStatus != kQ3ViewStatusDone.");		status = kQ3Failure;	}	return status;}/* *	Document_GetMaximumDimension * *	Get model's maximum diagonal dimension. */TQ3Status Document_GetMaximumDimension(	TDocumentPtr	pDocument,	float 			*pMaxDimension,	TQ3BoundingBox	*pReturnedBoundingBox){	TQ3BoundingBox		localBoundingBox,						*pBoundingBox;	TQ3Vector3D			diagonalVector;	float 				dimension;	#define			kAntiSingularity	0.0001	if ((pDocument     == NULL) ||	    (pMaxDimension == NULL)) {		ERROR_DEBUG_STR("Document_GetMaximumDimension: NULL parameter.");		return kQ3Failure;	}	pBoundingBox = (pReturnedBoundingBox != NULL) ? pReturnedBoundingBox												  : &localBoundingBox;	/* Compute length of diagonal for the bounding box */	if (Document_BoundingBox(pDocument, pBoundingBox) == kQ3Failure) {		ERROR_DEBUG_STR("Document_GetMaximumDimension: Document_BoundingBox failed.");		return kQ3Failure;	}	/*	 *  If we have a point or flat model, then the "boundingBox" would	 *  end up being a "singularity" at the location of the point.  As	 *  this bounding "box" is used in setting up the camera spec,	 *  we get bogus input into QuickDraw 3D.	 */	dimension = pBoundingBox->max.x - pBoundingBox->min.x;	if (uIsZero(dimension)) {		pBoundingBox->max.x += kAntiSingularity;		pBoundingBox->min.x -= kAntiSingularity;	}	dimension = pBoundingBox->max.y - pBoundingBox->min.y;	if (uIsZero(dimension)) {		pBoundingBox->max.y += kAntiSingularity;		pBoundingBox->min.y -= kAntiSingularity;	}	dimension = pBoundingBox->max.z - pBoundingBox->min.z;	if (uIsZero(dimension)) {		pBoundingBox->max.z += kAntiSingularity;		pBoundingBox->min.z -= kAntiSingularity;	}	Q3Point3D_Subtract(&pBoundingBox->max,					   &pBoundingBox->min,					   &diagonalVector);	*pMaxDimension = Q3Vector3D_Length(&diagonalVector);	#undef	kAntiSingularity	return kQ3Success;}