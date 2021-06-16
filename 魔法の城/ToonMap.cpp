#include "ToonMap.h"
#include "DirectGraphics.h"
#include "Texture.h"

void ToonMap::Init() {
    //�e�N�X�`����ǂݍ��݂܂�
    CreateTextureFromFile(
        DirectGraphics::GetInstance()->GetDevice(), // D3D�f�o�C�X
        "Resources/texture/toonmap.dds",  //�e�N�X�`���̃t�@�C���p�X
        &m_srv  //�V�F�[�_�[���\�[�X�r���[
    );
}

void ToonMap::SetShaderResourceView() {
    //�s�N�Z���V�F�[�_�[�Ƀe�N�X�`����ݒ肵�܂��B
    ID3D11ShaderResourceView* srvArray[]{
            m_srv
    };
    DirectGraphics::GetInstance()->GetContext()->PSSetShaderResources(4, 1, srvArray);
}