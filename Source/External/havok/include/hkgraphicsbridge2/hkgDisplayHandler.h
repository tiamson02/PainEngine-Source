/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// Havok 2.0 Visual Debug Display Manager
//

#ifndef HKG_DISPLAY_HANDLER
#define HKG_DISPLAY_HANDLER

#include <hkmath2/hkMath.h>

#include <hkutilities2/visualdebugger/hkDisplayHandler.h>
#include <hkutilities2/visualdebugger/hkStepDisplayListener.h>

static const int HKG_DISPLAY_HANDLER_DEFAULT_GRID_SPAN = 200;
static const int HKG_DISPLAY_HANDLER_DEFAULT_GRID_BASE = 10;
static const hkReal HKG_DISPLAY_HANDLER_DEFAULT_LINE_GAP = 5.0f; // in pixels

class hkDisplayGeometry;
class hkgDisplayWorld;
class hkgDisplayContext;
class hkgDisplayObject;
class hkgCamera;

class hkgKeyboard;

class hkgDisplayHandler : public hkDisplayHandler, public hkStepDisplayListener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DISPLAY)

		hkgDisplayHandler(hkgDisplayWorld* displayWorld, hkgDisplayContext* displayContext);
		virtual ~hkgDisplayHandler();

		//
		// Display handler functions
		//
		virtual Status addGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int id, int tag);
		virtual Status setGeometryColor( int color, int id, int tag);
		virtual Status updateGeometry(const hkTransform& transform, int id, int tag);
		virtual Status removeGeometry(int id, int tag);

		virtual Status updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name);

		virtual Status displayPoint(const hkVector4& position, int colour, int tag);
		virtual Status displayLine(const hkVector4& start, const hkVector4& end, int colour, int tag);
		
		virtual Status displayText(const char* text, int color, int tag);

		virtual Status displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int color, int tag);
		virtual Status displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, int color, int tag);

		virtual Status sendStatisticsDump(const char* data, int length);

		virtual Status holdImmediate();
		virtual Status step(hkReal frameTimeInMs);

		void drawImmediate(); // These is a hack for the moment...(need to fix storage of immediate mode data...)
		void clearAndSwapImmediate(); 

			///	See hkutilities2/visualdebugger/hkDisplayCallbackUtil.h
		virtual void stepDisplayCallback(hkReal frameTimeInMs);

		//
		// Functions specific to the hkg display handler (this will be removed later on hopefully...)
		//

		//
		// Mapping: "id/type" to "display object"
		//
		struct IdDisplayObjectLink
		{
			int m_id;
			hkgDisplayObject* m_do;
		};
		const hkgDisplayHandler::IdDisplayObjectLink* getDisplayObjectId(const hkgDisplayObject* displayObject);

		void clearDisplay();

		void setGrid(bool state);
		bool getGrid();
		void setGridUpAxis(int axis); // where x = 0, y = 1 and z = 2

		void addStat(hkReal stat);

		void setAxes(bool state);
		bool getAxes();

		//hkgCamera* getCameraByName(const char* name);

	protected:

			// draws a grid
		void drawGrid();
		void drawAxis();
		void drawGraph();
		void drawMenu();
	
		hkgDisplayContext* m_displayContext;
		hkgDisplayWorld* m_displayWorld;

		//
		// Mapping: "id/type" to "display object"
		//
		hkArray<IdDisplayObjectLink> m_idDoMap;
		int	m_lastHitIndex;

		//
		// Immediate mode data
		//
		struct DisplayPoint
		{
			hkVector4 m_position;
			int m_color;
		};

		struct DisplayLine
		{
			hkVector4 m_start;
			hkVector4 m_end;
			int m_color;
		};

		// double buffer
		hkArray<DisplayPoint> m_displayPoints[2];
		hkArray<DisplayLine> m_displayLines[2];

		// indexes into the buffers
		int m_loadingBuffer;
		int m_readyBuffer;

		bool m_holdImmediate;

		bool m_wantGrid;
		int m_upAxis;

		bool m_wantAxis;

		hkReal m_scale;
		int m_span;

		int m_wantStats;
		hkArray<hkReal> m_stats;
			// this is just a temporary solution
};

#endif // HKG_DISPLAY_HANDLER

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
