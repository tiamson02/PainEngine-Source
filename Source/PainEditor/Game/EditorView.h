#ifndef EDITORVIEW___H
#define EDITORVIEW___H

#include "Render/View.h"

class EditorView : public View
{
public:
	virtual HWND GetActiveWindow( void );
	virtual HWND GetTopmostWindow( void );
};

#endif
