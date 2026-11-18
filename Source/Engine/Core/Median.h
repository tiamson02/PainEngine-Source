
// #pragma once
#ifndef _MEDIAN_
#define _MEDIAN_


template <class T_Type, const int MedianSize=10>
class Median
{
public:
	Median( void ) : mLastResultsCount(0) {};

	void AddResult( T_Type result )
	{
		int i;
		if( mLastResultsCount == MedianSize )
		{
			T_Type resultToKill = mLastResults[0];
			for( i = 1; i < MedianSize; ++i )
				mLastResults[i-1] = mLastResults[i];
			mLastResults[mLastResultsCount-1] = result;

			// Remove oldest result
			for( i = 0; i < mLastResultsCount; ++i )
				if( mSortedResults[i] == resultToKill )
					break;

			for( int j = i+1; j < mLastResultsCount; ++j )
				mSortedResults[j-1] = mSortedResults[j];
		}
		else
		{
			mLastResults[mLastResultsCount] = result;
			mLastResultsCount++;
		}

		// Now mSortedResults are valid to mLastResultsCount-1

		// Find a place to insert new result
		for( i = 0; i < mLastResultsCount-1; ++i )
			if( mSortedResults[i] > result )
				break;

		// Make a place moving higher results
		for( int j = mLastResultsCount-1; j > i; --j )
			mSortedResults[j] = mSortedResults[j-1];

		// Insert new result
		mSortedResults[i] = result;
	}

	void ShiftResults( T_Type shift )
	{
		for( int i = 0; i < mLastResultsCount; ++i )
		{
			mLastResults[i] += shift;
			mSortedResults[i] += shift;
		}
	}

	void Clear( void )
	{
		if( mLastResultsCount )
		{
			mLastResults[0] = mLastResults[mLastResultsCount-1];
			mSortedResults[0] = mLastResults[0];
			mLastResultsCount = 1;
		}
	}

	T_Type GetMedian( void )
	{
		return (mLastResultsCount%2==0)?(mSortedResults[mLastResultsCount/2-1]+mSortedResults[mLastResultsCount/2])/2:mSortedResults[mLastResultsCount/2];
	}

	T_Type GetAverage( int fromHowMany )
	{
		if( fromHowMany > mLastResultsCount ) fromHowMany = mLastResultsCount;
		if( !fromHowMany )
			return 0;
		int first = (mLastResultsCount - fromHowMany)/2;

		T_Type overall = 0;
		for( int i = 0; i < fromHowMany; ++i )
			overall += mSortedResults[i+first];
		overall /= fromHowMany;
		return overall;
	}

	T_Type Correct( T_Type result )
	{
		AddResult( result );
		return GetMedian();
	}

private:
	int mLastResultsCount;
	T_Type mLastResults[MedianSize];
	T_Type mSortedResults[MedianSize];
};

#endif //_MEDIAN_

