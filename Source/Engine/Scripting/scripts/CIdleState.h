#ifndef IDLE_H_ALREADY_INCLUDED
#define IDLE_H_ALREADY_INCLUDED

#include "CAIState.h"
#include "Core/Math/Vector.h"

class CIdleState : public CAIState
{
private:
    float   lastTimeAmbientSound;
    float   lastAmbient;
    int     delay;
	bool    GuardStill;
	int     timeChangeStillToFalse;
	bool    OLDonlyWPmove;
	bool    OLDdoNotUseWP;
    bool    walked;
    Vector  newDestination;
    bool    newDestinationSet;

public:
	CIdleState();
    virtual ~CIdleState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);
};

#endif	// IDLE_H_ALREADY_INCLUDED
