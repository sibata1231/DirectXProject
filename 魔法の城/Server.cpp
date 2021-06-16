#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <process.h>	// �}���`�X���b�h��(�u���b�L���O�֐��Ή�)
#include <time.h>		// ���O�@�\�p

#include "Server.h"
#include "message.h"
#include "WsaErr.hpp"

#include "imgui.h"

#include "GNPlayer.h"
#include "Sky.h"
//------------------------------------------------------
//
//  ��M�f�[�^���M���O
//		nRecvType   : ��M�f�[�^�^�C�v
//		fromAddr    : ���M���A�h���X
//
//------------------------------------------------------
void Server::LoggingData(int nRecvType, sockaddr_in fromAddr,ClientInfo& client) {
    switch (nRecvType) {
    case 0:
        strcpy(client.log.szRecvType, "MSGTYPE_POSITIONINFO");
        break;
    case 1:
        strcpy(client.log.szRecvType, "MSGTYPE_REGIST");
        break;
    case 2:
        strcpy(client.log.szRecvType, "MSGTYPE_ELIMINATE");
        break;
    default:
        break;
    }

    time(&client.log.present_tm);
    client.log.myTime = localtime(&client.log.present_tm);
    client.log.pszNowTime = asctime(client.log.myTime);
}

/*----------------------------------------------------------------------
    �N���C�A���g�f�[�^��M�֐�
        ����
            pRecvInfo�@�F�@recvinfo�\���̂̃A�h���X
        �߂�l
            �Ȃ�
------------------------------------------------------------------------*/
unsigned WINAPI Server::RecvClientData(void *pRecvInfo) {
    sockaddr_in		fromAddr;			// ���M���A�h���X
    int				nStatus;
    int				i;
    int				nLen;

    Server& server = Server::GetInstance();
    // �i�v���[�v
    while (1) {
        // ���M���A�h���X���̎擾
        nLen = sizeof(fromAddr);

        // �f�[�^��M����
        nStatus = recvfrom(server.m_mySock, server.m_recvBuff.szData, sizeof(m_recvBuff.szData), 0, (sockaddr *)&fromAddr, &nLen);

        // �G���[�łȂ���Ώ���
        if (nStatus != SOCKET_ERROR) {
            // ��M�������b�Z�[�W�^�C�v���Ƃɏ�������
            switch (server.m_recvBuff.Msg.Header.nType) {
            case MSGTYPE_POSITIONINFO: // �ʒu��񂪑��M����Ă�����				
                // �o�^���[�U���X�g�����[�v
                for (i = 0; i < MAX_PLAYER; i++) {
                    // �o�^�ς�(�g�p��)�̃��[�U(�z��v�f)���H
                    // ���M���h�o�A�h���X�����������H
                    // ���M���|�[�g�ԍ������������H
                    if (server.m_Client[i].status == 1 &&
                        server.m_Client[i].fromAddr.sin_addr.S_un.S_addr == fromAddr.sin_addr.S_un.S_addr &&
                        server.m_Client[i].fromAddr.sin_port == fromAddr.sin_port) {
                        // ���M����Ă������W�ōX�V����
                        server.m_Client[i].posX = server.m_recvBuff.Msg.Body0.posX;
                        server.m_Client[i].posY = server.m_recvBuff.Msg.Body0.posY;
                        server.m_Client[i].posZ = server.m_recvBuff.Msg.Body0.posZ;
                        server.m_Client[i].object->m_transform->m_position = Vector3((float)server.m_Client[i].posX, (float)server.m_Client[i].posY, (float)server.m_Client[i].posZ);
                        break;
                    }
                }
                break;
            case MSGTYPE_REGIST: // �o�^��񂪑��M����Ă�����
                // �o�^���[�U���X�g�����[�v
                for (i = 0; i < MAX_PLAYER; i++) {
                    // ���g�p�̔z��v�f���H
                    if (server.m_Client[i].status == 0) {
                        // �o�^����
                        std::string nameNo = std::to_string(i);
                        server.m_Client[i].fromAddr = fromAddr;
                        server.m_Client[i].status = 1;
                        server.m_Client[i].posX = server.m_recvBuff.Msg.Body1.posX;
                        server.m_Client[i].posY = server.m_recvBuff.Msg.Body1.posY;
                        server.m_Client[i].posZ = server.m_recvBuff.Msg.Body1.posZ;
                        server.m_Client[i].name = nameNo + server.m_recvBuff.Msg.Body1.name;
                        server.m_Client[i].object = new Object(nullptr);
                        server.m_Client[i].object->AddComponent<GNPlayer>();
                        server.m_Client[i].object->m_transform->m_name = server.m_Client[i].name;
                        break;
                    }
                }
                break;
            case MSGTYPE_ELIMINATE: // �폜��񂪑��M����Ă�����
                // �o�^���[�U���X�g�����[�v
                for (i = 0; i < MAX_PLAYER; i++) {
                    // �o�^�ς�(�g�p��)�̃��[�U(�z��v�f)���H
                    // ���M���h�o�A�h���X�����������H
                    // ���M���|�[�g�ԍ������������H
                    if (server.m_Client[i].status == 1 &&
                        server.m_Client[i].fromAddr.sin_addr.S_un.S_addr == fromAddr.sin_addr.S_un.S_addr &&
                        server.m_Client[i].fromAddr.sin_port == fromAddr.sin_port) {
                        // �폜����
                        server.m_Client[i].status = 0;
                        std::string nameNo = std::to_string(i);
                        //ObjectManager::GetInstance().Delete(server.m_Client[i].name);
                        break;
                    }
                }
                break;
            }
            // ���O�o�͂���ꍇ
            // �����Ń��M���O�̊֐����Ă�
            server.LoggingData(server.m_recvBuff.Msg.Header.nType, fromAddr, server.m_Client[i]);
        }
        else {
            // �C�x���g�t���O�Z�b�g            
        }
    }
    SetEvent(server.m_hEvent);
    return 0;
}

//------------------------------------
// UDP����������
//------------------------------------
void Server::Init() {
    WSADATA		wsaData;					// �v�r�`�r�s�`�q�s�t�o�p
    int			nStatus;					// �߂�l
    int			nErrCode;					// �\�P�b�g�̃G���[�R�[�h

    // WinSock����������
    nStatus = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (nStatus != 0) {
        nErrCode = WSAGetLastError();	// �G���[�R�[�h�̎擾
        WsaError::GetInstance().DispErr(nErrCode);				// �G���[�̕\��
        return;
    }

    // �\�P�b�g����(IPv4,�f�[�^�O�����ʐM,UDP�v���g�R��)
    m_mySock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_mySock == INVALID_SOCKET) {
        nErrCode = WSAGetLastError();	// �G���[�R�[�h�̎擾
        WsaError::GetInstance().DispErr(nErrCode);				// �G���[�̕\��
        return;
    }

    // �����̃\�P�b�g�ɂh�o�A�h���X�A�|�[�g�ԍ������蓖�Ă�(�o�C���h)
    m_myAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    m_myAddr.sin_family = AF_INET;
    m_myAddr.sin_port = htons(PORT_NO);
    bind(m_mySock, (sockaddr *)&m_myAddr, sizeof(m_myAddr));

    // �C�x���g�I�u�W�F�N�g����
    m_hEvent = CreateEvent(nullptr, true, false, nullptr);

    // �f�[�^����M���邽�߂̃X���b�h���J�n������
    m_thread = (HANDLE)_beginthreadex(nullptr,0,RecvClientData,nullptr, 2048, nullptr);

    // �w�i�`��
    Object* sky = new Object(nullptr);
    sky->AddComponent<Sky>();
}

//------------------------------------
// UDP�I������
//------------------------------------
void Server::Uninit() {
    int		nStatus;					// �߂�l
    int		nErrCode;					// �\�P�b�g�̃G���[�R�[�h

    // �\�P�b�g���N���[�Y
    nStatus = closesocket(m_mySock);
    if (nStatus != 0) {
        nErrCode = WSAGetLastError();
        WsaError::GetInstance().DispErr(nErrCode);
    }

    // �T�u�X���b�h�̏I����҂�
    WaitForMultipleObjects(1, &m_hEvent, true, INFINITE);

    // �X���b�h�̏I���҂�
    WaitForSingleObject(m_thread, INFINITE);

    // �C�x���g�n���h���̃N���[�Y	
    CloseHandle(m_hEvent);

    CloseHandle(m_thread);
    // �v�h�m�r�n�b�j�̌㏈��
    WSACleanup();

    //ObjectManager::GetInstance().AllDelete();
}

void Server::DebugLog() {
    ImGui::Begin(" [ Server ]");
    for (int i = 0; i < MAX_PLAYER; i++) {
        if (m_Client[i].status == 0) {
            continue;
        }
        ImGui::Text("Name : %s", m_Client[i].name.c_str());
        ImGui::Text("Pos[(x)%d,(y)%d,(z)%d]", m_Client[i].posX, m_Client[i].posY, m_Client[i].posZ);
        if (m_Client[i].log.myTime) {
            ImGui::Text("%d/%02d/%02d %02d:%02d:%02d %s\n",
                m_Client[i].log.myTime->tm_year + 1900,
                m_Client[i].log.myTime->tm_mon + 1,
                m_Client[i].log.myTime->tm_mday,
                m_Client[i].log.myTime->tm_hour,
                m_Client[i].log.myTime->tm_min,
                m_Client[i].log.myTime->tm_sec,
                m_Client[i].log.szRecvType);
        }
    }
    ImGui::End();
}

void Server::Update() {
    //ObjectManager::GetInstance().Update();
}

void Server::Draw() {
    //ObjectManager::GetInstance().Draw();
}