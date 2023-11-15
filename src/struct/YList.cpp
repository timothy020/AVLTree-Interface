#include "YList.h"
#include "src/struct/YList.h"


namespace YJS_NAMESPACE {

	// ItemPtr 指向迭代器的指针

	void YList::insertItem(Index index, ItemMessage itemMsg)
	{
		ItemPtr _insertPtr = predecessor(_getItemByIndex(index));
		ItemListPtr _insertFirst = (ItemListPtr)_insertPtr.first;

		// 特化版本
		if (++_idByItemPtr(_insertPtr) == itemMsg.id) {
			_insertFirst->push_back(itemMsg);
			YJS_DEBUG("特化插入");
		}
		else {
			ItemListPtr insertNode = new ItemList(itemMsg);
			if (!_isEnd(_insertPtr)) {
				ItemListPtr _insertSecond =
					_insertFirst->subItemList(_insertPtr.second + 1);
				YJS_DEBUG("分裂");
				//static int conut = 0;
				//YJS_DEBUG("%d", conut++);

			}
			_insertFirst->right->left = insertNode;
			insertNode->right = _insertFirst->right;
			_insertFirst->right = insertNode;
			insertNode->left = _insertFirst;
			
		}

	}

	//void YList::insertItem(ItemPtr item)
	//{
	//	return;
	//}

	void YList::deleteItem(Index index)
	{
		ItemPtr _deletePtr = _getItemByIndex(index);

		if (_deletePtr == this->end())
			return;

		//--this->size;

		ItemList* p = (ItemList*)_deletePtr.first;

		
		p->isDelete[_deletePtr.second] = 1;
		// --p->realSize;
	}

	ItemPtr YList::getItemByPos(Index index) const
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



	ItemPtr YList::predecessor(ItemPtr item) const
	{
		int offset = item.second;
		ItemListPtr p = (ItemListPtr)item.first;
		do
		{
			while (--offset >= 0)
				if(p->isDelete[offset] == 0)
					return { p, offset };
			p = p->left;
			offset = p->_size;
		} while (p != _head );

		return { _head, 0 };;
	}

	ItemPtr YList::successor(ItemPtr item) const
	{
		int offset = item.second;
		ItemListPtr p = (ItemListPtr)item.first;
		do
		{
			while (++offset < p->_size) {
				if(p->isDelete[offset] == 0)
					return { p, offset };
			}
				
			offset = -1;
			p = p->right;
		} while (p != end().first);

		return this->end();
	}

	Id YList::getId(ItemPtr item) const {
		return item.first->headId + item.second;
	}


	ItemPtr YList::_getItemByIndex(Index index) const {
		ItemPtr p = this->begin();
		while (p != this->end() && index--)
			p = this->successor(p);
		return p;
	}

	bool YList::_isEnd(ItemPtr item)const {
		int _size = ((ItemListPtr)item.first)->_size - 1;
		return item.second == _size || _size == -1;
	}

	bool YList::_isBegin(ItemPtr item)const {
		return item.second == 0;
	}

	Id YList::_idByItemPtr(ItemPtr item) const {
		return item.first->headId+item.second;
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