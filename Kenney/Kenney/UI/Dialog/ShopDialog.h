#ifndef KENNEY_SHOPDIALOG
#define KENNEY_SHOPDIALOG

class CShopDialog
{

public:
	static CShopDialog* GetInstance(){ return &m_sShopDialog; }
public:
	CShopDialog(void);
	~CShopDialog(void);
public:
	bool Init(CMANGOSceneManager* mgSceneManager);
	void SetVisible(bool visible = false);
	void ShowShopDialog( const int id, bool visible);
	void Destroy();
	void OnGameGUIEvent(UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext);
private:
	CMANGOSceneManager* m_mgSceneManager;

private:
	static CShopDialog m_sShopDialog;
};

#endif