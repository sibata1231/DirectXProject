#ifndef ___MESSAGE_H___
#define ___MESSAGE_H___

#include <Windows.h>
#include <string>
#include <time.h>
#include "Object.h"

#define	NAME_MAX				(32)	
#define	MSGTYPE_POSITIONINFO	 (0)				// ポジション通知
#define MSGTYPE_REGIST			 (1)				// クライアント登録
#define	MSGTYPE_ELIMINATE		 (2)				// クライアント削除

// 共通
#define		NAME		"GN31"
#define		PORT_NO		(20250)
#define		INTERVAL	   (10)
#define		MAX_PLAYER	   (10)
#define		RECT_WIDTH	  (100)
#define		RECT_HEIGHT	   (50)

// クライアント
#define		TITLE_CLIENT "UDP Screen Client"

// サーバー
#define		TITLE_SERVER "UDP Screen Server"

/*----------------------------------------------

    UDPスクリーンサーバーメッセージ定義ヘッダー

------------------------------------------------*/
// メッセージヘッダー
struct	_MSGHEADER {
    short			nType;			// メッセージタイプ 0:ポジション通知　1:クライアント登録　2:クライアント削除
};

// メッセージボディ（タイプ0:ポジション通知）
struct	_MSGBODY0 {
    char	name[NAME_MAX];				// 名前
    int		posX;						// 初期座標X
    int		posY;						// 初期座標Y
    int     posZ;                       // 初期座標Z
};

// メッセージボディ（タイプ1:登録）
struct	_MSGBODY1 {
    char	name[NAME_MAX];				// 名前
    int		posX;						// 初期座標Z
    int		posY;						// 初期座標Y
    int     posZ;                       // 初期座標Z
};

// メッセージボディ（タイプ2:削除）
struct	_MSGBODY2 {
    char	name[NAME_MAX];				// 名前
};

// メッセージの構造
struct	_MESSAGE {
    struct	_MSGHEADER	Header;
    union {
        struct	_MSGBODY0	Body0;		// ポジション通知
        struct	_MSGBODY1	Body1;		// クライアント登録
        struct	_MSGBODY2	Body2;		// クライアント削除
    };
};

// 送受信メッセージ構造体
struct	MsgData {
    union {
        struct	_MESSAGE  Msg;
        char	szData[sizeof(_MESSAGE)];
    };
};

struct Log {
    time_t	present_tm;
    struct	tm *myTime;
    char*	pszNowTime;
    char	szRecvType[128];
};
typedef struct {
    int			status;	  // 0: not use  1: use
    int			posX;	  // X座標
    int			posY;	  // Y座標
    int         posZ;     // Z座標
    sockaddr_in	fromAddr; // アドレス
    std::string	name;	  // 名前
    Log         log;      // ログデータ
    Object*     object;   // オブジェクト
}ClientInfo;

#endif 