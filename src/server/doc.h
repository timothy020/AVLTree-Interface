#pragma once

#include "src/struct/YList.h"

#include "src/struct/BPlusTree.h"

#include "src/struct/AVLTree.h"

#include "src/struct/YListNew.h"

#include "src/server/transaction.h"

#include <string>


namespace YJS_NAMESPACE {

	enum YAlloc
	{
		LIST,
		BTREE,
		LISTNEW,
		AVL
	};

	class Doc{
	public:
		Doc(YAlloc);
		~Doc();

		Id localId = {1, 0};

	public:
		
		YInterface* yStruct;

		void localInsert(Index index, char context);

		void localDelete(Index index);

		std::string getText();

	};
	

};