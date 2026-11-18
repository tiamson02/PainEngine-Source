#ifdef PKTV_ENABLED

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

template <typename T>
inline void PrintStat( ulong32 count, ulong32 overall, const T& t )
{
	GLog.Print( "Unknown type - add PrintStat template for it: (%u times) (%f%%)\n", count, count*100.f/overall );
}

template <>
inline void PrintStat<ulong32>( ulong32 count, ulong32 overall, const ulong32& t )
{
	GLog.Print( "%u: (%u times) (%f%%)\n", t, count, count*100.f/overall );
}

template <>
inline void PrintStat<float>( ulong32 count, ulong32 overall, const float& t )
{
	GLog.Print( "%f: (%u times) (%f%%)\n", t, count, count*100.f/overall );
}

template <>
inline void PrintStat<Vector>( ulong32 count, ulong32 overall, const Vector& t )
{
	GLog.Print( "(%f,%f,%f): (%u times) (%f%%)\n", t.X, t.Y, t.Z, count, count*100.f/overall );
}

template <>
inline void PrintStat<Quaternion>( ulong32 count, ulong32 overall, const Quaternion& t )
{
	GLog.Print( "(%f,%f,%f,%f): (%u times) (%f%%)\n", t.X, t.Y, t.Z, t.W, count, count*100.f/overall );
}

template <>
inline void PrintStat<Matrix>( ulong32 count, ulong32 overall, const Matrix& t )
{
	GLog.Print( "(%f,%f,%f,%f),(%f,%f,%f,%f),(%f,%f,%f,%f),(%f,%f,%f,%f): (%u times) (%f%%)\n",
		t.m4x4[0][0], t.m4x4[0][1], t.m4x4[0][2], t.m4x4[0][3],
		t.m4x4[1][0], t.m4x4[1][1], t.m4x4[1][2], t.m4x4[1][3],
		t.m4x4[2][0], t.m4x4[2][1], t.m4x4[2][2], t.m4x4[2][3],
		t.m4x4[3][0], t.m4x4[3][1], t.m4x4[3][2], t.m4x4[3][3],
		count, count*100.f/overall );
}

template <>
inline void PrintStat<String>( ulong32 count, ulong32 overall, const String& t )
{
	GLog.Print( "'%s': (%u times) (%f%%)\n", t.AsChar(), count, count*100.f/overall );
}

template <>
inline void PrintStat<long32>( ulong32 count, ulong32 overall, const long32& t )
{
	GLog.Print( "%d: (%u times) (%f%%)\n", t, count, count*100.f/overall );
}

template <>
inline void PrintStat<int>( ulong32 count, ulong32 overall, const int& t )
{
	GLog.Print( "%d: (%u times) (%f%%)\n", t, count, count*100.f/overall );
}

template <>
inline void PrintStat<bool>( ulong32 count, ulong32 overall, const bool& t )
{
	GLog.Print( "%s: (%u times) (%f%%)\n", t?"true":"false", count, count*100.f/overall );
}

template<class T>
class ValuesMonitor
{
public:
	ValuesMonitor( String monitorName ) : mName(monitorName) {}
	~ValuesMonitor( void ) { PrintStats(); }

	void PrintStats( void )
	{
		// Determine overall counts
		ulong32 overall = 0;
		for( int i = 0; i < Counts.Size(); ++i )
			overall += Counts[i];

		DynamicArray<int> excluded;

		if( Counts.Size() )
		{
			GLog.Print( "Monitor '%s' report: overall count %u\n", mName.AsChar(), overall );
			GLog.Print( "============================================\n" );
		}
		else
			GLog.Print( "Monitor '%s' - no counts!\n", mName.AsChar() );

		int lines = 0;
		while( Counts.Size() && lines < 10 )
		{
			if( excluded.Size() == Counts.Size() )
				break;

			// find max value
			ulong32 maxCount = 0;
			int maxIndex = -1;
			for( int i = 0; i < Counts.Size(); ++i )
				if( ( maxCount < Counts[i] ) && ( excluded.Find(i) == -1 ) )
				{
					maxCount = Counts[i];
					maxIndex = i;
				}
			excluded.Add(maxIndex);

			PrintStat<T>( maxCount, overall, Values[maxIndex] );
			++lines;
		}

		if( Counts.Size() )
			GLog.Print( "============================================\n" );
	}

	void Count( T value )
	{
		for( int i = 0; i < Counts.Size(); ++i )
			if( Values[i] == value )
			{
				++Counts[i];
				return;
			}
		Values.Add( value );
		Counts.Add( 1 );
	}

private:
	String						mName;
	DynamicArray<T>				Values;
	DynamicArray<ulong32>	Counts;
};
#endif
