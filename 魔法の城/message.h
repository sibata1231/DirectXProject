#ifndef ___MESSAGE_H___
#define ___MESSAGE_H___

#include <Windows.h>
#include <string>
#include <time.h>
#include "Object.h"

#define	NAME_MAX				(32)	
#define	MSGTYPE_POSITIONINFO	 (0)				// �|�W�V�����ʒm
#define MSGTYPE_REGIST			 (1)				// �N���C�A���g�o�^
#define	MSGTYPE_ELIMINATE		 (2)				// �N���C�A���g�폜

// ����
#define		NAME		"GN31"
#define		PORT_NO		(20250)
#define		INTERVAL	   (10)
#define		MAX_PLAYER	   (10)
#define		RECT_WIDTH	  (100)
#define		RECT_HEIGHT	   (50)

// �N���C�A���g
#define		TITLE_CLIENT "UDP Screen Client"

// �T�[�o�[
#define		TITLE_SERVER "UDP Screen Server"

/*----------------------------------------------

    UDP�X�N���[���T�[�o�[���b�Z�[�W��`�w�b�_�[

------------------------------------------------*/
// ���b�Z�[�W�w�b�_�[
struct	_MSGHEADER {
    short			nType;			// ���b�Z�[�W�^�C�v 0:�|�W�V�����ʒm�@1:�N���C�A���g�o�^�@2:�N���C�A���g�폜
};

// ���b�Z�[�W�{�f�B�i�^�C�v0:�|�W�V�����ʒm�j
struct	_MSGBODY0 {
    char	name[NAME_MAX];				// ���O
    int		posX;						// �������WX
    int		posY;						// �������WY
    int     posZ;                       // �������WZ
};

// ���b�Z�[�W�{�f�B�i�^�C�v1:�o�^�j
struct	_MSGBODY1 {
    char	name[NAME_MAX];				// ���O
    int		posX;						// �������WZ
    int		posY;						// �������WY
    int     posZ;                       // �������WZ
};

// ���b�Z�[�W�{�f�B�i�^�C�v2:�폜�j
struct	_MSGBODY2 {
    char	name[NAME_MAX];				// ���O
};

// ���b�Z�[�W�̍\��
struct	_MESSAGE {
    struct	_MSGHEADER	Header;
    union {
        struct	_MSGBODY0	Body0;		// �|�W�V�����ʒm
        struct	_MSGBODY1	Body1;		// �N���C�A���g�o�^
        struct	_MSGBODY2	Body2;		// �N���C�A���g�폜
    };
};

// ����M���b�Z�[�W�\����
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
    int			posX;	  // X���W
    int			posY;	  // Y���W
    int         posZ;     // Z���W
    sockaddr_in	fromAddr; // �A�h���X
    std::string	name;	  // ���O
    Log         log;      // ���O�f�[�^
    Object*     object;   // �I�u�W�F�N�g
}ClientInfo;

#endif 