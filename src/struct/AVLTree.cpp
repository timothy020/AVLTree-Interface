#include "AVLTree.h"

int AVLNode::SPACE_SIZE = 20;
/*
64λϵͳ��
    AVLNode��4��static int��������:16B
    AVLTree�У�����ָ�룺16B
    ÿ����һ��AVLNode�������� 64+SPACE_SIZE*2 B���ڹ��캯�����ۼ�
    ͨ�� AVLTree::getMemory()��ȡ
*/
double AVLNode::ByteS = 16 + 16; 
int AVLNode::NODE_COUNT = 0;  //�ܽ���������캯�����ۼ�
int AVLNode::SPLIT_NODE = 0; //���Ѳ����Ľ������insertByPos�У�����������ۼ�

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
    //TODO:���ص��ǿն��������ⲿҪ�п�
    if (validIndex < 0) {
        return ItemPtr();
    }
    //��������
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
        //TODO:�Ƿ��Ҫ
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
        //��ȡ��ʼԪ�ص�clock
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
        //TODO���пղ���
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
    //     //TODO���пղ���
    //     if(node == nullptr) return {nullptr,-1};

    //     if (node->getValidDataSize() > 0) {
    //         response = node->getValidByIndex(node->validDataSize - 1);
    //         //TODO: ����ҿ�����
    //         return {response.first, response.second+1};
    //     } else {
    //         node = getPredecessorNode(node);
    //     }
    // }
    return {nullptr, 0};
}

void AVLTree::update(AVLNode* node) {
    //������ô���Ľڵ���root�ĸ��ڵ�
    if (node == nullptr) {
        return;
    }
    node->setHeight(1 + max(getHeight(node->left), getHeight(node->right)));
    node->setTreeSize(getTreeSize(node->left) + getTreeSize(node->right) + getValidDataSize(node));

    AVLNode* p = node->parent;
    //TODO��������ѭ��
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
    //ά�����ڵ�
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

    //����root�����������update֮ǰ����������������ȷ�ĸ��ڵ�
    if (isRoot) {
        this->root = right;
    }

    //ע��update��˳���Ե�����update
    // update(node);
    // update(right);

    return right;
}

AVLNode* AVLTree::rightRotation(AVLNode* node) {
    //ά�����ڵ�
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

    //����root�����������update֮ǰ����
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
        //�����������
        if (getHeight(node->right->right) > getHeight(node->right->left)) {
            //���ΪRR��ƽ��
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
            //���ΪRL��ƽ��
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
        //�����������
        if (getHeight(node->left->left) > getHeight(node->left->right)) {
            // //�����LL��ƽ��
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
            //�����LR��ƽ��
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
 * ���ֲ��������
 * * �ҵ��˴������Node������GId�͸�Node��ĩβ�ַ��������ģ�ֱ���޸�Node����
 * * �ҵ��˴������Node������pos���ô���ĩβ�ַ�����һλ������GId����������ʱ�ҵ���Node�ĺ��Node����������������new��Node���������ṹ����ά��
 * * �ҵ��˴������Node������pos���ô���ĩβ�ַ��м䣬����ԭNodeΪa��b����b���뵽��һ��λ�ã�ά������ƽ�⣬����newһ���½ڵ�c���ٲ��뵽a����һ��λ�ã�ά�����
 * ά������ƽ��
 * ֮���Բ����õݹ�ķ�ʽ���в��룬����Ϊ�������ڸ���Id�ҵ��ڵ�����󣬵ݹ�Ӹ������޷�ʵ��
 *
 * ���ܳ��ֵ����⣺������ĳ���ڵ�ĵ�һ���ַ�������
 * @param pos
 */
void AVLTree::insertByPos(int pos, const ItemMessage &message) {
    if (root == nullptr) {
        root = new AVLNode(message);
        return;
    }
    AVLNode* eden;

    //����������¿�ͷ����
    if (pos == 0) {
        AVLNode* beginNode = getNodeByPos(root, pos);
        eden = new AVLNode(message);
        beginNode->left = eden;
        eden->parent = beginNode;
        update(beginNode);
        rebalance(beginNode, beginNode->parent);
        return;
    }
    //�ҵ���һ���ַ���������Node��ItemPtr
    AVLNode* preCharacterNode = getNodeByPos(root, pos - 1);
    ItemPtr preCharacterItemPtr = getItemPtrByPos(pos - 1);
    //TODO: �пղ���
    if(preCharacterItemPtr.first == nullptr) return;
    //��ȡ��һ���ַ���Counter�������Ƿ�ɾ����
    int preCounter = preCharacterNode->getIdCounterByOffset(preCharacterItemPtr.second);
    int messageCounter = message.id.clock;
    //�������Id��������
    if (preCounter == messageCounter - 1) {
        //�������û��
        if (preCharacterNode->getAllDataSize() < AVLNode::SPACE_SIZE) {
            preCharacterNode->insertCharacter(preCharacterItemPtr.second + 1, message);
            update(preCharacterNode);
        //������������ģ�����Ҫnewһ���µ�Node�����뵽��һ��λ��
        } else {
            eden = new AVLNode(message);
            //��һ���ַ�����Node�ĺ��Node
            AVLNode* preNextNode =  getNextInsertPosNode(preCharacterNode);
            if (preNextNode == preCharacterNode) {
                preNextNode->right = eden;
                eden->parent = preNextNode;
            } else {
                preNextNode->left = eden;
                eden->parent = preNextNode;
            }
            //����size��height
            update(preNextNode);
            fullRebalance(preNextNode);
        }
        //�������Id�ǲ�������
    } else {
        //���ǡ����һ���ַ������������ַ���ĩβ��newһ���½ڵ㲢����Ӧ��������λ��
        //��Ӧ�������ַ�Ҫ���뵽ĳ��Node�������еĵ�һ��λ��
        if (preCharacterNode->getAllDataSize() - 1 == preCharacterItemPtr.second) {
            eden = new AVLNode(message);

            //��һ���ַ�����Node�ĺ��Node
            AVLNode* preCharacterNodeNextNode = getNextInsertPosNode(preCharacterNode);
            if (preCharacterNodeNextNode == preCharacterNode) {
                preCharacterNodeNextNode->right = eden;
                eden->parent = preCharacterNodeNextNode;
            } else {
                preCharacterNodeNextNode->left = eden;
                eden->parent = preCharacterNodeNextNode;
            }
            //����size��height
            update(preCharacterNodeNextNode);
            fullRebalance(preCharacterNodeNextNode);

        //���Ҫ�����λ�ô���Node���ַ����м䣬��Node���ѣ�����ԭNode���������Ѻ�Ľڵ���뵽ԭ���ڵ����һ���ڵ㣬
        //�ٽ����ַ�newһ���µĽڵ�
        } else {
            ++AVLNode::SPLIT_NODE;
            //ע��˴��� +1
            int begin = preCharacterItemPtr.second + 1;
            int end = preCharacterNode->getAllDataSize();
            AVLNode* backSplitNode = preCharacterNode->subAVLNode(begin, end);
            preCharacterNode->frontSplit(begin);
            //�����Ը�Node��root�����нڵ��TreeSize
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
 * ����ɾ�������޸�ɾ�����λ��
 * @param pos
 */
void AVLTree::deleteByPos(int pos) {
    AVLNode* node = getNodeByPos(root, pos);

    //TODO���пղ���
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
* @param pos ����Ϊ0.Ϊ���������е�λ��
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
 * ����ֱ�Ӻ���ַ��������Ƿ�ɾ��
 * ���ڶ�ȡ����
 * @param itemptr
 * @return
 */
ItemPtr AVLTree::_successor(const ItemPtr &itemptr) const {
    if (itemptr.first == nullptr) {
        return ItemPtr();
    }
    //TODO��ָ��ǿת
    AVLNode* node = (AVLNode*)itemptr.first;
    int offset = itemptr.second;

    ItemPtr response;
    //��ȡnode�����������Ԫ�ظ���
    int allSize = node->getAllDataSize();

    //���offset�������һ��
    //TODO������ҿ�����
    if (offset < allSize - 1) {
        int index;
        //TODO������ҿ�����
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
 * ��ȡ��һ���ַ���Ӧ��ItemPtr
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
    //��ȡnode�����������Ԫ�ظ���
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
        //TODO�пղ���
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
        //TODO�пղ���
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
 * ���Ҫ��ĳ���ڵ����һ���ڵ㣬����ô˺����������ȷ�Ĳ���λ�ã�ʹ��getSuccessorNode��һ��
 * ������������ܻ᷵�ظýڵ�ĸ��׽ڵ�������Ƚڵ㡣�˺�����Ϊд��������з���ġ�
 * @param node
 * @return
 */
AVLNode* AVLTree::getNextInsertPosNode(AVLNode* node) const {
    AVLNode* p = node->right;
    //����ýڵ�Ϊ��
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
    //TODO:ָ��ǿת
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
    cout<<"ÿ�����洢���ַ���: "<<AVLNode::SPACE_SIZE<<endl;
    cout<<"���ڴ�: "<<AVLNode::ByteS/1024/1024<<"MB"<<endl;
    cout<<"�ܽ����: "<<AVLNode::NODE_COUNT<<endl;
    cout<<"���Ѳ����Ľ����: "<<AVLNode::SPLIT_NODE<<endl;
}