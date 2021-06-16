#ifndef ___SAMPLE_OBJECT_H___
#define ___SAMPLE_OBJECT_H___

#include "model.h"
#include "GraphicsUtility.h"

class SampleObject : public Component {
public:
    SampleObject()          {};
    virtual ~SampleObject() {};
    void Start();
    void Update();
    void Draw();
    XMVECTOR* CalcScreenToWorld(
        XMVECTOR* pout,
        int Sx,     // �X�N���[��X���W
        int Sy,     // �X�N���[��Y���W
        float fZ,   // �ˉe��Ԃł�Z�l�i0�`1�j
        int Screen_w,
        int Screen_h,
        XMMATRIX* View,
        XMMATRIX* Prj);

    XMVECTOR* CalcScreenToXZ(
        XMVECTOR* pout,
        int Sx,
        int Sy,
        int Screen_w,
        int Screen_h,
        XMMATRIX* View,
        XMMATRIX* Prj);
};

#endif