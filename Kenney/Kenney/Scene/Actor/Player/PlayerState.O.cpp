#include "stdafx.h"
#include "Player.h"
#include "Scene.h"

bool CPlayer::TickFSM(float fElapsedTime)
{
	// Gravity
	if (m_tiles[m_iRowFoot][m_iCol] > EBT_BlockMin || 
		m_tiles[m_iRowFoot][m_iCol+1] > EBT_BlockMin)
		SubtractActorState(EAS_Fall);
	else AddActorState(EAS_Fall);

	if (m_bUp) {
		if (m_tiles[m_iRow][m_iCol] & EBT_Door || m_tiles[m_iRow+1][m_iCol] & EBT_Door) {
			return true;
		} else if (m_tiles[m_iRow][m_iCol] & EBT_Ladder || m_tiles[m_iRow+1][m_iCol] & EBT_Ladder) {
			AddActorState(EAS_Climb);
		}
	}

	bool bIgnore(false);
	while (true) {
		if (m_eCurState & EAS_Death) 
		{
			bIgnore = true;
			break;
		}
		if (m_eCurState & EAS_Hurt)
		{
			m_vecVel.y -= m_fDefSpe;
			SubtractActorState(EAS_Hurt); AddActorState(EAS_Fall);

		}
		if (m_eCurState & EAS_Climb) 
		{
			if (m_tiles[m_iRowFoot][m_iColMid] & EBT_Ladder) {
				m_vecAcc.y = 0.f;
			} else {
				SubtractActorState(EAS_Climb);
				AddActorState(EAS_Fall);
			}
			if (m_eCurState & EAS_Walk) 
				m_vecVel.x = m_fDefSpe * (m_bMirror?-1:1);
			else m_vecVel.x = 0.f;
			if (m_bUp) m_vecVel.y = -m_fDefSpe;
			else if (m_eCurState & EAS_Duck) m_vecVel.y = m_fDefSpe;
			else m_vecVel.y = 0.f;
			break;
		}
		if (m_eCurState & EAS_Swim) 
		{
			
		}
		if (m_eCurState & EAS_Jump) 
		{
			if (m_vecVel.y == 0.f) { 
				m_vecVel.y = -m_fJumpHeight;
			} else { 
				if (m_vecVel.y > 0.f) {
					SubtractActorState(EAS_Jump); 
					/* Toggle flappy bird mode XD -->* 
					m_bJump = true; /**/
					AddActorState(EAS_Fall);
				}
			}
		}
		if (m_eCurState & EAS_Fall) 
		{
			if (m_tiles[m_iRow+1][m_iCol] & EBT_Water || m_tiles[m_iRow+1][m_iCol+1] & EBT_Water)
				m_vecAcc.y = m_fGravityWater;
			else m_vecAcc.y = m_fGravity;
		}
		if (m_eCurState & EAS_Walk) 
		{
			if (abs(m_vecVel.x) < m_fDefSpe) m_vecAcc.x = m_fDefAcc * (m_bMirror?-1:1);
			else m_vecAcc.x = 0.f;
		}
		if (m_eCurState & EAS_Stand)
		{			
			if (abs(m_vecVel.x) > 1.f) m_vecAcc.x = m_fDefAcc * (m_vecVel.x>0.f?-1:1);
			else { m_vecAcc.x = 0.f; m_vecVel.x = 0.f; }
		}
		break;
	}
	m_vecVel += m_vecAcc * fElapsedTime;
	m_vecPos += m_vecVel * fElapsedTime;
	if (m_vecPos.x < 0.f) m_vecPos.x = 0.f;
	else if (m_vecPos.x > m_fMaxCol) m_vecPos.x = m_fMaxCol;
	if (m_vecPos.y < 0.f) m_vecPos.y = 0.f;
	if (!bIgnore) DetectCollision(fElapsedTime);
	return m_vecPos.y > BG_HEIGHT;
}

void CPlayer::DetectCollision(float fElapsedTime) {
	bool bDuck((m_eCurState & EAS_Duck)>0), bUpdateX(false), bUpdateY(false);
	UpdateGrid(bDuck);
	if (m_iRowFoot >= MAX_ROW) return;
	short sCollision(0); bool bEdge(m_iCol+1 <= m_fMaxCol/BLOCKLENGTH);
	if (m_tiles[m_iRow][m_iCol] > EBT_BlockMin)						sCollision += EGC_11;
	if (bEdge && m_tiles[m_iRow][m_iCol+1] > EBT_BlockMin)			sCollision += EGC_12;
	if (m_tiles[m_iRow+1][m_iCol] > EBT_BlockMin)					sCollision += EGC_21;
	if (bEdge && m_tiles[m_iRow+1][m_iCol+1] > EBT_BlockMin)		sCollision += EGC_22;
	if (m_iRow+2 < MAX_ROW) {
		if (m_tiles[m_iRow+2][m_iCol] > EBT_BlockMin)				sCollision += EGC_31;
		if (bEdge && m_tiles[m_iRow+2][m_iCol+1] > EBT_BlockMin)	sCollision += EGC_32;
	}

	if (bDuck) {
		if (m_vecVel.y > 0.f) {
			bUpdateY = DetectPixelColl(sCollision & EGC_MID);
		} else if (m_vecVel.y < 0.f) {
			bUpdateY = DetectPixelColl(sCollision & EGC_UP);
		}
		if (m_vecVel.x > 0.f) {
			bUpdateX = DetectPixelColl(sCollision & EGC_12);
		} else if (m_vecVel.x < 0.f) {
			bUpdateX = DetectPixelColl(sCollision & EGC_11);
		}
	} else {
		if (m_vecVel.y > 0.f) {
			bUpdateY = DetectPixelColl(sCollision & 
				(In3Grid()?EGC_DOWN:EGC_MID));
		} else if (m_vecVel.y < 0.f) {
			bUpdateY = DetectPixelColl(sCollision & EGC_UP);
		}
		if (bUpdateY) {
			m_vecPos.y -= m_vecVel.y * fElapsedTime;
			m_vecVel.y = m_vecVel.y < 0.f ? .1f : 0.f;
			UpdateGridY(bDuck);
			bUpdateY = false;
		}
		if (m_vecVel.x > 0.f) {
			bUpdateX = DetectPixelColl(sCollision & 
				((In3Grid()&&m_vecVel.y)?EGC_RIGHT:EGC_SHORT_RIGHT));
		} else if (m_vecVel.x < 0.f) {
			bUpdateX = DetectPixelColl(sCollision & 
				((In3Grid()&&m_vecVel.y)?EGC_LEFT:EGC_SHORT_LEFT));
		}
	}
	if (bUpdateY) {
		m_vecPos.y -= m_vecVel.y * fElapsedTime;
		m_vecVel.y = m_vecVel.y < 0.f ? .1f : 0.f;
		UpdateGridY(bDuck);
	}
	if (bUpdateX) {
		m_vecPos.x -= m_vecVel.x * fElapsedTime;
		m_vecVel.x = 0.f;
		UpdateGridX(bDuck);
	}
}

bool CPlayer::DetectPixelColl(short sColl) {
	if (!sColl) return false;	/* Toggle pixel collision -->*/
	for (unsigned int i(0); i < 6; ++i, sColl = (sColl>>1)) {
		if (sColl & 1) {
			int *pixels = CResourceManager::GetIntance()->GetPixels
				(m_tiles[m_iRow+i/2][m_iCol+i%2]>>EBT_BlockNum);
			RECT rect, intersect;
			rect.top = (m_iRow+i/2) * BLOCKLENGTH;
			rect.bottom = (m_iRow+i/2+1) * BLOCKLENGTH;
			rect.left = (m_iCol+i%2) * BLOCKLENGTH;
			rect.right = (m_iCol+i%2+1) * BLOCKLENGTH;
			IntersectRect(&intersect, &m_rect, &rect);
			intersect.top -= rect.top;
			intersect.bottom -= rect.top;
			intersect.left -= rect.left;
			intersect.right -= rect.left;
			int m(intersect.top), n(intersect.left);
			for (; m < intersect.bottom; ++m)
				for (; n < intersect.right; ++n) 
					if (pixels[m*BLOCKLENGTH+n] & 0xFF000000)
						return true;
		}
	}
	return false;/**/
	return true;
}