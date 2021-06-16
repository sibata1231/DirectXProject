#ifndef ___RECT_TRANSFORM_H___
#define ___RECT_TRANSFORM_H___

#include "Transform.h"
class RectTransform : public Transform {    
public:
    XMFLOAT4	m_color;	    	    // �J���[
    XMFLOAT2	m_uv;					// UV�n�_
    XMFLOAT2	m_frameSize;			// UV�ő�
    XMFLOAT4X3	m_polygonVertexPos;		// ���_���W(�|���S���ό`)
    XMFLOAT4	m_polygonFrontTexFrame;	// �O�����_�n�_&�ő�(�|���S���ό`)
    XMFLOAT4	m_polygonBackTexFrame;	// ������_�n�_&�ő�(�|���S���ό`)
public:
    RectTransform();
    virtual ~RectTransform() {};
    virtual void Start();
    virtual void Debug();
};

#endif