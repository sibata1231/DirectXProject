#ifndef ___TOON_MAP___
#define ___TOON_MAP___

#include "SystemMain.h"

class ToonMap {
public:
    ToonMap() {}
    ~ToonMap() {}

    //������
    void Init();
    //�e�N�X�`�����s�N�Z���V�F�[�_�[�ɐݒ肷��A���f�����h���[���鎞�ɌĂ�ł�������
    void SetShaderResourceView();
private:
    ID3D11ShaderResourceView* m_srv = nullptr;
};

#endif
