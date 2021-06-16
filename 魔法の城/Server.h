#ifndef ___SERVER_H___
#define ___SERVER_H___

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include "message.h"
#include "Singleton.h"

#pragma comment(lib,"ws2_32.lib")

// スレッドに渡す構造体
typedef struct {
    SOCKET	sockNo;		// ソケット番号
    HANDLE	hEvent;		// イベントハンドル
}RecvInfo;

class Server : public Singleton<Server> {
private:
    u_short		m_portNo = PORT_NO;		// ポート番号
    SOCKET		m_mySock;				// ソケット番号
    sockaddr_in	m_myAddr;				// 自分のアドレス
    sockaddr_in	m_toAddr;				// 送信先アドレス
    sockaddr_in m_fromAddr;				// 受信アドレス
    ClientInfo  m_Client[MAX_PLAYER];   // クライアント情報
    HANDLE		m_hEvent;				// イベントハンドル
    MsgData		m_recvBuff;				// 受信バッファ
    HANDLE      m_thread;               // スレッド管理
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