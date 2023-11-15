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
		���Ǵ���ڵ�����Ҫ����С��λ
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
		����һ�������Ҷ�ӽڵ㣬�κ�ʵ�ֵײ��㷨�Ŀ�������Ҫ�Դ���Ϊ���࣬
		�̳����������Լ���Ҷ�ӽڵ㡣
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