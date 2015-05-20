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

	LPDIRECT3DTEXTURE9 m_spritesheet;				// 场景元素总图
	std::vector<LPDIRECT3DTEXTURE9> m_resources;	// 其他贴图
	std::multimap<UINT, RECT> m_textures;			// 元素切图信息

private:
	static CResourceManager s_rm;

};