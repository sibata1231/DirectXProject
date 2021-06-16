#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include "Client.h"
#include "message.h"
#include "WsaErr.hpp"

#include "imgui.h"
#include "GNPlayer.h"

//------------------------------------
// サーバIP,ユーザ名をファイルから取得
//------------------------------------
void Client::ReadSetting() {
    FILE *pFile = nullptr;
    // セッティング(サーバIPなどの情報)ファイルのオープン
    pFile = fopen("setting.txt", "r");

    if (pFile == nullptr) {
        // ファイルがなければローカルIP(172.0.0.1)を接続先サーバとして接続
        strcpy(m_szServerIP, "172.0,0,1");
        strcpy(m_szName, "名無し");
    }
    else {
        // ファイルがあれば接続先サーバIPアドレスとこのユーザ名を取得
        fgets(m_szServerIP, 16, pFile);
        fgets(m_szName, 9, pFile);
    }
    // ファイルクローズ
    fclose(pFile);
}

//------------------------------------
// UDP初期化処理
//------------------------------------
void Client::Init() {
    WSADATA		wsaData;					// ＷＳＡＳＴＡＲＴＵＰ用
    char		szHostName[256];
    HOSTENT		*lpHost;
    IN_ADDR		inAddr;

    // WinSock初期化処理
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret != 0) { // エラーチェック
        getchar();
        exit(EXIT_FAILURE);
    }

    // ソケット生成(IPv4,データグラム通信,UDPプロトコル)
    m_mySock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // 登録情報を設定(ただし名前、座標とも初期値)
    m_sendBuff.Msg.Header.nType = MSGTYPE_REGIST; // クライアント登録
    strcpy(m_sendBuff.Msg.Body1.name, m_szName);  // 名称登録
    m_sendBuff.Msg.Body1.posX = m_myClient.posX; // 座標登録(X座標)
    m_sendBuff.Msg.Body1.posY = m_myClient.posY; // 座標登録(Y座標)

    // 送信先(サーバ情報)を設定
    m_toAddr.sin_addr.S_un.S_addr = inet_addr(m_szServerIP);
    m_toAddr.sin_family = AF_INET;
    m_toAddr.sin_port = htons(PORT_NO);

    // サーバ登録送信
    sendto(m_mySock, m_sendBuff.szData, sizeof(m_sendBuff.szData), 0, (sockaddr *)&m_toAddr, sizeof(m_toAddr));

    // ローカルホスト名を取得する
    gethostname(szHostName, sizeof(szHostName));

    // ＩＰアドレス取得
    lpHost = gethostbyname(szHostName);
    memcpy(&inAddr, lpHost->h_addr_list[0], 4);

    // クライアント情報を設定
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
// UDP終了処理
//------------------------------------
void Client::Uninit() {
    // 終了送信設定
    m_sendBuff.Msg.Header.nType = MSGTYPE_ELIMINATE;
    sendto(m_mySock, m_sendBuff.szData, sizeof(m_sendBuff.szData), 0, (sockaddr *)&m_toAddr, sizeof(m_toAddr));

    // ソケットのクローズ
    closesocket(m_mySock);

    // WinSock終了処理
    WSACleanup();
}

void Client::Update() {
    // 送信情報設定
    m_sendBuff.Msg.Header.nType = MSGTYPE_POSITIONINFO; // クライアント登録
    strcpy(m_sendBuff.Msg.Body1.name, m_myClient.name.c_str());  // 名称登録
    m_sendBuff.Msg.Body0.posX = m_myClient.posX;        // 座標登録(X座標)
    m_sendBuff.Msg.Body0.posY = m_myClient.posY;        // 座標登録(Y座標)
    m_sendBuff.Msg.Body0.posZ = m_myClient.posZ;        // 座標登録(Z座標)
    // 送信
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