#pragma once

#include "src/struct/YList.h"

#include <list>


namespace YJS_NAMESPACE {



	union ItemIter {
		ItemInterface** itemInterfacePtr;
		void* ItemInterfaceIter;
		~ItemIter() {
			delete 
		}
	};


	template<typename YStruct>
	class YAlloc final {
	public:
		typedef typename YStruct::PointType _PointType;
		typedef typename YStruct::YIterator _ItemPtrType;
		typedef typename YStruct::ItemType _ItemType;


		YAlloc() {
			yStruct = new YStruct;
		};
		~YAlloc() {
			delete yStruct;
		};

		YStruct* yStruct;

		void insertItem(Index index, ItemInterface item) {
			yStruct->insertItem(index, item);
		}

		void deleteItem(Index index) {
			yStruct->deleteItem(index);
		}

		void getItem() {

		}

		ItemIter getItemByPos(Index index) {
			return _getItemByPos(index, _PointType());
		}

		ItemIter _getItemByPos(Index index, PTR) {
			ItemInterface** _ptr = yStruct->getItemByPos(index, PTR());
			ItemIter _itemIter{ .itemInterfacePtr = _ptr };
			return _itemIter;
		}

		ItemIter _getItemByPos(Index index, ITER) {
			void* _iter = yStruct->getItemByPos(index, ITER());
			ItemIter _itemIter{.ItemInterfaceIter = _iter};
			return _itemIter;
		}


		ItemIter predecessor(ItemIter item) {
			return _predecessor(item, _PointType());
		}

		//ItemIter successor(ItemIter item) {
		//	yStruct->successor(item, PointType());
		//}

		ItemIter _predecessor(ItemIter item, PTR) {
			ItemInterface** _ptr= yStruct->predecessor((ItemInterface *)item.itemInterfacePtr, PTR());
			ItemIter _itemIter { .itemInterfacePtr = _ptr };
			return _itemIter;
		};

		ItemIter _predecessor(ItemIter item, ITER) {
			void* _iter = yStruct->predecessor(item.ItemInterfaceIter, ITER());
			ItemIter _itemIter{ .ItemInterfaceIter = _iter };
			return _itemIter;
		};

	};
}