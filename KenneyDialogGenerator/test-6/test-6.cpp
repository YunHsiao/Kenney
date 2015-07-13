// test-6.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "tinyxml2.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace tinyxml2;

struct RECT {
	long left;
	long top;
	long right;
	long bottom;
};

int _tmain(int argc, _TCHAR* argv[])
{
	XMLDocument doc, out; out.InsertFirstChild(out.NewDeclaration());
	if (doc.LoadFile("Plist.plist")) return 0;
	XMLElement *node = doc.FirstChildElement()->FirstChildElement()->FirstChildElement("dict");
	XMLElement *key, *dict;
	vector<string> keys; vector<RECT> rects;
	for (key = node->FirstChildElement("key"), 
		dict = node->FirstChildElement("dict"); 
		key && dict; 
		key = key->NextSiblingElement("key"), 
		dict = dict->NextSiblingElement("dict")) {
			keys.push_back(string(key->GetText()));
			RECT rect; string num("0123456789");
			string pos(dict->FirstChildElement("string")->GetText());						// "{{976,10},{38,36}}"
			pos = pos.substr(2, pos.size()-4);												// "976,10},{38,36"
			rect.left = atoi(pos.substr(0, pos.find_first_not_of(num)).c_str());			// = 976
			pos = pos.substr(pos.find_first_not_of(num));
			pos = pos.substr(pos.find_first_of(num));										// "10},{38,36"
			rect.top = atoi(pos.substr(0, pos.find_first_not_of(num)).c_str());				// = 10
			pos = pos.substr(pos.find_first_not_of(num));
			pos = pos.substr(pos.find_first_of(num));										// "38,36"
			rect.right = atoi(pos.substr(0, pos.find_first_not_of(num)).c_str());			// = 38
			pos = pos.substr(pos.find_first_not_of(num));
			pos = pos.substr(pos.find_first_of(num));										// "36"
			rect.bottom = atoi(pos.c_str());												// = 36
			rect.bottom += rect.top; rect.right += rect.left; rects.push_back(rect);			
	}
	for (unsigned int i(0); i < keys.size(); ++i) {
		node = out.NewElement("Sprite");
		node->SetAttribute("id", keys[i].c_str());
		node->SetAttribute("left", rects[i].left);
		node->SetAttribute("top", rects[i].top);
		node->SetAttribute("right", rects[i].right);
		node->SetAttribute("bottom", rects[i].bottom);
		out.InsertEndChild(node);
	}
	out.SaveFile("Plist.xml");
	out.Clear();
	out.InsertFirstChild(out.NewDeclaration());
	for (unsigned int i(0); i < keys.size(); ++i) {
		node = out.NewElement("TexRECT");
		node->SetAttribute("id", keys[i].c_str());
		node->SetAttribute("Left", rects[i].left);
		node->SetAttribute("Top", rects[i].top);
		node->SetAttribute("Right", rects[i].right);
		node->SetAttribute("Bottom", rects[i].bottom);
		out.InsertEndChild(node);
	}
	out.SaveFile("Codex.xml");

	int x, y, w, h, e(7); char* z = "0.51";
	cout << "x: "; cin >> x; cout << "y: "; cin >> y;
	cout << "width: "; cin >> w; cout << "height: "; cin >> h;
	for (unsigned int i(0); i < keys.size(); ++i) {
		if (keys[i].find("panel") >= keys.size()) continue;
		out.Clear();
		out.InsertFirstChild(out.NewDeclaration());
		// Middle
		node = out.NewElement("Image");
		node->SetAttribute("ID", 6101);
		node->SetAttribute("x", x+e);
		node->SetAttribute("y", y+e);
		node->SetAttribute("z", z);
		node->SetAttribute("width", w-2*e);
		node->SetAttribute("height", h-2*e);
		node->SetAttribute("left", rects[i].left+e);
		node->SetAttribute("top", rects[i].top+e);
		node->SetAttribute("right", rects[i].right-e);
		node->SetAttribute("bottom", rects[i].bottom-e);
		out.InsertEndChild(node);
		// Left-Top
		node = out.NewElement("Image");
		node->SetAttribute("ID", 6102);
		node->SetAttribute("x", x);
		node->SetAttribute("y", y);
		node->SetAttribute("z", z);
		node->SetAttribute("width", e);
		node->SetAttribute("height", e);
		node->SetAttribute("left", rects[i].left);
		node->SetAttribute("top", rects[i].top);
		node->SetAttribute("right", rects[i].left+e);
		node->SetAttribute("bottom", rects[i].top+e);
		out.InsertEndChild(node);
		// Right-Top
		node = out.NewElement("Image");
		node->SetAttribute("ID", 6103);
		node->SetAttribute("x", x+w-e);
		node->SetAttribute("y", y);
		node->SetAttribute("z", z);
		node->SetAttribute("width", e);
		node->SetAttribute("height", e);
		node->SetAttribute("left", rects[i].right-e);
		node->SetAttribute("top", rects[i].top);
		node->SetAttribute("right", rects[i].right);
		node->SetAttribute("bottom", rects[i].top+e);
		out.InsertEndChild(node);
		// Right-Bottom
		node = out.NewElement("Image");
		node->SetAttribute("ID", 6104);
		node->SetAttribute("x", x+w-e);
		node->SetAttribute("y", y+h-e);
		node->SetAttribute("z", z);
		node->SetAttribute("width", e);
		node->SetAttribute("height", e);
		node->SetAttribute("left", rects[i].right-e);
		node->SetAttribute("top", rects[i].bottom-e);
		node->SetAttribute("right", rects[i].right);
		node->SetAttribute("bottom", rects[i].bottom);
		out.InsertEndChild(node);
		// Left-Bottom
		node = out.NewElement("Image");
		node->SetAttribute("ID", 6105);
		node->SetAttribute("x", x);
		node->SetAttribute("y", y+h-e);
		node->SetAttribute("z", z);
		node->SetAttribute("width", e);
		node->SetAttribute("height", e);
		node->SetAttribute("left", rects[i].left);
		node->SetAttribute("top", rects[i].bottom-e);
		node->SetAttribute("right", rects[i].left+e);
		node->SetAttribute("bottom", rects[i].bottom);
		out.InsertEndChild(node);
		// Left
		node = out.NewElement("Image");
		node->SetAttribute("ID", 6106);
		node->SetAttribute("x", x);
		node->SetAttribute("y", y+e);
		node->SetAttribute("z", z);
		node->SetAttribute("width", e);
		node->SetAttribute("height", h-2*e);
		node->SetAttribute("left", rects[i].left);
		node->SetAttribute("top", rects[i].top+e);
		node->SetAttribute("right", rects[i].left+e);
		node->SetAttribute("bottom", rects[i].bottom-e);
		out.InsertEndChild(node);
		// Top
		node = out.NewElement("Image");
		node->SetAttribute("ID", 6107);
		node->SetAttribute("x", x+e);
		node->SetAttribute("y", y);
		node->SetAttribute("z", z);
		node->SetAttribute("width", w-2*e);
		node->SetAttribute("height", e);
		node->SetAttribute("left", rects[i].left+e);
		node->SetAttribute("top", rects[i].top);
		node->SetAttribute("right", rects[i].right-e);
		node->SetAttribute("bottom", rects[i].top+e);
		out.InsertEndChild(node);
		// Right
		node = out.NewElement("Image");
		node->SetAttribute("ID", 6108);
		node->SetAttribute("x", x+w-e);
		node->SetAttribute("y", y+e);
		node->SetAttribute("z", z);
		node->SetAttribute("width", e);
		node->SetAttribute("height", h-2*e);
		node->SetAttribute("left", rects[i].right-e);
		node->SetAttribute("top", rects[i].top+e);
		node->SetAttribute("right", rects[i].right);
		node->SetAttribute("bottom", rects[i].bottom-e);
		out.InsertEndChild(node);
		// Bottom
		node = out.NewElement("Image");
		node->SetAttribute("ID", 6109);
		node->SetAttribute("x", x+e);
		node->SetAttribute("y", y+h-e);
		node->SetAttribute("z", z);
		node->SetAttribute("width", w-2*e);
		node->SetAttribute("height", e);
		node->SetAttribute("left", rects[i].left+e);
		node->SetAttribute("top", rects[i].bottom-e);
		node->SetAttribute("right", rects[i].right-e);
		node->SetAttribute("bottom", rects[i].bottom);
		out.InsertEndChild(node);
		out.SaveFile(keys[i].substr(0, keys[i].size()-4).append(".xml").insert(0, "out\\").c_str());
	}
	
	return 0;
}

