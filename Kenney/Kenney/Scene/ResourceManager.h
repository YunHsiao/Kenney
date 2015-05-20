#pragma once

class CResourceManager {

private:
	CResourceManager();
	~CResourceManager();

public:
	static CResourceManager* GetIntance() { return &s_rm; }
public:
	BOOL Initialize(const CHAR* sprite/*png*/, const CHAR* sheet/*xml*/);
	const RECT& GetResource(UINT name);
	inline LPDIRECT3DTEXTURE9 GetAtlas() { return m_spritesheet; }

private:

	LPDIRECT3DTEXTURE9 m_spritesheet;				// ����Ԫ����ͼ
	std::vector<LPDIRECT3DTEXTURE9> m_resources;	// ������ͼ
	std::multimap<UINT, RECT> m_textures;			// Ԫ����ͼ��Ϣ

private:
	static CResourceManager s_rm;

};