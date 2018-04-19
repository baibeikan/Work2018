#ifndef __GameLoadStatus_H__
#define __GameLoadStatus_H__
#include "GameStatus.h"

class StatusDrive;
class GameLoadStatus : public GameStatus
{
public:
	GameLoadStatus(StatusDrive* pStatusDrive);
	
	virtual ~GameLoadStatus();

public:
	// ״̬����
	virtual void OnEnter();
	// ״̬����
	virtual void OnUpdate(float deltaTime);
	// ״̬�뿪
	virtual void OnLeave();

	virtual int GetStatusID() { return GAME_LOAD_STATUS; }

private:
	static DWORD LoadResourceThread(LPVOID lParam);

protected:
	LPDIRECT9TEXTURE		m_pArrLoadTexture[LOAD_TEXTURE_MAX];
	LPDIRECT9TEXTURE		m_pArrNumTexture[NUM_TEXTURE_MAX];

	float					m_fLoadFrame;
	float					m_fLoadfps;
	int						m_nLoadCompete;
	HANDLE m_hThread;
	HANDLE m_hRcvEvent;//�¼�

	// �Ƿ�������
	bool					m_bIsLoaded;
};

#endif
