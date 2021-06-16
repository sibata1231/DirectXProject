#ifndef ___CLIENT_H___
#define ___CLIENT_H___

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include "message.h"
#include "Singleton.h"

#pragma comment(lib,"ws2_32.lib")

class Client : public Singleton<Client> {
private:
    u_short		m_portNo = PORT_NO;	// ポート番号
    SOCKET		m_mySock;		    // ソケット番号
    sockaddr_in	m_myAddr;		    // 自分のアドレス
    sockaddr_in	m_toAddr;		    // 送信先アドレス
    HANDLE		m_hEvent;		    // イベントハンドル
    MsgData		m_sendBuff;		    // 送信バッファ
    char		m_szServerIP[16];	// サーバIP格納用
    char		m_szName[9];		// ユーザ名格納用
public:
    ClientInfo	m_myClient;         // クライアント情報
public:
    void ReadSetting();
    void Init();
    void Uninit();
    void Update();
    void Draw();
    void DebugLog();
};

#endif