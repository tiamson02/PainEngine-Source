#ifndef FLEE_H_ALREADY_INCLUDED
#define FLEE_H_ALREADY_INCLUDED

#include "CAIState.h"
#include "Scripting/Scripting.h"

class CFleeState : public CAIState
{
public:
	CFleeState();
    virtual ~CFleeState();

    virtual void    onInit(CAIBrain *brain, Script & obj);
    virtual void    onUpdate(CAIBrain *brain, Script & obj);
    virtual void    onRelease(CAIBrain *brain, Script & obj);
    virtual float   evaluate(CAIBrain *brain, Script & obj);

	float	timer;
	bool	rot;
};

#endif	// FLEE_H_ALREADY_INCLUDED
