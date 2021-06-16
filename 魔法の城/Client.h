#ifndef ___CLIENT_H___
#define ___CLIENT_H___

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include "message.h"
#include "Singleton.h"

#pragma comment(lib,"ws2_32.lib")

class Client : public Singleton<Client> {
private:
    u_short		m_portNo = PORT_NO;	// �|�[�g�ԍ�
    SOCKET		m_mySock;		    // �\�P�b�g�ԍ�
    sockaddr_in	m_myAddr;		    // �����̃A�h���X
    sockaddr_in	m_toAddr;		    // ���M��A�h���X
    HANDLE		m_hEvent;		    // �C�x���g�n���h��
    MsgData		m_sendBuff;		    // ���M�o�b�t�@
    char		m_szServerIP[16];	// �T�[�oIP�i�[�p
    char		m_szName[9];		// ���[�U���i�[�p
public:
    ClientInfo	m_myClient;         // �N���C�A���g���
public:
    void ReadSetting();
    void Init();
    void Uninit();
    void Update();
    void Draw();
    void DebugLog();
};

#endif