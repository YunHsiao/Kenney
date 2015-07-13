#ifndef KENNEY_GOODS
#define KENNEY_GOODS

#include "Bag.h"
class CGoods
{
public:
	static CGoods* GetInstance(){ return &m_sGoods; }
public:
	CGoods(void);
	~CGoods(void);
public:
	bool Init(CMANGOSceneManager* mgSceneManager);
	void ShowGoods( const int id, bool visible);
	void SetVisible( bool visible = false );
	void Destroy();
	void OnGameGUIEvent(UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext);
private:
	void ShowGood(int id);
	void HideGoods();
private:
	CMANGOSceneManager* m_mgSceneManager;
	CMANGOScene* m_mgScene;
	unsigned int m_iGoodsID;
	int m_iNpcId;
	CBag* m_pBag;
	//CPlayer* m_player;
private:
	static CGoods m_sGoods;
};
#endif