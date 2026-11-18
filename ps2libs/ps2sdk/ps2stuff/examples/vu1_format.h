/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

// absolute offsets

#define kInitInfoStart		0

  // lights

typedef struct { tU32 dir; tU32 point; tU32 spot; tU32 dummy; } tLightPtrs;

#define kNumLights	        (kInitInfoStart) // x = directional, y = point, z = spot
#define kLightPointers0		(kNumLights + 1) // same as above
#define kLightPointers1		(kLightPointers0 + 1) // same as above
#define kLightPointers2		(kLightPointers1 + 1) // same as above
#define kLightPointers3		(kLightPointers2 + 1) // same as above
#define kLightPointers4		(kLightPointers3 + 1) // same as above
#define kLightPointers5		(kLightPointers4 + 1) // same as above
#define kLightPointers6		(kLightPointers5 + 1) // same as above
#define kLightPointers7		(kLightPointers6 + 1) // same as above

#define kLightAmbientOffset	0
#define kLightDiffuseOffset	1
#define kLightSpecularOffset	2
#define kLightPosOffset		3
#define kLightSpotDirOffset	4
#define kLightAttenCoeffOffset	5

#define kLightStructSize	6

#define kLight0Base		(kLightPointers7 + 1)
#define kLight1Base		(kLight0Base + kLightStructSize)
#define kLight2Base		(kLight1Base + kLightStructSize)
#define kLight3Base		(kLight2Base + kLightStructSize)
#define kLight4Base		(kLight3Base + kLightStructSize)
#define kLight5Base		(kLight4Base + kLightStructSize)
#define kLight6Base		(kLight5Base + kLightStructSize)
#define kLight7Base		(kLight6Base + kLightStructSize)

#define kGlobalAmbient		(kLight7Base + kLightStructSize)

#define kMaterialEmission	(kGlobalAmbient + 1)
#define kMaterialAmbient	(kMaterialEmission + 1)
#define kMaterialDiffuse	(kMaterialAmbient + 1)
#define kMaterialSpecular	(kMaterialDiffuse + 1)

#define kVertexXfrm		(kMaterialSpecular + 1)
#define kFixedVertToEye		(kVertexXfrm + 4)
#define kObjToWorldXfrmTrans	(kFixedVertToEye + 1)
#define kWorldToObjXfrm		(kObjToWorldXfrmTrans + 4)

#define kGifTag			(kWorldToObjXfrm + 4)

#define kDoubleBufBase		(kGifTag + 1)
#define kDoubleBufOffset	((1024 - kDoubleBufBase)/2)
#define kDoubleBufSize		kDoubleBufOffset

// buffer-relative offsets

#define kNumVertices	0
// these are temporary till I think of something better..
#define kStripADCs	(kNumVertices + 1)
#define kInputStart	(kStripADCs + 4)

#define kInputBufSize	(kDoubleBufSize/2)
#define kOutputStart	(0 + kInputBufSize)
#define kOutputBufSize	(kDoubleBufSize - kOutputStart)

// function definitions

extern "C" void vsmTriStripInit();
extern "C" void vsmTriStrip();
extern "C" void vsmTriStripClipped();

extern "C" void vsmLineStripClipped();
