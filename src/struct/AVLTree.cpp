#include "AVLTree.h"

int AVLNode::SPACE_SIZE = 20;
/*
64位系统：
    AVLNode中4个static int计数变量:16B
    AVLTree中，两个指针：16B
    每创建一个AVLNode会新增： 64+SPACE_SIZE*2 B，在构造函数中累计
    通过 AVLTree::getMemory()获取
*/
double AVLNode::ByteS = 16 + 16; 
int AVLNode::NODE_COUNT = 0;  //总结点数：构造函数中累加
int AVLNode::SPLIT_NODE = 0; //分裂产生的结点数，insertByPos中，分裂情况下累加

AVLNode::AVLNode() : 
    height(1), informations(SPACE_SIZE),
    left(nullptr), right(nullptr), parent(nullptr){
        context.resize(SPACE_SIZE);

        AVLNode::ByteS += 64+SPACE_SIZE*2;
        ++NODE_COUNT;
}


AVLNode::AVLNode(const ItemMessage &message) : 
    ItemListInterface(message), informations(SPACE_SIZE),
    height(1), validDataSize(1), allDataSize(1), treeSize(1),
    left(nullptr), right(nullptr), parent(nullptr) {

    context.resize(SPACE_SIZE);
    this->informations[0] = CharacterInfo(message);

    AVLNode::ByteS += 64+SPACE_SIZE*2;
    ++NODE_COUNT;
};

AVLNode::AVLNode(const vector<ItemMessage> &message) : 
    // headId(message[0].id), headOrigin(message[0].origin), endRightOrigin(message.back().rightOrigin),
    informations(SPACE_SIZE),
    left(nullptr), right(nullptr), parent(nullptr) {

    headId = message[0].id;
    headOrigin = message[0].origin;
    endRightOrigin = message.back().rightOrigin;
    context.resize(SPACE_SIZE);
    
    int length = message.size();

    for (int i = 0; i < length; ++i) {
        this->context[i] = message[i].character;
        this->informations[i] = CharacterInfo(message[i]);
    }

    this->height = 1;
    this->validDataSize = length;
    this->allDataSize = length;
    this->treeSize = length;

    AVLNode::ByteS += 64+SPACE_SIZE*2;
    ++NODE_COUNT;
}

AVLNode::AVLNode(const Id &gid, char character):
    // headId(gid), context(SPACE_SIZE), 
    informations(SPACE_SIZE), 
    height(1), validDataSize(1), allDataSize(1), treeSize(1),
    left(nullptr), right(nullptr), parent(nullptr) {

    headId = gid; context.resize(SPACE_SIZE);
    this->context[0] = character;
    this->informations[0] = CharacterInfo();
    //TODO: debug _successor()
    this->informations[0].setDeleted(false);

    AVLNode::ByteS += 64+SPACE_SIZE*2;
    ++NODE_COUNT;
}

bool AVLNode::isDeleted(int offset) {
    if (offset < 0) {
        cout<<"error! exception occurred in isDeleted()"<<endl;
    }
    return informations[offset].getisDeleted();
}

ItemPtr AVLNode::getValidByIndex(int validIndex) {
    //TODO:返回的是空对象，所以外部要判空
    if (validIndex < 0) {
        return ItemPtr();
    }
    //正常流程
    AVLNode* node = this;
    ItemPtr itemptr;
    int offset = 0;
    while (validIndex >= 0) {
        if (isDeleted(offset++)) {
            continue;
        }
        validIndex--;
    }
    itemptr.first = this;
    itemptr.second = offset - 1;

    return itemptr;
}

bool AVLNode::deleteCharacterByOffset(int offset) {
    if (!informations[offset].getisDeleted()) {
        informations[offset].setDeleted(true);
        this->setTreeSize(this->getTreeSize() - 1);
        this->setValidDataSize(this->getValidDataSize() - 1);
        return true;
    }
    return false;
}

Id AVLNode::getIdByOffset(int offset) {
    Id gid;
    gid.client = this->headId.client;
    gid.clock = this->headId.clock + offset;
    return gid;
}

int AVLNode::getIdCounterByOffset(int offset) {
    return this->headId.clock + offset;
}

bool AVLNode::insertCharacter(int offset, const ItemMessage &message) {
    context[offset] = message.character;
    informations[offset] = CharacterInfo(message);
    this->treeSize++;
    this->validDataSize++;
    this->allDataSize++;
    this->endRightOrigin = message.rightOrigin;
    return true;
}

void AVLNode::frontSplit(int offset) {
    int validDataSize = 0;
    for (int i = offset; i < SPACE_SIZE; ++i) {
        //TODO:是否必要
        context[i] = '\0';
        informations[i] = CharacterInfo();
    }
    for (int i = 0; i < offset; ++i) {
        if (!isDeleted(i)) {
            validDataSize++;
        }
    }
    this->treeSize = this->treeSize - this->allDataSize + offset;
    this->validDataSize = validDataSize;
    this->allDataSize = offset;
    this->endRightOrigin = computeRightOriginId(offset-1);
}

AVLNode* AVLNode::subAVLNode(int begin, int end) {
    AVLNode* eden = new AVLNode();
    Id edenId;
    int validDataSize = 0;

    edenId.client = this->headId.client;
    edenId.clock = this->headId.clock + begin;
    eden->setId(edenId);

    for (int i = begin; i < end; ++i) {
        if (!isDeleted(i)) {
            validDataSize++;
        }
        eden->context[i - begin] = this->context[i];
        eden->informations[i - begin] = this->informations[i];
    }
    eden->validDataSize = validDataSize;

    eden->height = 1;
    eden->allDataSize = end - begin;
    eden->treeSize = end - begin;
    eden->headOrigin = computeLeftOriginId(begin);
    eden->endRightOrigin = computeRightOriginId(end-1);

    return eden;
}

Id AVLNode::computeLeftOriginId(int offset) {
    if(offset == 0) {
        return this->headOrigin;
    } else {
        //获取初始元素的clock
        int clock = this->headId.clock;
        int clientId = this->headId.client;
        clock = clock+offset-1;
        return Id(clientId, clock);
    }
} 

Id AVLNode::computeRightOriginId(int offset) {
    if (offset == this->allDataSize - 1) {
        return this->endRightOrigin;
    } else {
        int clock = this->headId.clock;
        int clientId = this->headId.client;
        clock = clock + offset + 1;
        return Id(clientId, clock);
    }
}

/*--------------AVLTree-----------------*/

AVLTree::AVLTree():
    virtualRoot(new AVLNode()),root(nullptr) {
}

AVLNode* AVLTree::getRoot() const {
    return root;
}

int AVLTree::getHeight(AVLNode* node) const {
    if (node == nullptr)
        return 0;
    return node->getHeight();
}

int AVLTree::getTreeSize(AVLNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    return node->getTreeSize();
}

int AVLTree::getAllDataSize(AVLNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    return node->getAllDataSize();
}

int AVLTree::getValidDataSize(AVLNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    return node->getValidDataSize();
}

int AVLTree::getBalance(AVLNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    return getHeight(node->right) - getHeight(node->left);
}

ItemPtr AVLTree::_begin() const {
    if (root == nullptr) {
        return ItemPtr();
    }
    AVLNode* node = root;
    while (node->left != nullptr) {
        node = node->left;
    }
    ItemPtr response;

    while (true) {
        //TODO：判空操作
        if(node == nullptr) return {nullptr,-1};

        if (node->getValidDataSize() > 0) {
            response = node->getValidByIndex(0);
            return response;
        } else {
            node = getSuccessorNode(node);
        }
    }
}

ItemPtr AVLTree::_end() const {
    // if (root == nullptr) {
    //     return ItemPtr();
    // }
    // AVLNode* node = root;
    // while (node->right != nullptr) {
    //     node = node->right;
    // }
    // ItemPtr response;

    // while (true) {
    //     //TODO：判空操作
    //     if(node == nullptr) return {nullptr,-1};

    //     if (node->getValidDataSize() > 0) {
    //         response = node->getValidByIndex(node->validDataSize - 1);
    //         //TODO: 左闭右开区间
    //         return {response.first, response.second+1};
    //     } else {
    //         node = getPredecessorNode(node);
    //     }
    // }
    return {nullptr, 0};
}

void AVLTree::update(AVLNode* node) {
    //如果正好传入的节点是root的父节点
    if (node == nullptr) {
        return;
    }
    node->setHeight(1 + max(getHeight(node->left), getHeight(node->right)));
    node->setTreeSize(getTreeSize(node->left) + getTreeSize(node->right) + getValidDataSize(node));

    AVLNode* p = node->parent;
    //TODO：避免死循环
    while (p != nullptr) {
        p->setHeight(1 + max(getHeight(p->left), getHeight(p->right)));
        p->setTreeSize(getTreeSize(p->left) + getTreeSize(p->right) + getValidDataSize(p));
        // if(p == p->parent) break;
        p = p->parent;
    }
}

void AVLTree::fullRebalance(AVLNode* node) {
    if (node == nullptr) {
        return;
    }
    rebalance(node, node->parent);
    AVLNode* p = node->parent;
    while (p != nullptr) {
        rebalance(p, p->parent);
        p = p->parent;
    }
}

AVLNode* AVLTree::leftRotation(AVLNode* node) {
    //维护根节点
    bool isRoot = false;
    if (node == this->root) {
        isRoot = true;
    }

    AVLNode* right = node->right;
    AVLNode* rightLeft = right->left;
    AVLNode* nodeParent = node->parent;

    node->right = rightLeft;
    if (rightLeft != nullptr) {
        rightLeft->parent = node;
    }

    right->left = node;
    node->parent = right;

    right->parent = nodeParent;
    // if (nodeParent != nullptr) {
    //     //TODO:DEBUG
    //     nodeParent->right = right;
    // }

    //更新root必须放在两个update之前！！更新依赖于正确的根节点
    if (isRoot) {
        this->root = right;
    }

    //注意update的顺序，自底向上update
    // update(node);
    // update(right);

    return right;
}

AVLNode* AVLTree::rightRotation(AVLNode* node) {
    //维护根节点
    bool isRoot = false;
    if (node == this->root) {
        isRoot = true;
    }

    AVLNode* left = node->left;
    AVLNode* leftRight = left->right;
    AVLNode* nodeParent = node->parent;

    node->left = leftRight;
    if (leftRight != nullptr) {
        leftRight->parent = node;
    }

    left->right = node;
    node->parent = left;

    left->parent = nodeParent;
    // if (nodeParent != nullptr) {
    //     //TODO:DEBUG
    //     nodeParent->left = left;
    // }

    //更新root必须放在两个update之前！！
    if (isRoot) {
        this->root = left;
    }

    // update(node);
    // update(left);

    return left;
}

AVLNode* AVLTree::rebalance(AVLNode* node, AVLNode* parent) {
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);
    if (balance > 1) {
        //如果右子树高
        if (getHeight(node->right->right) > getHeight(node->right->left)) {
            //如果为RR不平衡
            // node = leftRotation(node);

            AVLNode* nodeParent = node->parent;
            bool isLeftLeaf = nodeParent!=nullptr&&nodeParent->left==node ? true : false;
            AVLNode* newNode = leftRotation(node);
            // if(nodeParent) nodeParent->right = newNode;
            if(nodeParent) {
                if(isLeftLeaf)
                    nodeParent->left = newNode;
                else 
                    nodeParent->right = newNode;
            }
            update(node); update(node->parent);
            // update(node); update(node->right);
            node = newNode;
        } else {
            //如果为RL不平衡
            AVLNode* oldNode = node->right;
            node->right = rightRotation(node->right);
            node->right->parent = node;
            update(oldNode); update(oldNode->parent);
            // update(oldNode); update(oldNode->left);

            // node = leftRotation(node);
            AVLNode* nodeParent = node->parent;
            bool isLeftLeaf = nodeParent!=nullptr&&nodeParent->left==node ? true : false;
            AVLNode* newNode = leftRotation(node);
            // if(nodeParent) nodeParent->right = newNode;
            if(nodeParent) {
                if(isLeftLeaf)
                    nodeParent->left = newNode;
                else 
                    nodeParent->right = newNode;
            }
            update(node); update(node->parent);
            // update(node); update(node->right);
            node = newNode;
        }
    } else if (balance < -1) {
        //如果左子树高
        if (getHeight(node->left->left) > getHeight(node->left->right)) {
            // //如果是LL不平衡
            // node = rightRotation(node);

            AVLNode* nodeParent = node->parent;
            bool isLeftLeaf = nodeParent!=nullptr&&nodeParent->left==node ? true : false;
            AVLNode* newNode = rightRotation(node);
            // if(nodeParent) nodeParent->left = newNode;
            if(nodeParent) {
                if(isLeftLeaf)
                    nodeParent->left = newNode;
                else 
                    nodeParent->right = newNode;
            }
            update(node); update(node->parent);
            // update(node); update(node->left);
            node = newNode;
        } else {
            //如果是LR不平衡
            AVLNode* oldNode = node->left;
            node->left = leftRotation(node->left);
            node->left->parent = node;
            update(oldNode); update(oldNode->right);

            // node = rightRotation(node);
            AVLNode* nodeParent = node->parent;
            bool isLeftLeaf = nodeParent!=nullptr&&nodeParent->left==node ? true : false;
            AVLNode* newNode = rightRotation(node);
            if(nodeParent) {
                if(isLeftLeaf)
                    nodeParent->left = newNode;
                else 
                    nodeParent->right = newNode;
            }
            update(node); update(node->parent);
            // update(node); update(node->left);
            node = newNode;
        }
    }

    return node;
}


/**
 * 三种插入情况：
 * * 找到了待插入的Node，发现GId和该Node的末尾字符是连续的，直接修改Node属性
 * * 找到了待插入的Node，发现pos正好处于末尾字符的下一位，但是GId不连续，此时找到该Node的后继Node，并在其左子树上new新Node，并对树结构进行维护
 * * 找到了待插入的Node，发现pos正好处于末尾字符中间，分裂原Node为a，b，将b插入到下一个位置，维护树的平衡，并再new一个新节点c，再插入到a的下一个位置，维护书店
 * 维护树的平衡
 * 之所以不采用递归的方式进行插入，是因为后续存在根据Id找到节点的需求，递归从根本上无法实现
 *
 * 可能出现的问题：可能在某个节点的第一个字符处插入
 * @param pos
 */
void AVLTree::insertByPos(int pos, const ItemMessage &message) {
    if (root == nullptr) {
        root = new AVLNode(message);
        return;
    }
    AVLNode* eden;

    //如果是在文章开头插入
    if (pos == 0) {
        AVLNode* beginNode = getNodeByPos(root, pos);
        eden = new AVLNode(message);
        beginNode->left = eden;
        eden->parent = beginNode;
        update(beginNode);
        rebalance(beginNode, beginNode->parent);
        return;
    }
    //找到上一个字符的所处的Node和ItemPtr
    AVLNode* preCharacterNode = getNodeByPos(root, pos - 1);
    ItemPtr preCharacterItemPtr = getItemPtrByPos(pos - 1);
    //TODO: 判空操作
    if(preCharacterItemPtr.first == nullptr) return;
    //获取上一个字符的Counter（无论是否被删除）
    int preCounter = preCharacterNode->getIdCounterByOffset(preCharacterItemPtr.second);
    int messageCounter = message.id.clock;
    //如果两个Id是连续的
    if (preCounter == messageCounter - 1) {
        //如果数组没满
        if (preCharacterNode->getAllDataSize() < AVLNode::SPACE_SIZE) {
            preCharacterNode->insertCharacter(preCharacterItemPtr.second + 1, message);
            update(preCharacterNode);
        //如果数组是满的，则需要new一个新的Node并插入到下一个位置
        } else {
            eden = new AVLNode(message);
            //上一个字符所在Node的后继Node
            AVLNode* preNextNode =  getNextInsertPosNode(preCharacterNode);
            if (preNextNode == preCharacterNode) {
                preNextNode->right = eden;
                eden->parent = preNextNode;
            } else {
                preNextNode->left = eden;
                eden->parent = preNextNode;
            }
            //更新size和height
            update(preNextNode);
            fullRebalance(preNextNode);
        }
        //如果两个Id是不连续的
    } else {
        //如果恰好上一个字符是其所处的字符的末尾，new一个新节点并放在应该所处的位置
        //对应待插入字符要插入到某个Node的数组中的第一个位置
        if (preCharacterNode->getAllDataSize() - 1 == preCharacterItemPtr.second) {
            eden = new AVLNode(message);

            //上一个字符所在Node的后继Node
            AVLNode* preCharacterNodeNextNode = getNextInsertPosNode(preCharacterNode);
            if (preCharacterNodeNextNode == preCharacterNode) {
                preCharacterNodeNextNode->right = eden;
                eden->parent = preCharacterNodeNextNode;
            } else {
                preCharacterNodeNextNode->left = eden;
                eden->parent = preCharacterNodeNextNode;
            }
            //更新size和height
            update(preCharacterNodeNextNode);
            fullRebalance(preCharacterNodeNextNode);

        //如果要插入的位置处于Node的字符的中间，则Node分裂，更新原Node，并将分裂后的节点插入到原来节点的下一个节点，
        //再将该字符new一个新的节点
        } else {
            ++AVLNode::SPLIT_NODE;
            //注意此处的 +1
            int begin = preCharacterItemPtr.second + 1;
            int end = preCharacterNode->getAllDataSize();
            AVLNode* backSplitNode = preCharacterNode->subAVLNode(begin, end);
            preCharacterNode->frontSplit(begin);
            //更新自该Node到root的所有节点的TreeSize
            update(preCharacterNode);

            AVLNode* preCharacterNodeNextNode = getNextInsertPosNode(preCharacterNode);
            if (preCharacterNodeNextNode == preCharacterNode) {
                preCharacterNodeNextNode->right = backSplitNode;
                backSplitNode->parent = preCharacterNodeNextNode;
            } else {
                preCharacterNodeNextNode->left = backSplitNode;
                backSplitNode->parent = preCharacterNodeNextNode;
            }

            update(preCharacterNodeNextNode);
            fullRebalance(preCharacterNodeNextNode);

            eden = new AVLNode(message);
            preCharacterNodeNextNode = getNextInsertPosNode(preCharacterNode);
            if (preCharacterNodeNextNode == preCharacterNode) {
                preCharacterNodeNextNode->right = eden;
                eden->parent = preCharacterNodeNextNode;
            } else {
                preCharacterNodeNextNode->left = eden;
                eden->parent = preCharacterNodeNextNode;
            }
            update(preCharacterNodeNextNode);
            fullRebalance(preCharacterNodeNextNode);
        }
    }
}
/**
 * 假性删除，即修改删除标记位置
 * @param pos
 */
void AVLTree::deleteByPos(int pos) {
    AVLNode* node = getNodeByPos(root, pos);

    //TODO：判空操作
    if(node == nullptr) return;

    int validIndex = getValidIndexByPos(root, pos);
    //int validIndex = pos - node->left.getTreeSize();
    ItemPtr itemptr = node->getValidByIndex(validIndex);
    node->deleteCharacterByOffset(itemptr.second);

    AVLNode* p = node->parent;
    while (p != nullptr) {
        p->setTreeSize(p->getTreeSize() - 1);
        p = p->parent;
    }

    update(node->parent);
}

ItemPtr AVLTree::getItemPtrByPos(int pos) const {
    if (pos >= root->getTreeSize() || pos < 0) {
        return ItemPtr();
    }
    AVLNode* node = getNodeByPos(root, pos);
    ItemPtr response;
    int validIndex = getValidIndexByPos(root, pos);
    response = node->getValidByIndex(validIndex);
    return response;
}

/**
 *
* @param node
* @param pos 可能为0.为整个文章中的位置
* @return
*/
AVLNode* AVLTree::getNodeByPos(AVLNode* node, int pos) const {
    if (node == nullptr) {
        return nullptr;
    }
    int curValidDataSize = getValidDataSize(node);
    int leftTreeSize = getTreeSize(node->left);
    if (pos >= leftTreeSize && pos <= leftTreeSize + curValidDataSize - 1) {
        return node;
    } else if (pos < leftTreeSize) {
        return getNodeByPos(node->left, pos);
    } else {
        return getNodeByPos(node->right, pos - leftTreeSize - curValidDataSize);
    }
}

int AVLTree::getValidIndexByPos(AVLNode* node, int pos) const {
    if (node == nullptr) {
        return 0;
    }
    int curValidDataSize = getValidDataSize(node);
    int leftTreeSize = getTreeSize(node->left);
    if (pos >= leftTreeSize && pos <= leftTreeSize + curValidDataSize - 1) {
        return pos - leftTreeSize;
    } else if (pos < leftTreeSize) {
        return getValidIndexByPos(node->left, pos);
    } else {
        return getValidIndexByPos(node->right, pos - leftTreeSize - curValidDataSize);
    }
}

/**
 * 返回直接后继字符，无论是否删除
 * 用于读取服务
 * @param itemptr
 * @return
 */
ItemPtr AVLTree::_successor(const ItemPtr &itemptr) const {
    if (itemptr.first == nullptr) {
        return ItemPtr();
    }
    //TODO：指针强转
    AVLNode* node = (AVLNode*)itemptr.first;
    int offset = itemptr.second;

    ItemPtr response;
    //获取node内数组的所有元素个数
    int allSize = node->getAllDataSize();

    //如果offset不是最后一个
    //TODO：左闭右开区间
    if (offset < allSize - 1) {
        int index;
        //TODO：左闭右开区间
        for (index = offset + 1; index < allSize; ++index) {
            if (!node->isDeleted(index)) {
                // if(index <= allSize && node->context[index] == '\0') continue;
                response.second = index;
                response.first = node;
                return response;
            }
        }
        node = this->getSuccessorNode(node);
        response = _successor(ItemPtr(node, -1));

    } else {
        node = this->getSuccessorNode(node);
        response = _successor(ItemPtr(node, -1));
    }

    return response;
}

/**
 * 获取上一个字符对应的ItemPtr
 * @param itemptr
 * @return
 */
ItemPtr AVLTree::_predecessor(const ItemPtr &itemptr) const {
    if (itemptr.first == nullptr) {
        return ItemPtr();
    }
    AVLNode* node = (AVLNode*)itemptr.first;
    int offset = itemptr.second;

    ItemPtr response;
    int resOffset = 0;
    //获取node内数组的所有元素个数
    int allSize = node->getAllDataSize();
    if (offset > 0) {
        int index;
        for (index = offset - 1; offset >= 0; offset--) {
            if (!node->isDeleted(offset)) {
                response.first = node;
                response.second = index;
                return response;
            }
        }
        node = getPredecessorNode(node);
        response = _predecessor(ItemPtr(node, node->getAllDataSize()));
    } else {
        node = getPredecessorNode(node);
        response = _predecessor(ItemPtr(node, node->getAllDataSize()));
    }
    return response;
}

AVLNode* AVLTree::getSuccessorNode(AVLNode* node) const {
    if (node->right != nullptr) {
        node = node->right;
        while (node->left != nullptr) {
            node = node->left;
        }
    } else {
        //TODO判空操作
        while (node->parent != nullptr && node->parent->left != node) {
            node = node->parent;
            if (node->parent == nullptr) {
                break;
            }
        }
        node = node->parent;
    }
    return node;
}

AVLNode* AVLTree::getPredecessorNode(AVLNode* node) const {
    if (node->left != nullptr) {
        node = node->left;
        while (node->right != nullptr) {
            node = node->right;
        }
    } else {
        //TODO判空操作
        while (node->parent != nullptr && node->parent->right != node) {
            node = node->parent;
            if (node->parent == nullptr) {
                break;
            }
        }
        node = node->parent;
    }
    return node;
}

/**
 * 如果要往某个节点插入一个节点，则调用此函数，获得正确的插入位置，使用getSuccessorNode是一个
 * 读入操作，可能会返回该节点的父亲节点或者祖先节点。此函数是为写入操作进行服务的。
 * @param node
 * @return
 */
AVLNode* AVLTree::getNextInsertPosNode(AVLNode* node) const {
    AVLNode* p = node->right;
    //如果该节点为空
    if (p == nullptr) {
        return node;
    }
    while (p->left != nullptr) {
        p = p->left;
    }
    return p;
}

AVLNode* AVLTree::getPreInsertPos(AVLNode* node) const {
    AVLNode* p = node->left;
    if (p == nullptr) {
        return node;
    }
    while (p->right != nullptr) {
        p = p->right;
    }
    return p;
}

Id AVLTree::getIdByItemPtr(ItemPtr itemptr) const {
    //TODO:指针强转
    return ((AVLNode*)itemptr.first)->getIdByOffset(itemptr.second);
}


void AVLTree::draw(AVLNode* root, AVLNode* flag) {
    vector<vector<string>> tree = levelOrder(root, flag);
    for (int i = 0; i < tree.size(); ++i) {
        vector<string> level = tree[i];
        for (int j = 0; j < level.size(); ++j) {
            cout<<level[j] << " ";
        }
        cout<<endl;
    }
}

vector<vector<string>> AVLTree::levelOrder(AVLNode* root, AVLNode* flag) {
    vector<vector<string>> list;
    if (root == nullptr) {
        return list;
    }
    deque<AVLNode*> deque;
    deque.push_back(root);

    while (!deque.empty()) {
        int num = deque.size();
        vector<string> subList;
        for (int i = 0; i < num; i++) {
            AVLNode* node = deque.front();
            deque.pop_front();
            if (node == flag) {
                subList.push_back("_ ");
            } else {
                if (node->left != nullptr) {
                    deque.push_back(node->left);
                } else {
                    deque.push_back(flag);
                }
                if (node->right != nullptr) {
                    deque.push_back(node->right);
                } else {
                    deque.push_back(flag);
                }
                string str;
                for (int j = 0; j < node->allDataSize; ++j) {
                    // if (!node->isDeleted(j)) {
                        str += node->context[j];
                    // }
                }
                subList.push_back(str);
            }
        }
        list.push_back(subList);
    }
    return list;
}

ItemPtr AVLTree::begin() const {
    ItemPtr itemptr = _begin();
    return {itemptr.first, itemptr.second};
}

ItemPtr AVLTree::end() const {
    ItemPtr itemptr = _end();
    return {itemptr.first, itemptr.second};
}

void AVLTree::insertItem(Index index, ItemMessage itemMsg) {
    insertByPos(index, itemMsg);
}

void AVLTree::deleteItem(Index index) {
    deleteByPos(index);
}

ItemPtr AVLTree::getItemByPos(Index index) const {
    ItemPtr itemptr = getItemPtrByPos(index);
    return {itemptr.first, itemptr.second};
}

ItemPtr AVLTree::successor(ItemPtr itemPtr) const {
    ItemPtr res = _successor(itemPtr);
    return {res.first, res.second};
}

ItemPtr AVLTree::predecessor(ItemPtr itemPtr) const {
    ItemPtr res = _predecessor(itemPtr);
    return {res.first, res.second};
}

Id AVLTree::getId(ItemPtr itemPtr) const {
    Id gid = getIdByItemPtr(itemPtr);
    return Id(gid.client, 0);
}

void AVLTree::printMemoryInfo() {
    cout<<"-------------AVLTree Memory Info----------------"<<endl;
    cout<<"每个结点存储的字符数: "<<AVLNode::SPACE_SIZE<<endl;
    cout<<"总内存: "<<AVLNode::ByteS/1024/1024<<"MB"<<endl;
    cout<<"总结点数: "<<AVLNode::NODE_COUNT<<endl;
    cout<<"分裂产生的结点数: "<<AVLNode::SPLIT_NODE<<endl;
}