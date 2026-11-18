

class PlayerController
{
    enum EMode
    {
        EMovePhysics    = 0,
        EMoveFly        = 1
    };

public:
    
    PlayerController();

    void Action(const DWORD action);
    void Rotate(const Vector& rotation);

    const Camera& GetCamera() const { return mCamera; }

private:
    Camera  mCamera;
    BYTE    MoveType;
};