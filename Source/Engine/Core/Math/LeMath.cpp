#include "Engine.h"

#include "Core/Math/LeMath.h"

static Matrix *initIdentityMatrix();

Matrix *GIdentityMatrix = initIdentityMatrix();


static Matrix *initIdentityMatrix(){
    Matrix *m = new Matrix;

    m->SetIdentity();

    return m;
}
