#pragma once
#include "src/include/namespace.h"
#include <vector>
#include <tuple>
#include "src/item/id.h"
#include <string>


namespace YJS_NAMESPACE {
	typedef int Offset;
	typedef std::string Context;

	/*
		这是传输节点所需要的最小单位
	*/
	struct ItemMessage {
		char character;
		Id id;
		Id origin;
		Id rightOrigin;

		ItemMessage(){};
		ItemMessage(const char character, const Id id, const Id origin, const Id rightOrigin) :
			character(character),
			id(id),
			origin(origin),
			rightOrigin(rightOrigin) {}
	};

	/*
		这是一种特殊的叶子节点，任何实现底层算法的开发者需要以此类为基类，
		继承他并构造自己的叶子节点。
	*/
	class ItemListInterface {
	public:
		Context context;
		Id headId;
		Id headOrigin;
		Id endRightOrigin;

		ItemListInterface() :
			headId(Id(-1, -1)),
			headOrigin(Id(-1, -1)),
			endRightOrigin(Id(-1, -1))
		{};

		ItemListInterface(const ItemMessage& itemMessage):
				context(1,itemMessage.character),
				headId(itemMessage.id),
				headOrigin(itemMessage.origin),
				endRightOrigin(itemMessage.rightOrigin){}

		ItemListInterface(const ItemListInterface& list, const int& offset):
			headId(list.headId + offset),
			endRightOrigin(list.endRightOrigin),
			headOrigin(list.headId + (offset - 1)),
			context(list.context.substr(offset)){}


		virtual ~ItemListInterface() {};

	};






}