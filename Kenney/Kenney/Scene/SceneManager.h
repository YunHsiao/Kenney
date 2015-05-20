#pragma once

class CSceneManager {

private:
	CSceneManager();
	~CSceneManager();
	
public:
	static CSceneManager* GetIntance() { return &s_sm; }

public:
	BOOL Initialize(LPDIRECT3DDEVICE9 pD3DDevice, CMANGOSceneManager* pSceneManager);
	void Tick(FLOAT fElapsedTime);
	void Render();
	inline CMANGOSceneManager* GetSceneManager() { return m_pSceneManager; }

private:
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPD3DXSPRITE m_pSprite;
	CMANGOSceneManager* m_pSceneManager;

private:
	static CSceneManager s_sm;

};

//#define WORLD_GRID_DISTANCE 64
//
//#define WORLD_TRIGGER_CHECK_TIME		0.2f	// 触发器检测时间间隔
//#define WORLD_TRIGGER_CHECK_DISTANCE	30		// 触发器检测距离
//
//class CPlayer;
//class CNPCManager;
//class CMonsterManager;
//class CTaskManager;
//class CItemManager;
//
//class CWorld
//{
//private:
//
//	struct MapElement
//	{
//		INT id, x, y, tid;
//		RECT rect;
//	};
//
//private:
//	CWorld();
//	~CWorld();
//
//public:
//	static CWorld* GetIntance(){ return &s_World; }
//
//public:
//	void Initialize(LPDIRECT3DDEVICE9 pD3DDevice, CMANGOSceneManager* pSceneManager);
//	void Destroy();
//
//	void EnterScene(const CHAR* filename);
//	void ChangeScene(INT iTriggerID);
//
//	bool Tick(FLOAT fElapsedTime);
//
//	void Render();
//
//	LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//	//inline INT GetCurrentMapID() { return m_iCurMapID; }
//	//inline INT GetWorldRow() { return m_iMapRow; }
//	//inline INT GetWorldCol() { return m_iMapCol; }
//	//inline DWORD* GetWorldData() { return m_pMapData; }
//
//	//inline CMANGOSceneManager* GetSceneManager() { return m_pSceneManager; }
//	//inline CPlayer* GetPlayer() { return m_pPlayer; }
//	//inline CNPCManager* GetNPCManager() { return m_pNPCManager; }
//	//inline CMonsterManager* GetMonsterManager() { return m_pMonsterManager; }
//	//inline CTaskManager* GetTaskManager() { return m_pTaskManager; }
//	//inline CItemManager* GetItemManager() { return m_pItemManager; }
//
//protected:
//	void Clear();
//
//	//void LoadMapData(const char* filename);
//	//void LoadMapResource(const char* filename);
//	//void LoadSound(const char* filename);
//
//	//void LoadBrushResource(TiXmlNode* pBrushNode);
//	//void LoadNPCResource(TiXmlNode* pNPCNode);
//	//void LoadBackgroudResource(TiXmlNode* pBackgroudNode);
//
//	//HRESULT LoadTexture(LPCTSTR fileName, MapElementRes* pElementRes);
//
//	// 触发器循环检测
//	//bool TickTrigger(float fElapsedTime);
//	//bool CheckTrigger(MapElement* pTrigger);
//
//	//void Render(LPD3DXSPRITE pSprite);
//
//private:
//	LPDIRECT3DDEVICE9 m_pD3DDevice;
//	LPD3DXSPRITE m_pSprite;
//	CMANGOSceneManager* m_pSceneManager;	// UI场景管理器
//
//	int m_iCurMapID;						// 当前地图ID
//
//	//int m_iMapRow;							// 地图行数
//	//int m_iMapCol;							// 地图列数,地图宽
//	//DWORD* m_pMapData;						// 地图数组
//
//	float m_fTriggerCheckTime;				// 当前触发器循环检测时间
//
//	std::vector<MapElement> m_pBrushElement;			// 画刷元素结构
//	std::vector<MapElement> m_pTriggerElment;			// 触发器结构
//
//	//CPlayer* m_pPlayer;						// 主角玩家
//	//CNPCManager* m_pNPCManager;				// NPC管理器
//	//CMonsterManager* m_pMonsterManager;		// Monster管理器
//	//CTaskManager* m_pTaskManager;			// 任务管理器
//	//CItemManager* m_pItemManager;			// 物品管理器
//
//	D3DXMATRIX m_matWorld;
//
//private:
//	static CWorld s_World;
//};