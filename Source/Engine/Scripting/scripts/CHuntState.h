#ifndef HUNT_H_ALREADY_INCLUDED
#define HUNT_H_ALREADY_INCLUDED

#include "CAIState.h"
#include "Scripting/Scripting.h"

class CHuntState : public CAIState
{
public:
	CHuntState();
    virtual ~CHuntState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);

private:
	float	huntTime;
	int		walk;
	bool	active;
	int		substate;
	int		delayTimer;
};

#endif	// HUNT_H_ALREADY_INCLUDED
