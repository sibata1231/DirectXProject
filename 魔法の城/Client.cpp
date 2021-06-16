#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include "Client.h"
#include "message.h"
#include "WsaErr.hpp"

#include "imgui.h"
#include "GNPlayer.h"

//------------------------------------
// �T�[�oIP,���[�U�����t�@�C������擾
//------------------------------------
void Client::ReadSetting() {
    FILE *pFile = nullptr;
    // �Z�b�e�B���O(�T�[�oIP�Ȃǂ̏��)�t�@�C���̃I�[�v��
    pFile = fopen("setting.txt", "r");

    if (pFile == nullptr) {
        // �t�@�C�����Ȃ���΃��[�J��IP(172.0.0.1)��ڑ���T�[�o�Ƃ��Đڑ�
        strcpy(m_szServerIP, "172.0,0,1");
        strcpy(m_szName, "������");
    }
    else {
        // �t�@�C��������ΐڑ���T�[�oIP�A�h���X�Ƃ��̃��[�U�����擾
        fgets(m_szServerIP, 16, pFile);
        fgets(m_szName, 9, pFile);
    }
    // �t�@�C���N���[�Y
    fclose(pFile);
}

//------------------------------------
// UDP����������
//------------------------------------
void Client::Init() {
    WSADATA		wsaData;					// �v�r�`�r�s�`�q�s�t�o�p
    char		szHostName[256];
    HOSTENT		*lpHost;
    IN_ADDR		inAddr;

    // WinSock����������
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret != 0) { // �G���[�`�F�b�N
        getchar();
        exit(EXIT_FAILURE);
    }

    // �\�P�b�g����(IPv4,�f�[�^�O�����ʐM,UDP�v���g�R��)
    m_mySock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // �o�^����ݒ�(���������O�A���W�Ƃ������l)
    m_sendBuff.Msg.Header.nType = MSGTYPE_REGIST; // �N���C�A���g�o�^
    strcpy(m_sendBuff.Msg.Body1.name, m_szName);  // ���̓o�^
    m_sendBuff.Msg.Body1.posX = m_myClient.posX; // ���W�o�^(X���W)
    m_sendBuff.Msg.Body1.posY = m_myClient.posY; // ���W�o�^(Y���W)

    // ���M��(�T�[�o���)��ݒ�
    m_toAddr.sin_addr.S_un.S_addr = inet_addr(m_szServerIP);
    m_toAddr.sin_family = AF_INET;
    m_toAddr.sin_port = htons(PORT_NO);

    // �T�[�o�o�^���M
    sendto(m_mySock, m_sendBuff.szData, sizeof(m_sendBuff.szData), 0, (sockaddr *)&m_toAddr, sizeof(m_toAddr));

    // ���[�J���z�X�g�����擾����
    gethostname(szHostName, sizeof(szHostName));

    // �h�o�A�h���X�擾
    lpHost = gethostbyname(szHostName);
    memcpy(&inAddr, lpHost->h_addr_list[0], 4);

    // �N���C�A���g����ݒ�
    m_myClient.fromAddr.sin_addr.S_un.S_addr = inAddr.S_un.S_addr;
    m_myClient.fromAddr.sin_family = AF_INET;
    m_myClient.status = 1;
    m_myClient.name = std::string(m_szName);
    m_myClient.posX =    0;
    m_myClient.posY = -150;
    m_myClient.posZ =  400;

    Object *player = new Object(nullptr);
    player->AddComponent<GNPlayer>();
}

//------------------------------------
// UDP�I������
//------------------------------------
void Client::Uninit() {
    // �I�����M�ݒ�
    m_sendBuff.Msg.Header.nType = MSGTYPE_ELIMINATE;
    sendto(m_mySock, m_sendBuff.szData, sizeof(m_sendBuff.szData), 0, (sockaddr *)&m_toAddr, sizeof(m_toAddr));

    // �\�P�b�g�̃N���[�Y
    closesocket(m_mySock);

    // WinSock�I������
    WSACleanup();
}

void Client::Update() {
    // ���M���ݒ�
    m_sendBuff.Msg.Header.nType = MSGTYPE_POSITIONINFO; // �N���C�A���g�o�^
    strcpy(m_sendBuff.Msg.Body1.name, m_myClient.name.c_str());  // ���̓o�^
    m_sendBuff.Msg.Body0.posX = m_myClient.posX;        // ���W�o�^(X���W)
    m_sendBuff.Msg.Body0.posY = m_myClient.posY;        // ���W�o�^(Y���W)
    m_sendBuff.Msg.Body0.posZ = m_myClient.posZ;        // ���W�o�^(Z���W)
    // ���M
    sendto(m_mySock, m_sendBuff.szData, sizeof(m_sendBuff.szData), 0, (sockaddr *)&m_toAddr, sizeof(m_toAddr));
}

void Client::DebugLog() {
    ImGui::Begin("[ Client ]");
    ImGui::Text("TestUdpClient");
    ImGui::Text("Name : %s [(x)%d,(y)%d]", m_myClient.name.c_str(), m_myClient.posX, m_myClient.posY);
    ImGui::DragInt("< PosX >", &m_myClient.posX);
    ImGui::DragInt("< PosY >", &m_myClient.posY);
    ImGui::DragInt("< PosZ >", &m_myClient.posZ);
    ImGui::End();
}

void Client::Draw() {

}