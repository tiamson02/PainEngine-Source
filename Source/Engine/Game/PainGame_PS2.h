#ifndef PAINGAME_PS2__H
#define PAINGAME_PS2__H

#include "Engine/BaseGame.h"

class PainGame: public EngineGame
{
public:
	PainGame() {};
	virtual ~PainGame() {};

	void ProcessInput();
};

class DummyMessageHandler{
protected:
    static const int QUEUE_SIZE = 8;
    
    typedef struct{
        WPARAM  wparam;
        LPARAM  lparam;
        int     msg;
    } Msg;


    static bool m_bPausedScripts;
    static bool m_bEnabledSaveAll;
    static Msg  m_msgQueue[QUEUE_SIZE];
    static int  m_msgCount;
    //static bool m_initialized;

    //static bool init();
    static void unqueueFirstMessage();
    static int  OnScriptMessage(int msg, WPARAM wParam, LPARAM lParam);

public:
    enum{
        PKMSG_LOADLEVEL =           0x8000 + 2555,     //WM_APP + 2555
        PKMSG_SETCONTROLPROFILE =   0x8000,
        PKMSG_LOADNEXTLEVELETTE =   0x8001,
    };

    static void EnableSave(bool enable);
    static void OnPlay();
    static void OnReloadLevel();
    static void QueueHorribleWindowsMessage(int msg, WPARAM wParam, LPARAM lParam);
    static bool IsMessageWaiting();
    static void DispatchHorribleWindowsMessage();
};

#endif // PAINGAME_PS2__H
