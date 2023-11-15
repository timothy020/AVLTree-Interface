#pragma once
#include <string>
#include <deque>
#include <map>
#include <vector>
#include <iostream>
#include "src/item/id.h"
#include "src/struct/YStruct.h"
using namespace std;
using namespace YJS_NAMESPACE;

namespace YJS_NAMESPACE {

class AVLNode; //前置声明

class CharacterInfo {
private:
    //YATA算法需要的数据
    // Id leftOriginId;
    //YATA算法需要的数据
    // Id rightOriginId;
    //是否删除的标记，为true则为被删除（假删），为false则未被删除
    bool isDeleted;

public:
    CharacterInfo() {
        // isDeleted = false;
        // TODO: debug _successor()
        isDeleted = true;
    }
    CharacterInfo(const ItemMessage &message):
        // leftOriginId(message.origin), 
        // rightOriginId(message.rightOrigin),  
        isDeleted(false) {
    }
    // Id getLeftOriginId()  const {return leftOriginId;}
    // Id getRightOriginId() const {return rightOriginId;}
    bool getisDeleted()     const {return isDeleted;}
    void setDeleted(bool isDeleted) {this->isDeleted = isDeleted;}
};

class AVLNode : public ItemListInterface {
public:
    //设置3只是为了方便测试，可以设置为8、10、16或者其他
    static int SPACE_SIZE; //类外初始化
    static double ByteS; //记录数据结构占用多少字节
    static int NODE_COUNT; //总结点数
    static int SPLIT_NODE; //分裂产生的结点数
    //该节点初始元素的Id，无论是否被删除
    // Id gid;
    //用于存储实际的字符数据，大小暂定为10
    // vector<char> text; 
    //用于记录每个字符的信息，是否被删除，原始前驱和原始后继
    vector<CharacterInfo> informations;
    //该节点所在的高度，用于AVL树的平衡
    int height;
    //数组中合法数据的大小
    int validDataSize;
    //数组中所有的数据大小，即包括被删除的数据
    int allDataSize;

    //以该节点为根节点的树的有效值的个数!!用于根据pos对节点进行搜索
    int treeSize;
    //左节点
    AVLNode *left;
    //右节点
    AVLNode *right;
    //节点的父指针
    AVLNode *parent;
    //客户端传来的数据。用于YATA算法，第一个字符的前驱
    // Id headOrigin;
    //客户端传来的数据。用于YATA算法，最后一个字符（无论是否被标记删除）的后继，无论是否被删除
    // Id endRightOrigin;

public:
    int getTreeSize()       const {return treeSize;}
    int getValidDataSize()  const {return validDataSize;}
    int getAllDataSize()    const {return allDataSize;}
    int getHeight()         const {return height;}
    string getText()        const {return context;}
    AVLNode* getLeft()      const {return left;}
    AVLNode* getRight()     const {return right;}

    void setTreeSize(int treeSize)              {this->treeSize = treeSize;}
    void setValidDataSize(int validDataSize)    {this->validDataSize = validDataSize;}
    void setId(Id gid)                          {this->headOrigin = gid;}
    void setHeight(int height)                  {this->height = height;}

    AVLNode();
    AVLNode(const ItemMessage &message);
    AVLNode(const vector<ItemMessage> &message);
    AVLNode(const Id &gid, char character);

    bool isDeleted(int offset);
    /**
     * 接口待测试
     * 可能会出现死循环
     * @param validIndex 表示未被删除的节点，从0开始计数
     * @return
     */
    ItemPtr getValidByIndex(int validIndex);
    bool deleteCharacterByOffset(int offset);
    Id getIdByOffset(int offset);
    int getIdCounterByOffset(int offset);
    bool insertCharacter(int offset, const ItemMessage &message);

    /**
     * 在节点分裂时更新自身，采用安全的方式,擦除[offset, SPACE_SIZE]后的数据.
     * @param offset 假设为5，则保留下标为0至下标为4的节点
     */
    void frontSplit(int offset);

    /**
     * 获取一个节点的子节点，并不加入到树结构，即不会继承height，size等属性
     * 左开右闭，从0开始计数
     * @param begin
     * @param end
     * @return
     */
    AVLNode* subAVLNode(int begin, int end);
    Id computeLeftOriginId(int offset);
    Id computeRightOriginId(int offset);
};

class AVLTree : public YInterface {
private:
    //虚拟树的头节点，参考红黑树的实现
    AVLNode* virtualRoot;
    AVLNode* root;
    //暂未使用，后续开发
    // map<Id, ItemPtr> gidToItemPtr;
public:
    AVLTree();
    AVLNode* getRoot() const;

    int getHeight(AVLNode* node) const;
    int getTreeSize(AVLNode* node) const;
    int getAllDataSize(AVLNode* node) const;
    int getValidDataSize(AVLNode* node) const;
    int getBalance(AVLNode* node) const;

    //把功能委托给现有函数
    virtual ItemPtr begin() const override;
    virtual ItemPtr end() const override;
    virtual void insertItem(Index index, ItemMessage itemMsg) override;
    virtual void deleteItem(Index index) override;
    virtual ItemPtr getItemByPos(Index index) const override;
    virtual ItemPtr successor(ItemPtr itemPtr) const override;
    virtual ItemPtr predecessor(ItemPtr itemPtr) const override;
    virtual Id getId(ItemPtr itemPtr) const override;

    ItemPtr _begin() const;
    ItemPtr _end() const;

    void insertByPos(int pos, const ItemMessage &message);
    void deleteByPos(int pos);

    AVLNode* leftRotation(AVLNode* node);
    AVLNode* rightRotation(AVLNode* node);

    ItemPtr getItemPtrByPos(int pos) const; 
    AVLNode* getNodeByPos(AVLNode* node, int pos) const;
    int getValidIndexByPos(AVLNode* node, int pos) const;

    ItemPtr _successor(const ItemPtr &nodeOffset) const;
    ItemPtr _predecessor(const ItemPtr &nodeOffset) const;


    AVLNode* getSuccessorNode(AVLNode* node) const;
    AVLNode* getPredecessorNode(AVLNode* node) const;
    AVLNode* getNextInsertPosNode(AVLNode* node) const;
    AVLNode* getPreInsertPos(AVLNode* node) const;
    Id      getIdByItemPtr(ItemPtr nodeOffset) const;

    //测试时查看树结构
    void draw(AVLNode* root, AVLNode* flag);
    vector<vector<string>> levelOrder(AVLNode* root, AVLNode* flag);

    //打印内存信息
    void printMemoryInfo();

private:
    void update(AVLNode* node);
    void fullRebalance(AVLNode* node);
    AVLNode* rebalance(AVLNode* node, AVLNode* parent);
};

}