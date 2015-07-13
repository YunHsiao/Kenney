#ifndef KENNEY_RESOURCEMANAGER
#define KENNEY_RESOURCEMANAGER

class CResourceManager {

private:
	CResourceManager();
	~CResourceManager();
public:
	static CResourceManager* GetIntance() { return &s_rm; }
public:
	bool Initialize(const char* sprite/*png*/, const char* sheet/*xml*/,const char* back/*xml*/);
	void Destroy();
	inline LPDIRECT3DTEXTURE9 GetBackground(unsigned int id) { return m_mapTextureBacks[id]; }
	inline LPDIRECT3DTEXTURE9 GetAtlas() { return m_spritesheet; }
	void WritePixels(unsigned int id);
	inline const RECT& GetResource(unsigned int id) 
	{ return m_textures.lower_bound(id)->second; }
	std::vector<RECT> GetResourceRange(unsigned int id);
	inline int* GetPixels(unsigned int id) { return m_pixels.lower_bound(id)->second; }
	std::vector<int*> GetPixelsRange(unsigned int id);
private:
	bool LoadBack(const char* back);
private:
	typedef std::map<unsigned int, const char*> Back;
	typedef std::map<unsigned int, const char*>::iterator BackIt;
	typedef std::map<unsigned int, LPDIRECT3DTEXTURE9> TextureBack;
	typedef std::map<unsigned int, LPDIRECT3DTEXTURE9>::iterator TextureBackIt;
	typedef std::multimap<unsigned int, RECT> Rects;
	typedef std::multimap<unsigned int, RECT>::iterator RectsIt;
	typedef std::multimap<unsigned int, int*> Pixels;
	typedef std::multimap<unsigned int, int*>::iterator PixelsIt;
private:
	LPDIRECT3DTEXTURE9 m_spritesheet;				// 场景元素总图
	TextureBack m_mapTextureBacks;					// 背景贴图
	Back m_mapBack;
	Rects m_textures;								// 元素切图信息
	Pixels m_pixels;

private:
	static CResourceManager s_rm;

};

#endif