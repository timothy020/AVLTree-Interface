#pragma once

#include "src/struct/YStruct.h"

#include <list>

namespace YJS_NAMESPACE {

	constexpr int cacheSize = 4;
	class YListNew : public YInterface {
	public:
		class ItemList : public ItemListInterface {
		public:
			ItemList(const ItemMessage& itemMessage)
				:ItemListInterface(itemMessage),
				size(1),
				isDelete(1,0) {};

			//ItemList(const ItemListInterface& p)
			//	:ItemListInterface(p)
			//{};

			virtual ~ItemList() {};
			// 添加你需要的的变量, 这是一个例子
			ItemList* left = nullptr;

			ItemList* right = nullptr;

			std::string isDelete;

			int size;
			// int realSize;

			void push_back(ItemMessage itemMsg) {
				++size;
				isDelete.push_back(0);
				context.push_back(itemMsg.character);
			};

			ItemList(const ItemList& itemlist, const int offset) :
				ItemListInterface(itemlist, offset),
				size(itemlist.size - offset),
				isDelete(itemlist.isDelete.substr(offset)),
				left(const_cast<ItemList*>(&itemlist)),
				right(itemlist.right)
			{}

			ItemList* subItemList(int offset) {
				ItemList* _temp = new ItemList(*this, offset);
				this->context.resize(offset);
				this->endRightOrigin = this->headId + offset;
				this->size = offset;
				this->right = _temp;
				this->isDelete.resize(offset);
				return _temp;
			}
		};

		typedef unsigned int Size;
		typedef ItemList* ItemListPtr;

		ItemListPtr _head;
		ItemListPtr _end;

		ItemPtr headPtr, endPtr, tempPtr;

		YListNew() {
			_head = new ItemList(
				ItemMessage{ '[', Id(8888,0) , Id(0, 0), Id(9999, 0) });
			_end = new ItemList(
				ItemMessage{ ']', Id(9999,0) , Id(8888, 0), Id(0, 0) });
			_head->size = 0;
			_head->right = _end;
			_end->size = 0;
			_end->left = _head;

			headPtr = std::make_pair(_head, 0);
			endPtr = std::make_pair(_end, 0);
			//tempPtr = std::make_pair(nullptr, -1);

			_cachePtr = std::vector<ItemPtr>(cacheSize, ItemPtr(nullptr, 0));
			_cachePos = std::vector<int>(cacheSize, -1);
		};
		virtual ~YListNew() {
			delete _head;
			delete _end;
		};

		//Size size = 0;

		std::vector<ItemPtr> _cachePtr;
		std::vector<int> _cachePos;
		//int _cacheNext = 0;

		virtual inline ItemPtr begin() const override {
			return successor(headPtr);
		};

		virtual inline ItemPtr end() const override {
			return endPtr;
		};

		virtual void insertItem(Index index, ItemMessage itemMsg) override;

		virtual void insertItem(Index index, char itemChar, Id id) override;

		// virtual void insertItem(ItemPtr item) override;

		virtual void deleteItem(Index index) override;

		// virtual void deleteItem(ItemPtr item) override;

		virtual ItemPtr getItemByPos(Index index) const override;

		// virtual ItemPtr getItemByGid(Id_const id) override;

		// virtual Id_const idByIndex(Index index) override;

		// virtual Index posByGid(Id_const id) override;

		virtual ItemPtr predecessor(ItemPtr item) const override;

		virtual ItemPtr successor(ItemPtr item) const override;

		virtual Id getId(ItemPtr itemPtr) const override;



	private:
		ItemPtr _truePredecessor(ItemPtr item) const;

		ItemPtr _getItemByIndex(Index index) const;
		

		bool _isEnd(ItemPtr item) const;

		bool _isBegin(ItemPtr item) const;

		Id _idByItemPtr(ItemPtr item) const;

		bool _findCache(int index, ItemPtr& itemPtr) const;

		bool _findCache2(int index, ItemPtr& itemPtr) ;

		void _insertCache(int index, ItemPtr& itemPtr);

		void _refreshCache(int index);

		void _refreshCache2(int index);
	};
}