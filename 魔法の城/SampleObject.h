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
        int Sx,     // スクリーンX座標
        int Sy,     // スクリーンY座標
        float fZ,   // 射影空間でのZ値（0～1）
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