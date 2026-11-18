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


hkDefaultMonitorFactory::hkSingleMonitor::hkSingleMonitor(const char* name, hkDefaultMonitorFactory *factory )
	: hkMonitor(name), m_myFactory(factory)
{
	resetValue();
}


void hkDefaultMonitorFactory::hkSingleMonitor::addValue( float value, const void* object )
{
	if ( m_myFactory->findObject( object ) )
	{
		m_sumValue += value;
		m_addValueCount++;
	}
}

void hkDefaultMonitorFactory::hkSingleMonitor::addValue( float value, const hkArray<const void*>& involvedObjects)
{
	if ( m_myFactory->findAnyObject( involvedObjects ) )
	{
		m_sumValue += value;
		m_addValueCount++;
	}
}

float hkDefaultMonitorFactory::hkSingleMonitor::getValue()
{
	float value = 0.0f;
	if(m_addValueCount)
	{
		value = m_sumValue / m_addValueCount;
	}

	return value;
}

void hkDefaultMonitorFactory::hkSingleMonitor::print( class hkOstream& ostream )
{
	if ( m_addValueCount > 1)
	{
		ostream.printf( "%-12s: %6.4f(%i)", getName(), m_sumValue/m_addValueCount ,m_addValueCount);
	}
	else if ( m_addValueCount == 1 )
	{
		ostream.printf( "%-12s: %-6.4f", getName(), m_sumValue);
	}
	else
	{
		ostream.printf( "%-12s: ---", getName() );
	}

}

void hkDefaultMonitorFactory::hkSingleMonitor::resetValue()
{
	beginNewFrame();
}

void hkDefaultMonitorFactory::hkSingleMonitor::beginNewFrame()
{
	m_sumValue = 0.0f;
	m_addValueCount = 0;
}


//
//			Int
//

hkDefaultMonitorFactory::hkIntMonitor::hkIntMonitor(const char* name, hkDefaultMonitorFactory* factory )
: hkSingleMonitor(name, factory)
{
}

void hkDefaultMonitorFactory::hkIntMonitor::print( class hkOstream& ostream )
{
	ostream.printf( "%-12s: %6i", getName(), int(m_sumValue));
}


//
//			None
//

hkDefaultMonitorFactory::hkNoneMonitor::hkNoneMonitor( const char* name, hkDefaultMonitorFactory* factory ):
hkSingleMonitor( name, factory)
{
}

void hkDefaultMonitorFactory::hkNoneMonitor::print( class hkOstream& ostream )
{
	ostream << getName();
}


//
//				Range
//


hkDefaultMonitorFactory::hkRangeMonitor::hkRangeMonitor( const char* name,hkDefaultMonitorFactory* factory):
hkMonitor( name)
{
	m_myFactory = factory;
	resetValue();
}

void hkDefaultMonitorFactory::hkRangeMonitor::addValue( float value, const void* object )
{
	if ( m_myFactory->findObject( object ) )
	{
		if ( value > m_maxValue )
		{
			m_maxValue = value;
			m_maxValueObjects.clear();
			if ( object )
			{
				m_maxValueObjects.pushBack( object );
			}
		}

		if ( value < m_minValue )
		{
			m_minValue = value;
			m_minValueObjects.clear();
			if ( object )
			{
				m_minValueObjects.pushBack( object );
			}
		}
		m_lastValue = value;
		m_sumValue += value;
		m_addValueCount++;
	}
}

void hkDefaultMonitorFactory::hkRangeMonitor::addValue( float value, const hkArray<const void*>& involvedObjects)
{
	if ( m_myFactory->findAnyObject( involvedObjects ) )
	{
		if ( value > m_maxValue )
		{
			m_maxValue = value;
			m_maxValueObjects = involvedObjects;
		}

		if ( value < m_minValue )
		{
			m_minValue = value;
			m_minValueObjects = involvedObjects;
		}
		m_lastValue = value;
		m_sumValue += value;
		m_addValueCount++;
	}
}

float hkDefaultMonitorFactory::hkRangeMonitor::getValue()
{
	return m_lastValue;
}

void hkDefaultMonitorFactory::hkRangeMonitor::print( class hkOstream& ostream )
{
	const float avg = m_sumValue / ( ( m_addValueCount ) ? m_addValueCount: 1 );

	ostream.printf( "%-12s: %9.3f avg: %9.3f  [%8.2f .. %8.2f]", getName(), m_lastValue, avg, m_minValue, m_maxValue );

}

void hkDefaultMonitorFactory::hkRangeMonitor::resetValue()
{
	beginNewFrame();
}

void hkDefaultMonitorFactory::hkRangeMonitor::beginNewFrame()
{
	m_maxValue = -1.0e17f;
	m_minValue = 1.0e17f;
	m_sumValue = 0.0f;
	m_minValueObjects.clear();
	m_maxValueObjects.clear();
	m_addValueCount = 0;
	m_lastValue = 0.0f;
}


//
//	Persistent Frame
//
hkDefaultMonitorFactory::hkPersistentRangeMonitor::hkPersistentRangeMonitor( const char* name, hkDefaultMonitorFactory *factory ):
	 hkRangeMonitor(name, factory)
{
	resetValue();
}

void hkDefaultMonitorFactory::hkPersistentRangeMonitor::beginNewFrame()
{
}


void hkDefaultMonitorFactory::hkPersistentRangeMonitor::resetValue()
{
	hkRangeMonitor::beginNewFrame();
}




//
//				Smooth
//

hkDefaultMonitorFactory::hkSmoothMonitor::hkSmoothMonitor( const char* name, hkDefaultMonitorFactory *factory ):
	 hkSingleMonitor(name, factory)
{
	resetValue();
}


void hkDefaultMonitorFactory::hkSmoothMonitor::print( class hkOstream& ostream )
{
	float max = 0.0f;
	float min = 1e16f;
	float sum = 0.0f;
	int numVals = 0;

	for ( int f = m_historyFirstValidFrame; f != m_historyCurrentFrame; f = (f<HK_FRAMES_TO_SMOOTH-1)? f+1: 0 )
	{
		float dt = m_historySums[f];
		sum += dt;
		if ( dt > max)
		{
			max = dt;
		}
		if ( dt < min)
		{
			min = dt;
		}
		numVals++;
	}
	if ( numVals == 0)
	{
		numVals = 1;
	}

	float dt = m_sumValue;
	ostream.printf( "%-12s: %09.5 ", getName(), dt );
	ostream.printf( "avg: %09.5f", sum / numVals );
	ostream.printf( " (%09.5f ... %09.5f)", min , max );
}

void hkDefaultMonitorFactory::hkSmoothMonitor::resetValue()
{
	m_historyCurrentFrame = 0;
	m_historyFirstValidFrame = 0;
}

void hkDefaultMonitorFactory::hkSmoothMonitor::beginNewFrame()
{
	m_historySums[m_historyCurrentFrame++] = m_sumValue;

	hkSingleMonitor::beginNewFrame();


	if ( m_historyCurrentFrame >= HK_FRAMES_TO_SMOOTH )
	{
		m_historyCurrentFrame = 0;
	}

	if ( m_historyCurrentFrame == m_historyFirstValidFrame )
	{
		m_historyFirstValidFrame++;
		if ( m_historyFirstValidFrame >= HK_FRAMES_TO_SMOOTH)
		{
			m_historyFirstValidFrame = 0;
		}
	}
}

//
//	Timer
//

hkDefaultMonitorFactory::hkTimerMonitor::hkTimerMonitor( const char* name, hkDefaultMonitorFactory *factory ) 
: hkSmoothMonitor( name, factory )
{
	;
}

void hkDefaultMonitorFactory::hkTimerMonitor::print( class hkOstream& ostream )
{
	float max = 0.0f;
	float min = 1e6f;
	float sum = 0.0f;
	int numVals = 0;

	for ( int f = m_historyFirstValidFrame; f != m_historyCurrentFrame; f = (f<HK_FRAMES_TO_SMOOTH-1)? f+1: 0 )
	{
		float dt = m_historySums[f];
		sum += dt;
		if ( dt > max)
		{
			max = dt;
		}
		if ( dt < min)
		{
			min = dt;
		}
		numVals++;
	}
	if ( numVals == 0)
	{
		numVals = 1;
	}

	float avg = sum / numVals;
	//float avg = m_historySums[m_historyFirstValidFrame]; // bumpy stats

	if ( m_addValueCount>1)
	{
		ostream.printf( "%-12s(%2i): %06.3f%% = ", getName(), m_addValueCount, avg * 100 * 60 );
	}
	else
	{
		ostream.printf( "%-16s: %06.3f%% = ", getName(), avg * 100 * 60 );
	}

	float factor = 1000.0f;
	ostream.printf( "%06.4f ms (%05.3f, %05.3f)", avg * factor, min * factor, max * factor );
}




//
//
//

hkMonitor* hkDefaultMonitorFactory::createMonitor( const char* name, hkMonitor::Type hint )
{
	hkMonitor* monitor;
	switch (hint)
	{
		case hkMonitor::TYPE_SINGLE:
			monitor = new hkSingleMonitor( name, this );
			break;
		case hkMonitor::TYPE_INT:
			monitor = new hkIntMonitor( name, this );
			break;
		case hkMonitor::TYPE_RANGE:
			monitor = new hkRangeMonitor( name, this );
			break;
		case hkMonitor::TYPE_PERSISTENT_RANGE:
			monitor = new hkPersistentRangeMonitor( name, this );
			break;
		case hkMonitor::TYPE_DIRECTORY:
			monitor = new hkNoneMonitor( name, this );
			break;
		case hkMonitor::TYPE_SMOOTH:
			monitor = new hkSmoothMonitor( name, this );
			break;
		case hkMonitor::TYPE_TIMER:
			monitor = new hkTimerMonitor( name, this );
			//monitor = new hkSmoothMonitor( name, this );
			break;
		default:
			HK_ERROR( "createMonitor called with an unknown type" );
			return HK_NULL;
	}
	return monitor;
}




hkDefaultMonitorFactory::CheckObjectResult hkDefaultMonitorFactory::findObject( const void* object )
{
	if ( object == HK_NULL )
	{
		return HK_OBJECT_FOUND;
	}
	if ( m_objects.getSize() == 0 )
	{
		return HK_OBJECT_FOUND;
	}
	for (int i = 0; i < m_objects.getSize(); i++)
	{
		if ( m_objects[i] == object )
		{
			return HK_OBJECT_FOUND;
		}
	}
	return HK_OBJECT_NOT_FOUND;
}

hkDefaultMonitorFactory::CheckObjectResult hkDefaultMonitorFactory::findAnyObject( const hkArray<const void*>& involvedObjects )
{
	if ( involvedObjects.getSize() == 0 )
	{
		return HK_OBJECT_FOUND;
	}
	if ( m_objects.getSize() == 0 )
	{
		return HK_OBJECT_FOUND;
	}
	for (int j = 0; j < involvedObjects.getSize(); j++)
	{
		const void* object = involvedObjects[j];
		for (int i = 0; i < m_objects.getSize(); i++)
		{
			if ( m_objects[i] == object )
			{
				return HK_OBJECT_FOUND;
			}
		}
	}
	return HK_OBJECT_NOT_FOUND;
}

void hkDefaultMonitorFactory::setObjectsToCheckFor( hkArray<const void*> objects )
{
	m_objects.clear();
	m_objects = objects;
}

hkDefaultMonitorFactory::hkDefaultMonitorFactory()
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
