#ifndef ___SERVER_H___
#define ___SERVER_H___

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include "message.h"
#include "Singleton.h"

#pragma comment(lib,"ws2_32.lib")

// �X���b�h�ɓn���\����
typedef struct {
    SOCKET	sockNo;		// �\�P�b�g�ԍ�
    HANDLE	hEvent;		// �C�x���g�n���h��
}RecvInfo;

class Server : public Singleton<Server> {
private:
    u_short		m_portNo = PORT_NO;		// �|�[�g�ԍ�
    SOCKET		m_mySock;				// �\�P�b�g�ԍ�
    sockaddr_in	m_myAddr;				// �����̃A�h���X
    sockaddr_in	m_toAddr;				// ���M��A�h���X
    sockaddr_in m_fromAddr;				// ��M�A�h���X
    ClientInfo  m_Client[MAX_PLAYER];   // �N���C�A���g���
    HANDLE		m_hEvent;				// �C�x���g�n���h��
    MsgData		m_recvBuff;				// ��M�o�b�t�@
    HANDLE      m_thread;               // �X���b�h�Ǘ�
private:
    void LoggingData(int nRecvType, sockaddr_in fromAddr, ClientInfo& client);
public:
    void Init();
    void Uninit();
    static unsigned WINAPI RecvClientData(void *pRecvInfo);
    void DebugLog();
    void Update();
    void Draw();
};

#endif