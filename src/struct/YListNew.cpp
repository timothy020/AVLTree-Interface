#include "YListNew.h"
#include "YListNew.h"
#include "YListNew.h"
#include "YListNew.h"
#include "YListNew.h"
#include "src/struct/YListNew.h"


namespace YJS_NAMESPACE {

	// ItemPtr 指向迭代器的指针

	void YListNew::insertItem(Index index, ItemMessage itemMsg)
	{
		/*
		ItemPtr _insertPtr = _getItemByIndex(index);
		ItemListPtr _insertFirst = (ItemListPtr)_insertPtr.first;

		// 特化版本
		if (++_idByItemPtr(_insertPtr) == itemMsg.id) {
			_insertFirst->push_back(itemMsg);
			// YJS_DEBUG("特化插入");
		}
		else {
			ItemListPtr insertNode = new ItemList(itemMsg);
			if (!_isEnd(_insertPtr)) {
				ItemListPtr _insertSecond =
					_insertFirst->subItemList(_insertPtr.second + 1);
				// YJS_DEBUG("分裂");
				//static int conut = 0;
				//YJS_DEBUG("%d", conut++);

			}
			_insertFirst->right->left = insertNode;
			insertNode->right = _insertFirst->right;
			_insertFirst->right = insertNode;
			insertNode->left = _insertFirst;

		}
		*/
		static ItemPtr _insertFirstPtr, _insertSecondPtr;
		static ItemListPtr _insertFirst, _insertSecond, insertNode;
		static Id _id;
		static bool flag = true;

		if (! _findCache(index, _insertSecondPtr))
		{
			_insertSecondPtr = _getItemByIndex(index);
			flag = true;
		}
		else {
			flag = false;
		}
		
		
		//_insertSecondPtr = _getItemByIndex(index);
		
		_insertSecond = (ItemListPtr)_insertSecondPtr.first;

		if (_insertSecondPtr.second == 0)
		{
			
			_insertFirst = _insertSecond->left;
			_id = _insertFirst->headId + (_insertFirst->size);
			if (_id == itemMsg.id)
			{
				_insertFirst->push_back(itemMsg);
				//YJS_DEBUG("特化插入");
			}
			else
			{


				insertNode = new ItemList(itemMsg);
				_insertSecond->left = insertNode;
				_insertFirst->right = insertNode;
				insertNode->right = _insertSecond;
				insertNode->left = _insertFirst;
				//YJS_DEBUG("中间插入 %d", itemMsg.id.clock);
			}
			if(flag)
				_insertCache(index, _insertSecondPtr);
		}
		else
		{
			_insertFirst = _insertSecond;
			_insertSecond =
				_insertFirst->subItemList(_insertSecondPtr.second);
			insertNode = new ItemList(itemMsg);
			_insertSecond->left = insertNode;
			_insertFirst->right = insertNode;
			insertNode->right = _insertSecond;
			insertNode->left = _insertFirst;
			_insertSecond->right->left = _insertSecond;

			for (int i = 0; i < 4; ++i) {
				this->_cachePos[i] = -1;
			}

			_insertSecondPtr.first = _insertSecond;
			_insertSecondPtr.second = 0;
			_insertCache(index, _insertSecondPtr);
			//YJS_DEBUG("分裂");
			//static int conut = 0;
			//YJS_DEBUG("%d", conut++);
		}
		_refreshCache(index);
	}

	void YListNew::insertItem(Index index, char itemChar, Id id)
	{

	}

	//void YList::insertItem(ItemPtr item)
	//{
	//	return;
	//}

	void YListNew::deleteItem(Index index)
	{
		// ItemPtr _deletePtr = _getItemByIndex(index);

		static ItemPtr _deletePtr;


		if (!_findCache2(index, _deletePtr))
		{
			_deletePtr = _getItemByIndex(index);
		}
		_refreshCache2(index);


		if (_deletePtr == this->end())
			return;

		//--this->size;

		ItemList* p = (ItemList*)_deletePtr.first;


		p->isDelete[_deletePtr.second] = 1;
		// --p->realSize;
	}

	ItemPtr YListNew::getItemByPos(Index index) const
	{
		return _getItemByIndex(index);
	}

	//void YList::deleteItem(ItemPtr item)
	//{
	//	return;
	//}

	//void YList::getItemByPos(Index index, ItemPtr itemPtr)
	//{
	//	*(YIterator*)itemPtr = _getItemByIndex(index);
	//}

	//ItemPtr YList::getItemByGid(Id_const id)
	//{
	//	return nullptr;
	//}

	//Id_const YList::idByIndex(Index index)
	//{
	//	return (*_getItemByIndex(index)).id;
	//}

	//Index YList::posByGid(Id_const id)
	//{
	//	return 0;
	//}



	ItemPtr YListNew::predecessor(ItemPtr item) const
	{
		static int offset = 0;
		static ItemListPtr p = nullptr;
		offset = item.second;
		p = (ItemListPtr)item.first;
		do
		{
			while (--offset >= 0)
				if (p->isDelete[offset] == 0)
					return { p, offset };
			p = p->left;
			offset = p->size;
		} while (p != _head);

		return headPtr;
	}

	ItemPtr YListNew::successor(ItemPtr item) const
	{
		static int offset = 0;
		static ItemListPtr p = nullptr;
		offset = item.second;
		p = (ItemListPtr)item.first;

		do
		{
			while (++offset < p->size) {
				if (p->isDelete[offset] == 0)
					return ItemPtr(p, offset);
			}
			offset = -1;
			p = p->right;
		} while (p != _end);

		return endPtr;
	}

	Id YListNew::getId(ItemPtr item) const {
		return item.first->headId + item.second;
	}


	ItemPtr YListNew::_getItemByIndex(Index index) const {
		static ItemPtr p = std::make_pair(nullptr, 0);
		for (index += 2, p = this->headPtr; --index;)
			p = this->successor(p);
		return p;
	}



	inline bool YListNew::_isEnd(ItemPtr item)const {
		int _size = ((ItemListPtr)item.first)->size;
		return (item.second + 1) == _size;
	}

	inline bool YListNew::_isBegin(ItemPtr item)const {
		return item.second == 0;
	}

	inline Id YListNew::_idByItemPtr(ItemPtr item) const {
		return item.first->headId + item.second;
	}

	ItemPtr YListNew::_truePredecessor(ItemPtr item) const
	{
		if (item.second != 0)
			--item.second;
		else {
			item.first = ((ItemListPtr)item.first)->left;
			item.second = ((ItemListPtr)item.first)->size - 1;
		}
		return item;
	}

	bool YListNew::_findCache(int index, ItemPtr& itemPtr) const
	{
		static int i = 0;
		for (i = 0; i < cacheSize; ++i) {
			if (_cachePos[i] == index)
			{
				//YJS_DEBUG("CACHE");
				itemPtr = _cachePtr[i];
				return true;
			}
		}
		return false;
	}

	bool YListNew::_findCache2(int index, ItemPtr& itemPtr)
	{
		static int i = 0;
		for (i = 0; i < cacheSize; ++i) {
			if (_cachePos[i] == index)
			{
				//YJS_DEBUG("CACHE");
				itemPtr = _cachePtr[i];

				--_cachePos[i];
				_cachePtr[i] = predecessor(_cachePtr[i]);

				return true;
			}
		}
		return false;
	}

	void YListNew::_insertCache(int index, ItemPtr& itemPtr) {
		static int _cacheNext = 0;
		_cachePos[_cacheNext] = index;
		_cachePtr[_cacheNext] = itemPtr;
		_cacheNext = (_cacheNext + 1) % cacheSize;
	};

	void YListNew::_refreshCache(int index)
	{
		for (int i = 0; i < cacheSize; ++i)
			if (_cachePos[i] >= index) {
				++_cachePos[i];
			}
				
	}

	void YListNew::_refreshCache2(int index) {
		for (int i = 0; i < cacheSize; ++i)
			if (_cachePos[i] > index) {
				--_cachePos[i];
			}
	}

	//ItemPtr YList::_getItemByIndex(Index index) const
	//{
	//	int offset = index;
	//	ItemList* p = this->head->right;

	//	while (offset > p->realSize) {
	//		offset -= p->realSize;
	//		p = p->right;
	//	}

	//	for (int i = 0; i < offset; ++i)
	//	{
	//		if (p->isDelete[i] == 0) {
	//			offset
	//		}

	//	}


	//	ItemPtr _yPtr = this->begin();
	//	while (index) {
	//		_yPtr = _yPtr->right;
	//		//if (_yPtr == this->head)
	//		//	return nullptr;
	//		if (!_yPtr->isDelete)
	//			--index;
	//	}
	//	return _yPtr;
	//};
}