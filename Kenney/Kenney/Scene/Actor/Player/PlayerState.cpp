#include "stdafx.h"
#include "Player.h"
#include "Scene.h"

bool CPlayer::TickFSM(float fElapsedTime)
{
	bool bDuck((m_eCurState & EAS_Duck)>0);
	// Gravity
	if (Coll(m_iRow+1,m_iColMid) > EBT_BlockMin || 
		Coll(m_iRow+2,m_iColMid) > EBT_BlockMin) {
			SubtractActorState(EAS_Fall);
	} else AddActorState(EAS_Fall);

	if (m_bUp && (Coll(m_iRow,m_iColMid) & EBT_Door || 
		Coll(m_iRow+1,m_iColMid) & EBT_Door)) 
			return true;
	if ((m_bUp || bDuck) && 
		(Coll(m_iRowFoot,m_iColMid) & EBT_Ladder)) 
			AddActorState(EAS_Climb);
	if (Coll(m_iRowFoot,m_iCol) & EBT_Water || 
		Coll(m_iRowFoot,m_iColRight) & EBT_Water) 
		AddActorState(EAS_Swim);

	bool bIgnore(false);
	do {
		if (m_eCurState & EAS_Death) 
		{
			UpdateGrid();
			bIgnore = true;
			break;
		}
		if (m_eCurState & EAS_Hurt)
		{
			break;
		}
		if (m_eCurState & EAS_Attack)
		{
			if (m_vecVel.y > 0.f) {
				SubtractActorState(EAS_Attack); 
				AddActorState(EAS_Fall);
			}
		}
		if (m_eCurState & EAS_Climb) 
		{
			if (Coll(m_iRowFoot,m_iColMid) & EBT_Ladder) {
				bIgnore = true;
				UpdateGrid();
				m_vecAcc.y = 0.f;
			} else {
				SubtractActorState(EAS_Climb);
				AddActorState(EAS_Fall);
			}
			if (m_eCurState & EAS_Walk) 
				m_vecVel.x = m_fDefSpe * (m_bMirror?-1:1);
			else m_vecVel.x = 0.f;
			if (m_bUp) m_vecVel.y = -m_fDefSpe;
			else if (bDuck) m_vecVel.y = m_fDefSpe;
			else m_vecVel.y = 0.f;
			break;
		}
		if (m_eCurState & EAS_Swim) 
		{
			
		}
		if (m_eCurState & EAS_Jump) 
		{
			if (m_bJump) {
				m_vecVel.y = -m_fJumpHeight;
				m_bJump = false;
				SubtractActorState(EAS_Jump); 
				AddActorState(EAS_Attack);
			}
		}
		if (m_eCurState & EAS_Fall) 
		{
			if (Coll(m_iRowFoot,m_iCol) & EBT_Water || 
				Coll(m_iRowFoot,m_iColRight) & EBT_Water)
				m_vecAcc.y = m_fGravityWater;
			else m_vecAcc.y = m_fGravity;
		}
		if (m_eCurState & EAS_Walk) 
		{
			if (abs(m_vecVel.x) < m_fDefSpe || (m_bMirror?m_vecVel.x>0:m_vecVel.x<0)) 
				m_vecAcc.x = m_fDefAcc * (m_bMirror?-1:1);
			else m_vecAcc.x = 0.f;
		}
		if (m_eCurState & EAS_Stand)
		{			
			if (abs(m_vecVel.x) > 1.f) m_vecAcc.x = m_fDefAcc * (m_vecVel.x>0.f?-1:1);
			else { m_vecAcc.x = 0.f; m_vecVel.x = 0.f; }
		}
	} while (false);
	m_vecVel += m_vecAcc * fElapsedTime;
	m_vecPos += m_vecVel * fElapsedTime;
	if (m_vecPos.x < 0.f) m_vecPos.x = 0.f;
	else if (m_vecPos.x > m_fMaxCol) m_vecPos.x = m_fMaxCol;
	if (m_vecPos.y < 0.f) m_vecPos.y = 0.f;
	if (!bIgnore) DetectCollision(fElapsedTime, bDuck);
	return m_vecPos.y > BG_HEIGHT;
}

void CPlayer::DetectCollision(float fElapsedTime, bool bDuck) {
	unsigned int bUpdateX(0), bUpdateY(0);
	UpdateGrid();
	if (m_iRowFoot >= MAX_ROW) return;
	short sCollision(GetColl());

	if (m_vecVel.y > 0.f) {
		bUpdateY = DetectPixelColl(sCollision & EGC_DOWN);
	} else if (m_vecVel.y < 0.f) {
		bUpdateY = DetectPixelColl(sCollision & EGC_UP);
	}
	if (bUpdateY) {
		if (IsHill(bUpdateY)) {
			m_vecPos.y -= m_iShift;
			m_vecVel.y = m_vecVel.y < 0.f ? .1f : 0.f;
		} else {
			if ((m_eCurState & EAS_Fall)==0) {
				if (m_vecVel.y > 0.f) m_bJump = true;
				m_vecPos.y -= m_vecVel.y * fElapsedTime;
				m_vecVel.y = m_vecVel.y < 0.f ? .1f : 0.f;
			}
		}
		UpdateGridY(); sCollision = GetColl();
	}

	if (m_vecVel.x > 0.f) {
		bUpdateX = DetectPixelColl(sCollision & EGC_RIGHT);
	} else if (m_vecVel.x < 0.f) {
		bUpdateX = DetectPixelColl(sCollision & EGC_LEFT);
	}
	if (bUpdateX) {
		m_vecPos.x -= m_vecVel.x * fElapsedTime;
		m_vecVel.x = 0.f;
		if (IsHill(bUpdateX)) 
			m_vecPos.y -= m_iShift;
		UpdateGridX();
	}

	PrizesColl(m_vecVel.x > 0.f);
}

short CPlayer::GetColl() {
	short sCollision(0);
	bool bEdge(m_iColRight <= m_fMaxCol/BLOCKLENGTH);
	if (Coll(m_iRow,m_iCol) > EBT_BlockMin)						sCollision += EGC_11;
	if (bEdge && Coll(m_iRow,m_iColRight) > EBT_BlockMin)		sCollision += EGC_12;
	if (Coll(m_iRowFoot,m_iCol) > EBT_BlockMin)					sCollision += EGC_31;
	if (bEdge && Coll(m_iRowFoot,m_iColRight) > EBT_BlockMin)	sCollision += EGC_32;
	if (In3Grid()) {
		if (Coll(m_iRow+1,m_iCol) > EBT_BlockMin)				sCollision += EGC_21;
		if (bEdge && Coll(m_iRow+1,m_iColRight) > EBT_BlockMin)	sCollision += EGC_22;
	}
	return sCollision;
}

bool CPlayer::PrizeCollProceed(int iRow, int iCol, int eColl) {
	if (Coll(iRow, iCol) & EBT_Prize) {
		std::pair<bool, CScene::TilesIt> p = 
			CScene::GetInstance()->GetPrizeIt(iRow, iCol);
		if (p.first) {
			if (DetectPixelColl(eColl, p.second->id)) {
				CScene::GetInstance()->PrizeAcquired(p.second);
				return true;
			}
		}
	}
	return false;
}

bool CPlayer::PrizesColl(bool bRight) {
	short sCollision(0);
	if (bRight) {
		if (PrizeCollProceed(m_iRow,m_iCol, EGC_11))		sCollision += EGC_11;
		if (PrizeCollProceed(m_iRowFoot,m_iCol, EGC_31))	sCollision += EGC_31;
		if (In3Grid()) {
			if (PrizeCollProceed(m_iRow+1,m_iCol, EGC_21))	sCollision += EGC_21;
		}
	} else {
		if (PrizeCollProceed(m_iRow,m_iColRight, EGC_12))		sCollision += EGC_12;
		if (PrizeCollProceed(m_iRowFoot,m_iColRight, EGC_32))	sCollision += EGC_32;
		if (In3Grid()) {
			if (PrizeCollProceed(m_iRow+1,m_iColRight, EGC_22)) sCollision += EGC_22;
		}
	}
	return sCollision > 0;
}

unsigned int CPlayer::DetectPixelColl(short sColl, unsigned int id) {
	if (sColl==0) return 0;	/* Switch to grid collision -->*/
	bool isBlock(id==0);
	for (unsigned int i(0); i < 6; ++i, sColl = (sColl>>1)) {
		if (sColl & 1) {
			unsigned int r(i>3?m_iRowFoot:m_iRow+i/2), c(i%2?m_iColRight:m_iCol); 
			if (isBlock) id = m_tiles[r][c]>>EBT_BlockNum;
			int *pBlc = CResourceManager::GetIntance()->GetPixels(id);
			RECT blc, its, ply = { (long) m_vecPos.x, (long) m_vecPos.y, 
				(long) (m_vecPos.x+m_vecSize.x), (long) (m_vecPos.y+m_vecSize.y) };
			blc.top = r * BLOCKLENGTH;
			blc.bottom = blc.top + BLOCKLENGTH;
			blc.left = c * BLOCKLENGTH;
			blc.right = blc.left + BLOCKLENGTH;
			IntersectRect(&its, &ply, &blc);
			ply.top -= its.top; ply.left -= its.left;
			blc.top -= its.top; blc.left -= its.left; 
			int m, n, w(its.right-its.left), h(its.bottom-its.top);
			for (m = 0; m < h; ++m) {
				for (n = 0; n < w; ++n) {
					if ((pBlc[(m-blc.top)*BLOCKLENGTH+(n-blc.left)] & 0xFF000000)
						// Next Line: Toggle Simplified Pixel Collision
						&& (m_pxl[(m-ply.top)*BLOCKLENGTH+(n-ply.left)] & 0xFF000000)
						) {
						if (g_bDebug) {
							char t[16]; _itoa_s(id, t, 10); strcat_s(t, "\n");
							OutputDebugStringA(t);
						}
						if ((m_eCurState & EAS_Fall)==0) 
							SubtractActorState(EAS_Hurt);
						m_iShift = h-m;
						return id;
					}
				}
			}
		}
	}
	return 0;/**/
	return 1;
}

bool CPlayer::IsHill(unsigned int id) {
	//switch (id) {
	//case 1014: case 1015: case 1032: case 1033: case 1050: case 1051:
	//case 1068: case 1069: case 1086: case 1087: case 1104: case 1105:
	//	return true;
	//}
	id -= 1014;
	if (id / 18 < 6 && id % 18 < 2) return true;
	return false;
}