#ifndef __GameIdleStatus_H__
#define __GameIdleStatus_H__
#include "GameStatus.h"

class GameIdleStatus : public GameStatus
{
public:
	GameIdleStatus(StatusDrive* pStatusDrive)
		:GameStatus(pStatusDrive)
	{
	}
	
	virtual ~GameIdleStatus(){}

public:
	// ״̬����
	virtual void OnEnter();
	// ״̬����
	virtual void OnUpdate(float deltaTime);
	// ״̬�뿪
	virtual void OnLeave();

	virtual int GetStatusID() { return GAME_IDLE_STATUS;  }


protected:

};

#endif
