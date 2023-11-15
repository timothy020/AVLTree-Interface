#pragma once

#include "src/struct/YStruct.h"

#include <list>

namespace YJS_NAMESPACE {
	class YList : public YInterface {
	public:
		class ItemList : public ItemListInterface {
		public:
			ItemList(const ItemMessage& itemMessage)
				:ItemListInterface(itemMessage),
				_size(1),
				isDelete(1,0) {};

			//ItemList(const ItemListInterface& p)
			//	:ItemListInterface(p)
			//{};

			virtual ~ItemList() {};
			// 添加你需要的的变量, 这是一个例子
			ItemList* left = nullptr;

			ItemList* right = nullptr;

			std::string isDelete;

			int _size;
			// int realSize;

			void push_back(ItemMessage itemMsg) {
				++_size;
				isDelete.push_back(0);
				context.push_back(itemMsg.character);
			};

			ItemList(const ItemList& itemlist, const int& offset) :
				ItemListInterface(itemlist, offset),
				_size(itemlist._size - offset),
				isDelete(itemlist.isDelete.substr(offset)),
				left(const_cast<ItemList*>(&itemlist)),
				right(itemlist.right)
			{}

			ItemList* subItemList(int offset) {
				ItemList* _temp = new ItemList(*this, offset);
				this->context.resize(offset);
				this->endRightOrigin = this->headId + offset;
				this->_size = offset;
				this->right = _temp;
				this->isDelete.resize(offset);
				return _temp;
			}
		};

		typedef unsigned int Size;
		typedef ItemList* ItemListPtr;

		ItemListPtr _head;
		ItemListPtr _end;

		YList() {
			_head = new ItemList(
				ItemMessage{ '[', Id(8888,0) , Id(0, 0), Id(9999, 0) });
			_end = new ItemList(
				ItemMessage{ ']', Id(9999,0) , Id(8888, 0), Id(0, 0) });
			_head->_size = 0;
			_head->right = _end;
			_end->_size = 0;
			_end->left = _head;

		};
		virtual ~YList() {
			delete _head;
			delete _end;
		};

		//Size size = 0;


		virtual inline ItemPtr begin() const override {
			return successor(ItemPtr(_head, 0));
		};

		virtual inline ItemPtr end() const override {
			return ItemPtr(_end, 0);
		};

		virtual void insertItem(Index index, ItemMessage itemMsg) override;

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
		ItemPtr _getItemByIndex(Index index) const;

		bool _isEnd(ItemPtr item) const;

		bool _isBegin(ItemPtr item) const;

		Id _idByItemPtr(ItemPtr item) const;
	};
}