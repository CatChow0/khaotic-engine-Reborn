#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "object.h"
#include "math.h"

class Physics : public Object
{
public:
    Physics();
    explicit Physics(const Physics&); // Use explicit to avoid implicit conversion
    ~Physics();

    XMVECTOR GetGravity() const; // Get the gravity value
    void SetGravity(XMVECTOR gravity); // Define the gravity value
    void ApplyGravity(Object*, float); // Apply gravity to an object
    void AddForce(Object*, XMVECTOR);
    bool IsColliding(Object*, Object*);
    bool CubesOverlap(Object*, Object*);
    bool SpheresOverlap(Object*, Object*);
    bool SphereCubeOverlap(Object*, Object*);

private:
    XMVECTOR m_gravity;
};

#endif