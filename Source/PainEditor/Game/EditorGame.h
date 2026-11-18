#ifndef EDITORGAME___H
#define EDITORGAME___H

#include "Engine/BaseGame.h"

class EditorGame: public EngineGame
{
public:
	EditorGame() {};
	virtual ~EditorGame() {};

	// przeniesc do skryptow:
    void ProcessInput();
};

/*
#ifdef TRANSGAMING_WINDOWS

class DummyMessageHandler 
{
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

    //Specific message handler functions
    static int  OnScriptMessage(int msg, WPARAM wParam, LPARAM lParam);

public:
    enum{
        PKMSG_LOADLEVEL =           0x8000 + 2555,     //WM_APP + 2555
        PKMSG_SETCONTROLPROFILE =   0x8000,
    };

    static void EnableSave(bool enable);
    static void OnPlay();
    static void OnReloadLevel();
    static void QueueHorribleWindowsMessage(int msg, WPARAM wParam, LPARAM lParam);
    static bool IsMessageWaiting();
    static void DispatchHorribleWindowsMessage();
};

#endif // TRANSGAMING_WINDOWS
*/
#endif