/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEMOFRAMEWORK2_GAME_H
#define HK_DEMOFRAMEWORK2_GAME_H

class hkGame : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO);

		enum Result
		{
			GAME_OK,
			GAME_PAUSED,
			GAME_STOP,
			GAME_RESTART,
			GAME_ERROR
		};

		struct hkParameter
		{
			hkParameter(const char* name, hkReal startVal, hkReal minVal, hkReal maxVal, hkReal step, const char* help="")
				: m_name(name), m_help(help), m_value(startVal), m_minValue(minVal), m_maxValue(maxVal), m_step(step)
			{
			}
			const char* m_name;
			const char* m_help;
			hkReal m_value;
			hkReal m_minValue;
			hkReal m_maxValue;
			hkReal m_step;
		};
		
			/// Run a single step of the game.
		virtual Result stepGame() = 0;

			/// What tweakable paramters does this game have
		hkArray<hkParameter>& getParameters() { return m_parameters; }

		const hkString& getError() { return m_error; }

			/// Mouse has been pressed.
		virtual void mouseDown() {}
			/// Mouse has been released.
		virtual void mouseUp() {}
			/// Mouse has been dragged.
		virtual void mouseDrag() {}

	protected:

		hkArray<hkParameter> m_parameters;
		hkString m_error;
};

#endif // HK_DEMOFRAMEWORK2_GAME_H

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
