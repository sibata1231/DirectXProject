#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <process.h>	// マルチスレッド化(ブロッキング関数対応)
#include <time.h>		// ログ機能用

#include "Server.h"
#include "message.h"
#include "WsaErr.hpp"

#include "imgui.h"

#include "GNPlayer.h"
#include "Sky.h"
//------------------------------------------------------
//
//  受信データロギング
//		nRecvType   : 受信データタイプ
//		fromAddr    : 送信元アドレス
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
    クライアントデータ受信関数
        引数
            pRecvInfo　：　recvinfo構造体のアドレス
        戻り値
            なし
------------------------------------------------------------------------*/
unsigned WINAPI Server::RecvClientData(void *pRecvInfo) {
    sockaddr_in		fromAddr;			// 送信元アドレス
    int				nStatus;
    int				i;
    int				nLen;

    Server& server = Server::GetInstance();
    // 永久ループ
    while (1) {
        // 送信元アドレス長の取得
        nLen = sizeof(fromAddr);

        // データ受信処理
        nStatus = recvfrom(server.m_mySock, server.m_recvBuff.szData, sizeof(m_recvBuff.szData), 0, (sockaddr *)&fromAddr, &nLen);

        // エラーでなければ処理
        if (nStatus != SOCKET_ERROR) {
            // 受信したメッセージタイプごとに処理する
            switch (server.m_recvBuff.Msg.Header.nType) {
            case MSGTYPE_POSITIONINFO: // 位置情報が送信されてきた時				
                // 登録ユーザリスト分ループ
                for (i = 0; i < MAX_PLAYER; i++) {
                    // 登録済み(使用中)のユーザ(配列要素)か？
                    // 送信元ＩＰアドレスが等しいか？
                    // 送信元ポート番号が等しいか？
                    if (server.m_Client[i].status == 1 &&
                        server.m_Client[i].fromAddr.sin_addr.S_un.S_addr == fromAddr.sin_addr.S_un.S_addr &&
                        server.m_Client[i].fromAddr.sin_port == fromAddr.sin_port) {
                        // 送信されてきた座標で更新する
                        server.m_Client[i].posX = server.m_recvBuff.Msg.Body0.posX;
                        server.m_Client[i].posY = server.m_recvBuff.Msg.Body0.posY;
                        server.m_Client[i].posZ = server.m_recvBuff.Msg.Body0.posZ;
                        server.m_Client[i].object->m_transform->m_position = Vector3((float)server.m_Client[i].posX, (float)server.m_Client[i].posY, (float)server.m_Client[i].posZ);
                        break;
                    }
                }
                break;
            case MSGTYPE_REGIST: // 登録情報が送信されてきた時
                // 登録ユーザリスト分ループ
                for (i = 0; i < MAX_PLAYER; i++) {
                    // 未使用の配列要素か？
                    if (server.m_Client[i].status == 0) {
                        // 登録処理
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
            case MSGTYPE_ELIMINATE: // 削除情報が送信されてきた時
                // 登録ユーザリスト分ループ
                for (i = 0; i < MAX_PLAYER; i++) {
                    // 登録済み(使用中)のユーザ(配列要素)か？
                    // 送信元ＩＰアドレスが等しいか？
                    // 送信元ポート番号が等しいか？
                    if (server.m_Client[i].status == 1 &&
                        server.m_Client[i].fromAddr.sin_addr.S_un.S_addr == fromAddr.sin_addr.S_un.S_addr &&
                        server.m_Client[i].fromAddr.sin_port == fromAddr.sin_port) {
                        // 削除処理
                        server.m_Client[i].status = 0;
                        std::string nameNo = std::to_string(i);
                        //ObjectManager::GetInstance().Delete(server.m_Client[i].name);
                        break;
                    }
                }
                break;
            }
            // ログ出力する場合
            // ここでロギングの関数を呼ぶ
            server.LoggingData(server.m_recvBuff.Msg.Header.nType, fromAddr, server.m_Client[i]);
        }
        else {
            // イベントフラグセット            
        }
    }
    SetEvent(server.m_hEvent);
    return 0;
}

//------------------------------------
// UDP初期化処理
//------------------------------------
void Server::Init() {
    WSADATA		wsaData;					// ＷＳＡＳＴＡＲＴＵＰ用
    int			nStatus;					// 戻り値
    int			nErrCode;					// ソケットのエラーコード

    // WinSock初期化処理
    nStatus = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (nStatus != 0) {
        nErrCode = WSAGetLastError();	// エラーコードの取得
        WsaError::GetInstance().DispErr(nErrCode);				// エラーの表示
        return;
    }

    // ソケット生成(IPv4,データグラム通信,UDPプロトコル)
    m_mySock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_mySock == INVALID_SOCKET) {
        nErrCode = WSAGetLastError();	// エラーコードの取得
        WsaError::GetInstance().DispErr(nErrCode);				// エラーの表示
        return;
    }

    // 自分のソケットにＩＰアドレス、ポート番号を割り当てる(バインド)
    m_myAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    m_myAddr.sin_family = AF_INET;
    m_myAddr.sin_port = htons(PORT_NO);
    bind(m_mySock, (sockaddr *)&m_myAddr, sizeof(m_myAddr));

    // イベントオブジェクト生成
    m_hEvent = CreateEvent(nullptr, true, false, nullptr);

    // データを受信するためのスレッドを開始させる
    m_thread = (HANDLE)_beginthreadex(nullptr,0,RecvClientData,nullptr, 2048, nullptr);

    // 背景描画
    Object* sky = new Object(nullptr);
    sky->AddComponent<Sky>();
}

//------------------------------------
// UDP終了処理
//------------------------------------
void Server::Uninit() {
    int		nStatus;					// 戻り値
    int		nErrCode;					// ソケットのエラーコード

    // ソケットをクローズ
    nStatus = closesocket(m_mySock);
    if (nStatus != 0) {
        nErrCode = WSAGetLastError();
        WsaError::GetInstance().DispErr(nErrCode);
    }

    // サブスレッドの終了を待つ
    WaitForMultipleObjects(1, &m_hEvent, true, INFINITE);

    // スレッドの終了待ち
    WaitForSingleObject(m_thread, INFINITE);

    // イベントハンドルのクローズ	
    CloseHandle(m_hEvent);

    CloseHandle(m_thread);
    // ＷＩＮＳＯＣＫの後処理
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