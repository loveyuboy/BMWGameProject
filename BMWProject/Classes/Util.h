/*
	创建人:张亚杰
	修改日期：2016.2.28
*/
#ifndef __UTIL__BMWPROJECT__
#define __UTIL__BMWPROJECT__

#include "cocos2d.h"
#include "Game.h"
USING_NS_CC;

#define Conn(x,y) x##y
#define ToChar(x) #@x
#define ToString(x) #x

/*CCSTUDIO_RESGISTR_READER_OBJECT 使用方法
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
*/
#define CCSTUDIO_RESGISTR_READER_OBJECT(readerName)\
	CSLoader* instance = CSLoader::getInstance();\
	instance->registReaderObject((ToString(readerName)),(ObjectFactory::Instance)readerName::getInstance);

#define CREATE_CSLOADER_NODE(uiName) CSLoader::createNode(Conn(ToString(uiName),".csb") );

/* 
CCSTUDIO_CREATE_STUDIOREADER_CLASS_H 使用方法 引入头文件
#include "cocostudio/CocosStudioExport.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"
*/
#define CCSTUDIO_CREATE_STUDIOREADER_CLASS_H(className)\
	class className : public cocostudio::NodeReader{\
	public: className(){}; ~className(){}; static className* getInstance(); static void purge();\
	cocos2d::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions);}

/*
使用方法 CCSTUDIO_CREATE_STUDIOREADER_CLASS_CPP(Reader,UIName)
*/
#define CCSTUDIO_CREATE_STUDIOREADER_CLASS_CPP(className,uiName)\
	static className* Conn(_instance,className) = nullptr;\
	className* className::getInstance(){\
	if (!Conn(_instance,className)){\
		Conn(_instance,className) = new className();}\
	return Conn(_instance,className);}\
	void className::purge(){CC_SAFE_DELETE(Conn(_instance,className));}\
	Node* className::createNodeWithFlatBuffers(const flatbuffers::Table *nodeOptions){\
	uiName* node = uiName::create();setPropsWithFlatBuffers(node, nodeOptions);return node;}\

#endif // !__UTIL__BMWPROJECT__

