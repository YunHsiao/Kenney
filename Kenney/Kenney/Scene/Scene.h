#ifndef KENNEY_SCENE
#define KENNEY_SCENE
#include "Actor.h"
#include "Player.h"
#include "PlayerDefine.h"
#include "Controller.h"

enum EBlockType
{
	EBT_Door   = (1 << 0),
	EBT_Ladder = (1 << 1),
	EBT_Water  = (1 << 2),
	EBT_Prize  = (1 << 3),
	EBT_Other  = (1 << 4),
	EBT_BlockMin  = (1 << 5),
	EBT_BlockNum  = 5,
	EBT_None   = 0
};

struct SMonsterParameter {

	float fMirrorTime,fRestTime;
	float fGravityWater,fGravity,fJumpHeight;
	float fDefSpe,fDefElapse;
	int  NPCId;
	char NPCName[32];
	char NPCDialogInfo[MAX_PATH];
};

struct MapElement
{
	int id, x, y, tid, swc;
	RECT rect;
};

class CScene {

private:
	CScene();
	~CScene();

public:
	static CScene* GetInstance() { return &s_Scene; }

public:
	bool Initialize(LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTexture, 
		CMANGOSceneManager* pSceneManager, LPD3DXLINE pLine);
	void Tick(float fElapsedTime);
	void Render();

	typedef std::list<MapElement>::iterator TilesIt;
	void CreatePlayer(ERoleType eRoleType, D3DXVECTOR3 vecPos);
	void CreateMonster(MapElement& mapElement);
	void CreateNPC(MapElement& mapElement);
	bool LoadLevel(const int LevelID);
	void Destroy();
	void Click(POINT pos);
	TilesIt PrizeAcquired(TilesIt it);
	std::pair<bool, TilesIt> GetPrizeIt(unsigned int iRow, unsigned int iCol);
	inline SMonsterParameter GetParameter(short id) { return m_mapParameters[id]; }
	inline int GetLevelID() { return m_nLevelID;}
	SMonsterParameter GetmultimapNPCList(int mapid,int npcid);
	inline CPlayer* GetPlayer() { return m_player; }
	int GetJewel(int nLevelID);
	

private:
	void Clear();
	bool LoadMapListByXml(const char* sheet);
	bool LoadParameters(const char* back);
	bool LoadNpcData(const char* back);
	void GetBlockInfo();
	void WriteDebugInfo();

	void ScrollScreen(D3DXVECTOR3 pos);
	void TickActor(float fElapsedTime);

	int  GetChangingLevelID(D3DXVECTOR3 PlayerPos);

	void RenderLogo();
	void RenderBackground();
	void RenderBrush();	
	void RenderActor();

private:

	bool m_bisVertIcle;
	LPD3DXSPRITE m_pSprite;
	LPDIRECT3DTEXTURE9 m_Texture;
	LPDIRECT3DTEXTURE9 m_TextureBackground;

	// µØÍ¼×ÊÔ´
	typedef std::map<int,std::string>::iterator LevelResIT;
	typedef std::map<int,std::string> LevelRes;
	typedef std::map<int,D3DXVECTOR3>::iterator LevelPosIT;
	typedef std::map<int,D3DXVECTOR3> LevelPos;
	typedef std::vector<CActor*>::iterator ActorIt;
	typedef std::list<MapElement> Tiles;
	LevelRes m_mapLevel;
	LevelPos m_mapPlayerPos;
	int m_nLevelID;
	unsigned int **m_ppBlock;
	unsigned int m_iMaxRow;
	unsigned int m_iMaxCol;
	unsigned int m_iBgCount;

	CMANGOSceneManager* m_pSceneManager;
	LPD3DXLINE m_pLine;
	Tiles m_vecMapElement, m_vecPrizes, m_vecDoors;
	std::map<int,int> m_vecBackgroudID, m_mapJewel;
	std::map<unsigned int, SMonsterParameter> m_mapParameters;


	typedef std::multimap<int,SMonsterParameter> multimapNPCList;
	typedef std::multimap<int,SMonsterParameter>::iterator multimapNPCListIt;
	multimapNPCList m_multimapNPCList;


	std::vector<CActor*> m_vecActor;
	CPlayer* m_player;

	D3DXMATRIX m_matWorld, m_matLogo;
	D3DXMATRIX m_matTranslate;

private:
	static CScene s_Scene;

};

#endif