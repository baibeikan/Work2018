#ifndef __GameRunStatus_H__
#define __GameRunStatus_H__
#include "GameStatus.h"

class GameRunStatus : public GameStatus
{
public:
	GameRunStatus(StatusDrive* pStatusDrive);

	
	virtual ~GameRunStatus(){}

public:
	// ״̬����
	virtual void OnEnter();
	// ״̬����
	virtual void OnUpdate(float deltaTime);
	// ״̬�뿪
	virtual void OnLeave();

	virtual int GetStatusID() { return GAME_RUN_STATUS; }

	virtual bool InputProcess(int message, int wParam, int lParam);

protected:
	LPDIRECT9TEXTURE		m_pArrTexture[TEXTURE_MAX];
	LPDIRECT9TEXTURE		m_pArrNumTexture[NUM_TEXTURE_MAX];
	// ��ǰ�Ƕ�
	float m_fCurrentAngle;
	// ���ٶ�
	float m_fAngleSpeed;
	// ���Ƶ�ǰ֡
	int m_nCurFrame;

};

#endif
