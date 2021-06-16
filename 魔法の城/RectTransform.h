#ifndef ___RECT_TRANSFORM_H___
#define ___RECT_TRANSFORM_H___

#include "Transform.h"
class RectTransform : public Transform {    
public:
    XMFLOAT4	m_color;	    	    // カラー
    XMFLOAT2	m_uv;					// UV始点
    XMFLOAT2	m_frameSize;			// UV最大
    XMFLOAT4X3	m_polygonVertexPos;		// 頂点座標(ポリゴン変形)
    XMFLOAT4	m_polygonFrontTexFrame;	// 前方頂点始点&最大(ポリゴン変形)
    XMFLOAT4	m_polygonBackTexFrame;	// 後方頂点始点&最大(ポリゴン変形)
public:
    RectTransform();
    virtual ~RectTransform() {};
    virtual void Start();
    virtual void Debug();
};

#endif