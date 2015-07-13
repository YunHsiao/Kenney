#include "..\Mango.h"
#include "../../../tinyxml2/tinyxml2.h"


CMANGOSceneManager::CMANGOSceneManager(void)
{
}

CMANGOSceneManager::~CMANGOSceneManager(void)
{
	int Count = m_SceneArray.GetSize();
	for( int i=0; i < Count; i++ )
	{
		 delete m_SceneArray.GetAt( i );
	}
}

CMANGOScene* CMANGOSceneManager::CreateScene(char * strName, INT iSceneID)
{
	tinyxml2::XMLDocument m_pDoc;
	tinyxml2::XMLElement *pRoot=NULL;
	std::string sElemetName="";

	if( m_pDoc.LoadFile( strName ) )
	{
		WCHAR temp[128];
		std::wstring tem = CU_A2U(strName);
		StringCbPrintf(temp,128,L"错误代码481，请检查 %s 是否存在",tem);
		MessageBox(NULL,temp,L"文件没有找到", MB_OK);
		return 0;
	}

	pRoot = m_pDoc.FirstChildElement();
	if(pRoot)
		sElemetName = pRoot->Value();

	CMANGOScene *pScene = new CMANGOScene; 
	if(sElemetName=="MANGOGUI")
	{	
		int id=0,x=0,y=0,width=0,height=0,type=0;
		float z=1.0f;
		char sText[256];

		char strPrefix[MAX_PATH] = "";
		if(pRoot->Attribute("Texture"))
			strcat_s( strPrefix,pRoot->Attribute("Texture"));
		std::wstring TextureName = CU_A2U(strPrefix);
		
		if(pRoot->Attribute("codex"))
		{
			strcpy_s( strPrefix,260,pRoot->Attribute("codex"));
			pScene->Init(&m_pManager,false,TextureName.c_str(),strPrefix);
		}else
			pScene->Init(&m_pManager,false,TextureName.c_str());
		
		if(iSceneID>0)
			id = iSceneID;
		else
			if(pRoot->Attribute("ID"))
				id = (atoi(pRoot->Attribute("ID")));

		pScene->m_SceneID = id;

		if(pRoot->Attribute("name"))
		{
			strcpy_s( strPrefix,260,pRoot->Attribute("name"));
			pScene->m_wsSceneName = CU_A2U(strPrefix);
		}

		if(pRoot->Attribute("x"))
			x = (atoi(pRoot->Attribute("x")));
		if(pRoot->Attribute("y"))
			y = (atoi(pRoot->Attribute("y")));
		if(pRoot->Attribute("width"))
			width= (atoi(pRoot->Attribute("width")));
		if(pRoot->Attribute("height"))
			height= (atoi(pRoot->Attribute("height")));

		pScene->SetLocation(x,y);
		pScene->SetSize(width,height);
		//为g_HUD对话框设置消息处理函数,添加控件

		//设置回调函数，将窗口ID号传入消息
		pScene->SetCallback( m_pCallbackEvent,(void*)id);
		for(pRoot=pRoot->FirstChildElement();pRoot;pRoot = pRoot->NextSiblingElement())
		{
			if(pRoot->Attribute("ID"))
				id = (atoi(pRoot->Attribute("ID")));
			if(pRoot->Attribute("x"))
				x = (atoi(pRoot->Attribute("x")));
			if(pRoot->Attribute("y"))
				y = (atoi(pRoot->Attribute("y")));
			if(pRoot->Attribute("z"))
				z = (FLOAT)(atof(pRoot->Attribute("z")));

			if(pRoot->Attribute("height"))
				height= (atoi(pRoot->Attribute("height")));
			if(pRoot->Attribute("width"))
				width= (atoi(pRoot->Attribute("width")));
			if(pRoot->Attribute("type"))
				type= (atoi(pRoot->Attribute("type")));

			ZeroMemory(sText, sizeof(sText));
			if(pRoot->GetText())
				strcpy_s(sText,256,pRoot->GetText());
			std::wstring text = CU_A2U(sText);
			
			if(!strcmp(pRoot->Value(),"Button"))
			{		
				pScene->AddButton( id, text.c_str(), x, y, width, height, z, type);
			}else
			if(!strcmp(pRoot->Value(),"ComboBox"))
			{
				pScene->AddComboBox( id, x, y, width, height ,z);
			}else
			if(!strcmp(pRoot->Value(),"CheckBox"))
			{
				pScene->AddCheckBox( id, text.c_str(),x, y, width, height ,z);
			}else
			if(!strcmp(pRoot->Value(),"RadioButton"))
			{
				if(pRoot->Attribute("group"))
					pScene->AddRadioButton( id, (atoi(pRoot->Attribute("group"))), text.c_str(),x, y, width, height ,z);
			}else
			if(!strcmp(pRoot->Value(),"Label"))
			{
				pScene->AddLable( id, text.c_str(), x, y, width, height ,z);
				if(pRoot->Attribute("Type"))
				if(!strcmp("TextArea",pRoot->Attribute("Type")))
				{
					pScene->GetLableByID(id)->GetElement(0)->dwTextFormat = DT_LEFT|DT_TOP|DT_WORDBREAK;
				}
			}else
			if(!strcmp(pRoot->Value(),"EditBox"))
			{
				pScene->AddEditBox( id, text.c_str(), x, y, width, height ,z);
			}else
			if(!strcmp(pRoot->Value(),"IMEEditBox"))
			{
				pScene->AddIMEEditBox( id, text.c_str(), x, y, width, height ,z);
			}else
			if(!strcmp(pRoot->Value(),"Slider"))
			{
				pScene->AddSlider( id, x, y, width, height ,z);
			}else
			if(!strcmp(pRoot->Value(),"Image"))
			{
				RECT re;
				if(pRoot->Attribute("left"))
					re.left = (atoi(pRoot->Attribute("left")));
				if(pRoot->Attribute("top"))
					re.top = (atoi(pRoot->Attribute("top")));
				if(pRoot->Attribute("right"))
					re.right = (atoi(pRoot->Attribute("right")));
				if(pRoot->Attribute("bottom"))
					re.bottom = (atoi(pRoot->Attribute("bottom")));

				CMANGOElement *Element = pScene->GetDefaultElement(MANGO_CONTROL_IMAGE,0);
				Element->SetTexture(0,&re);
				pScene->SetDefaultElement( MANGO_CONTROL_IMAGE, 0, Element );
				pScene->AddImage( id, x, y, width, height,z);
			}else
			if (!strcmp(pRoot->Value(), "ListBox"))
			{
				pScene->AddListBox(id, x, y, width, height, z);
			}else
			if (!strcmp(pRoot->Value(), "ScrollBar"))
			{
				pScene->AddScrollBar(id, x, y, width, height, z);
			}
		}
	}

	if(pScene)
		m_SceneArray.Add(pScene);

	return pScene;
}

HRESULT CMANGOSceneManager::OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return m_pManager.OnCreateDevice(pd3dDevice);
}

HRESULT CMANGOSceneManager::OnResetDevice()
{
	return m_pManager.OnResetDevice();
}
void CMANGOSceneManager::OnLostDevice()
{
	m_pManager.OnLostDevice();
}

void CMANGOSceneManager::OnDestroyDevice()
{
	m_pManager.OnDestroyDevice();
}

bool CMANGOSceneManager::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bNoFurtherProcessing = m_pManager.MsgProc( hWnd, uMsg, wParam, lParam );
	if(bNoFurtherProcessing)
		return bNoFurtherProcessing;

	int Count = m_SceneArray.GetSize();
	for( int i=Count-1; i >=0; --i )
	{
		bNoFurtherProcessing = m_SceneArray.GetAt( i )->MsgProc( hWnd, uMsg, wParam, lParam );
		if(bNoFurtherProcessing)
			return bNoFurtherProcessing;
	}
	return bNoFurtherProcessing;
}

CMANGOScene* CMANGOSceneManager::GetSceneByID(int ID)
{
	// Try to find the control with the given ID
	int Count = m_SceneArray.GetSize();
	for( int i=0; i < Count; i++ )
	{
		CMANGOScene* pScene = m_SceneArray.GetAt( i );
		if(pScene->m_SceneID == ID)
			return pScene;
	}
	// Not found
	return NULL;
}

CMANGOScene* CMANGOSceneManager::GetSceneByName(WCHAR *name)
{
	// Try to find the control with the given name

	int Count = m_SceneArray.GetSize();
	for( int i=0; i < Count; i++ )
	{
		CMANGOScene* pScene = m_SceneArray.GetAt( i );
		if(!wcscmp(pScene->m_wsSceneName.c_str(),name))
				return pScene;
	}
	// Not found
	return NULL;
}


int CMANGOSceneManager::GetSceneIDByName( WCHAR *name )
{
	// Try to find the control with the given name
	int Count = m_SceneArray.GetSize();
	for( int i=0; i < Count; i++ )
	{
		CMANGOScene* pScene = m_SceneArray.GetAt( i );
		if(!wcscmp(pScene->m_wsSceneName.c_str(),name))
			return pScene->m_SceneID;
	}
	// Not found
	return 0;
}

void CMANGOSceneManager::OnRender( float fElapsedTime )
{
	HRESULT hr;
	int Count = m_SceneArray.GetSize();
	for( int i=0; i < Count; i++ )
	{
		V((m_SceneArray.GetAt( i ))->OnRender(fElapsedTime));
	}
}