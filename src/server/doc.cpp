#include "src/server/doc.h"

namespace YJS_NAMESPACE{
	Doc::Doc(YAlloc yAlloc):
		yStruct(nullptr)
	{
		switch (yAlloc)
		{
		case LIST:
			yStruct = new YList;
			break;
		case BTREE:
			yStruct = new BPlusTree;
			break;
		case LISTNEW:
			yStruct = new YListNew;
			break;
		case AVL:
			yStruct = new AVLTree;
		default:
			break;
		}

		
	}
	Doc::~Doc()
	{
		delete yStruct;
	}

	void Doc::localInsert(Index index, char context)
	{
		//ItemPtr _insertPtr = yStruct->getItemByPos(index);
		//ItemPtr _insertNextPtr = yStruct->successor(_insertPtr);
		// =======================================
		ItemMessage _insertItemMsg(
			context,
			++localId,
			Id(-1,0),
			Id(-1,0)
		);

		yStruct->insertItem(index, _insertItemMsg);

	}

	void Doc::localDelete(Index index)
	{
		yStruct->deleteItem(index);
	}



	std::string Doc::getText()
	{
		ItemPtr _headTemp = this->yStruct->begin();
		std::string ans;
		while (_headTemp!=this->yStruct->end()) {
			ans += _headTemp.first->context[_headTemp.second];

			_headTemp = yStruct->successor(_headTemp);
		}
		return ans;
	}
}


