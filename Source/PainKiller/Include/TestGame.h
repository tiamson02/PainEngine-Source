#include "Engine.h"
#include "Engine/PCFSystem.h"
#include "Engine/BaseGame.h"

class TestGame : public BaseGame
{
public:
    TestGame();
    ~TestGame();

    BOOL Initialize();
    void Close();
    void Tick(float timeDelta);

private:
    FWorld   mWorld;
};
