
#include "al/al.h"
#include "al/alut.h"


ALboolean LoadALData();
void SetListenerValues();
void KillALData();
void StartSource();
void StopSource();
void PauseSource();

#if 0

	alutInit(NULL, 0);
	alGetError();

	// Load the wav data.

	if(LoadALData() == AL_FALSE)
	{
	    printf("Error loading data.");
		return 0;
	}

	SetListenerValues();

	// Setup an exit procedure.
#endif
