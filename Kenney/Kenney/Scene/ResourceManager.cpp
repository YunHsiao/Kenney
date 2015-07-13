#include "stdafx.h"
#include "ResourceManager.h"
#include "SceneManager.h"

CResourceManager CResourceManager::s_rm;

CResourceManager::CResourceManager():
m_spritesheet(NULL)
{
}

CResourceManager::~CResourceManager()
{
	Destroy();
}

void CResourceManager::Destroy() {
	for(TextureBackIt it = m_mapTextureBacks.begin(); it != m_mapTextureBacks.end(); ++it)
		SAFE_RELEASE(it->second)
	for(PixelsIt it = m_pixels.begin(); it != m_pixels.end(); ++it)
		SAFE_DELETE_ARRAY(it->second)
	m_pixels.clear();
	m_mapTextureBacks.clear();
	SAFE_RELEASE(m_spritesheet);
}

bool CResourceManager::Initialize(const char* sprite, const char* sheet,const char* back)
{
	if(FAILED(D3DXCreateTextureFromFileExA(CSceneManager::GetIntance()->GetDevice(),
		sprite,D3DX_DEFAULT_NONPOW2,D3DX_DEFAULT_NONPOW2,
		1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,D3DX_FILTER_NONE,0,NULL,NULL,
		&m_spritesheet)))
		return false;

	if(FAILED(LoadBack(back)))
		return false;

	tinyxml2::XMLDocument Doc;
	if(Doc.LoadFile(sheet))
	{
		MessageBox(NULL,TEXT("ResourceManager - 加载资源时出现错误"),NULL,MB_OK);
		return false;
	}
	tinyxml2::XMLElement* mark = Doc.FirstChildElement();
	for(mark->FirstChildElement(); NULL != mark; mark = mark->NextSiblingElement())
	{
		for(tinyxml2::XMLElement* content = mark->FirstChildElement();
			NULL != content; content = content->NextSiblingElement())
		{
			unsigned int id;
			RECT rect;
			if (content->Attribute("id")) {
				id = atoi(content->Attribute("id"));
				rect.left = atoi(content->Attribute("left"));
				rect.top = atoi(content->Attribute("top"));
				rect.right = atoi(content->Attribute("right"));
				rect.bottom = atoi(content->Attribute("bottom"));
				m_textures.insert(std::make_pair(id, rect));

				D3DLOCKED_RECT pValue; int *pColor = NULL;
				unsigned int w = rect.right - rect.left, 
					h = rect.bottom - rect.top;
				m_spritesheet->LockRect(0, &pValue, &rect, 0);
				pColor = (int*) pValue.pBits;
				int *pixels = new int[w * h];
				for(unsigned int i = 0; i < h; ++i) 
					for(unsigned int j = 0; j < w; ++j)
						pixels[i * w + j] = *(pColor + i * pValue.Pitch / 4 + j);
				m_spritesheet->UnlockRect(0);
				m_pixels.insert(std::make_pair(id, pixels));
			}
		}
	}

	return true;
}

void CResourceManager::WritePixels(unsigned int id) {
	char t[16] = {0}; _itoa_s(id, t, 16, 10); strcat_s(t, ".txt");
	std::ofstream out(t);
	if (!out) return;
	char buff[131072] = {0};
	RECT r = GetResource(id);
	unsigned int w = r.right - r.left, h = r.bottom - r.top;
	int* p = GetPixels(id);
	for(unsigned int i = 0; i < h; ++i) {
		for(unsigned int j = 0; j < w; ++j) {
			char b[32] = {0};
			_itoa_s(p[i*w+j], b, 16);
			strcat_s(buff, -1, b);
			strcat_s(buff, -1, " ");
		}
		strcat_s(buff, -1, "\n");
	}
	out << buff; out.close();
}

bool CResourceManager::LoadBack(const char* back)
{
	tinyxml2::XMLDocument Doc;
	if(Doc.LoadFile(back))
	{
		MessageBox(NULL,TEXT("ResourceManager - 加载背景资源时出现错误"),NULL,MB_OK);
		return false;
	}
	tinyxml2::XMLElement* mark = Doc.FirstChildElement();
	for(tinyxml2::XMLElement* content = mark->FirstChildElement(); NULL != content; content = content->NextSiblingElement())
	{
		unsigned int id(0);
		const char* path("");
		if(content->Attribute("id"))
		{
			id = atoi(content->Attribute("id"));
			path = content->Attribute("Path");
			m_mapBack[id] = path;
		}
	}

	for(BackIt it = m_mapBack.begin(); it != m_mapBack.end(); ++it)
	{
		LPDIRECT3DTEXTURE9 resources = NULL;
		D3DXCreateTextureFromFileExA(CSceneManager::GetIntance()->GetDevice(),
			it->second,D3DX_DEFAULT_NONPOW2,D3DX_DEFAULT_NONPOW2,
			1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,
			D3DX_FILTER_LINEAR,D3DX_FILTER_NONE,0,NULL,NULL,
			&resources);
		m_mapTextureBacks[it->first] = resources;
	}
	return true;
}

std::vector<RECT> CResourceManager::GetResourceRange(unsigned int id)
{
	std::vector<RECT> vecRect;
	std::pair<RectsIt, RectsIt> p(m_textures.equal_range(id));
	RectsIt it(p.first);
	while(it != p.second)
	{
		vecRect.push_back(it->second);
		++it;
	}
	return vecRect;
}

std::vector<int*> CResourceManager::GetPixelsRange(unsigned int id) {
	std::vector<int*> vecPxl;
	std::pair<PixelsIt, PixelsIt> p(m_pixels.equal_range(id));
	PixelsIt it(p.first);
	while(it != p.second)
	{
		vecPxl.push_back(it->second);
		++it;
	}
	return vecPxl;
}