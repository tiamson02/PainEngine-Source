/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/stopwatch/hkStopwatch.h>
#include <hkbase/debugutil/hkDefaultMonitorFactory.h>
#include <hkbase/debugutil/hkTestingMonitorFactory.h>
#include <hkbase/htl/hkAlgorithm.h>
//
//	Test Timer
//


void hkTestingMonitorFactory::hkTestingTimerMonitor::beginNewFrame()
{
	m_values.pushBack( m_sumValue );
	hkSingleMonitor::beginNewFrame();
}

void hkTestingMonitorFactory::hkTestingTimerMonitor::resetValue()
{
	hkSingleMonitor::resetValue();
	m_values.clear();
}


hkTestingMonitorFactory::hkTestingTimerMonitor::hkTestingTimerMonitor( const char* name, hkDefaultMonitorFactory *factory ) 
: hkDefaultMonitorFactory::hkSingleMonitor( name, factory )
{
}


void hkTestingMonitorFactory::hkTestingTimerMonitor::print( class hkOstream& ostream )
{
	float sum = 0.0f;
	int numVals = m_values.getSize();
	static float invTicksPerSecond = 1.0f / (float)hkStopwatch::getTicksPerSecond();

	// sort the values
	//hkSort( & m_values[0], numVals );

	for ( int f = 0; f < numVals; f++)
	{
		float dt = m_values[f];
		sum += dt;
	}

	if ( numVals == 0)
	{
		numVals = 1;
	}

	float dt = sum / numVals * invTicksPerSecond;

	ostream.printf( "%-16s (%2i): avg %06.3f%%:                times [ms]:", getName(), numVals, dt * 100.0f * 60.0f);

	for (int i = 0; i < numVals; i++)
	{
		ostream.printf(" %5.3f,", m_values[numVals - i -1] * invTicksPerSecond * 1000.0f);
	}
}




//
//
//

hkMonitor* hkTestingMonitorFactory::createMonitor( const char* name, hkMonitor::Type hint )
{
    hkMonitor* monitor = HK_NULL;
    switch (hint)
    {
        case hkMonitor::TYPE_TIMER:
            monitor = new hkTestingTimerMonitor( name, this );
            break;
        default:
            monitor = hkDefaultMonitorFactory::createMonitor( name, hint );
    }
    return monitor;
}

hkTestingMonitorFactory::hkTestingMonitorFactory()
{
}

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
