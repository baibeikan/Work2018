#ifndef __StatusBase_H__
#define __StatusBase_H__

class StatusDrive;
class StatusBase
{
public:
	StatusBase(StatusDrive* pStatusDrive)
		:m_pStatusDrive(pStatusDrive) {}
	virtual ~StatusBase(){}

public:
	// ״̬����
	virtual void OnEnter() = 0;
	// ״̬����
	virtual void OnUpdate(float deltaTime) = 0;
	// ״̬�뿪
	virtual void OnLeave() = 0;

	virtual int GetStatusID() = 0;
	// ���봦��
	virtual bool InputProcess(int message, int wParam, int lParam) { return false; }
protected:
	StatusDrive* m_pStatusDrive;
};

#endif
