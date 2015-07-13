#ifndef KENNEY_MONSTER
#define KENNEY_MONSTER

#include "Actor.h"
#include "MonsterDefine.h"
#include "Scene.h"
#include "Player.h"

class CMonster : public CActor
{
public:
	CMonster(std::vector<RECT> pRole, D3DXVECTOR3 vecPos, LPD3DXSPRITE pSprite, 
		LPDIRECT3DTEXTURE9 pTex, LPD3DXLINE pLine);
	virtual ~CMonster(void);
public:
	virtual void Destroy();
	virtual void UpdataFrame();
	virtual bool TickFSM(float fElapsedTime);
	inline void UpdateGridY(){		
		m_iRow = (int)m_vecPos.y/BLOCKLENGTH;
		m_iRowFoot = (int)(m_vecPos.y + m_vecSize.y)/BLOCKLENGTH;
		m_iRowV = (int)(m_vecPos.y + (m_bMirror ?  0.f: m_vecSize.y ))/BLOCKLENGTH;
		m_rect.top	= (long)(m_vecPos.y);
		m_rect.bottom = (long)(m_rect.top + m_vecSize.y); 
	}
	inline void UpdateGridX(){
		m_iCol = (int)(m_vecPos.x + (m_bMirror ? m_vecSize.x : 0.f ))/BLOCKLENGTH;
		m_iColV = (int) m_vecPos.x/BLOCKLENGTH;
		m_rect.left = (long)(m_vecPos.x);
		m_rect.right = (long)(m_rect.left + m_vecSize.x);
	}
protected:
	bool DetectionCollision(RECT intersect, const RECT MonsterRect, const RECT* PlayerRect);
	bool IntersectRect(RECT* intersect, const RECT* MonsterRect, const RECT* PlayerRect);
	virtual void Death();
protected:
	inline void SetElt(float Evaluation){	m_fEvaluation = Evaluation;	}
	inline void Mirror(float fElapseTime)
	{
		/*m_fRestTime = 6.f;*/
		m_fDefSpe = 0.f;
		if(m_eCurState & EAS_Death){
			return;
		}else{
		if(m_bStopFrames){
			SetActorState(EAS_Stand);
			}
		}
		m_fElapsedTime += fElapseTime;
		if(m_fElapsedTime > m_fMirrorTime)
		{
			m_vecPos -= m_vecVel * m_fEvaluation * fElapseTime;
			m_fDefSpe = m_fEvaluation;
			m_vecVel = -m_vecVel;
			m_bMirror = !m_bMirror;
			m_fElapsedTime = 0.f;
			m_bTimeDetection = true;
			SetElt(m_fDefSpe);
			if(m_eCurState & EAS_Death){
				return;
			}else{
				if(m_bStopFrames){SetActorState(EAS_Duck);}
			}
		}
	}
protected:
	bool m_bDeathTally;
	bool m_bStopFrames;
	bool m_bTimeDetection;
	CPlayer* m_pPlayer;
	RECT m_rect,m_intersect;
	std::vector<int*> m_vecPxl;
	int m_iRow;
	int m_iRowFoot;
	int m_iRowV;
	int m_iCol;
	int m_iColV;
	float m_fElapsedTime,m_fMirrorTime,m_fRestTime,m_fEvaluation;
	EMonsterAction m_eMonsterFrame;
};
#endif
