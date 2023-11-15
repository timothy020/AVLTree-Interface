#include<vector>
#include<stack>
#include <tuple>
#include<iostream>
#include "src/struct/YStruct.h"

namespace YJS_NAMESPACE {

    constexpr auto MAX_INTERNAL_SIZE = 5;
    constexpr auto STRING_POOL_MAX = 20;

    using namespace std;

    struct Node
    {
        char content;
        bool deleted;
    };

    class InternalNode;
    class LeafNode;

    class TreeNode
    {
    public:
        TreeNode* parent;
        bool isleaf = false;

        virtual Context GetNode() { return ""; }
        virtual void SetNode(Context node) { }

        virtual void SetDeleted(vector<char> isdeleted) { }
        virtual vector<char> GetDeleted() { return {}; }

        virtual void SetOffset(vector<unsigned  int>) { }
        virtual vector<unsigned int> GetOffset() { return {}; }

        virtual vector<TreeNode*> GetChild() { return {}; }
        virtual void SetChild(vector<TreeNode*>) { }

        virtual void SetRightOfLeaf(TreeNode* rgh) { }
        virtual TreeNode* GetRightOfLeaf() { return nullptr; }
        virtual void SetLeftOfLeaf(TreeNode* lef) { }
        virtual TreeNode* GetLeftOfLeaf() { return nullptr; }
        virtual void SetSum(unsigned int num) { }
        virtual unsigned int GetSum() { return 0; }
        virtual void SetGID(Id g) { }
        virtual Id GetGID() { return {}; }
        TreeNode();
        virtual ~TreeNode();
        void FreeChildren();
    };


    class LeafNode : public TreeNode, public ItemListInterface
    {
    private:
        vector<char> isdeletedList; 
        TreeNode* leftptr; 
        TreeNode* rightptr; 
    public:
        void SetRightOfLeaf(TreeNode* rgh) { rightptr = rgh; }
        TreeNode* GetRightOfLeaf() { return rightptr; }
        void SetLeftOfLeaf(TreeNode* lef) { leftptr = lef; }
        TreeNode* GetLeftOfLeaf() { return leftptr; }
        void SetDeleted(vector<char> isdeleted) { isdeletedList = isdeleted; }
        vector<char> GetDeleted() { return isdeletedList; }

        Context GetNode() { return context; }
        void SetNode(Context node) { context = node; } //

        void SetGID(Id g) { headId = g; }
        Id GetGID() { return headId; }
        LeafNode();
        ~LeafNode();
    };


    class InternalNode : public TreeNode
    {
    private:
        vector<TreeNode*> children; 
        vector<unsigned int> offsetList; 
        int offsetSum; 
    public:
        void SetOffset(vector<unsigned int> num) { offsetList = num; }
        vector<unsigned int> GetOffset() { return offsetList; }

        vector<TreeNode*> GetChild() { return children; }
        void SetChild(vector<TreeNode*> pointer) { children = pointer; }

        void SetSum(unsigned int num) { offsetSum = num; }
        unsigned int GetSum() { return offsetSum; }
        InternalNode();
        ~InternalNode();
    };

    class BPlusTree : public YInterface
    {
    private:
        TreeNode* BT_root;
        tuple<int, TreeNode*, stack<int>> getShiftByPos(int pos)const;
        void insertInternal(TreeNode* cursor, stack<int> path, int length);
        void updateLeafParent(TreeNode* leftLeaf, TreeNode* middleLeaf, TreeNode* rightLeaf, TreeNode* parent, int updateIdx, int length);

        pair<int, TreeNode*> getLeafByPos(int pos) const;
        void insert(int pos, Context contents, Id gid);

    public:
        void display();
        void checkPool(); 

        ItemPtr begin() const override;
        ItemPtr end() const override;
        void insertItem(Index index, ItemMessage itemMsg) override;
        void deleteItem(Index index) override;
        ItemPtr getItemByPos(Index index) const override;
        ItemPtr predecessor(ItemPtr itemPtr) const override;
        ItemPtr successor(ItemPtr itemPtr) const override;
        Id getId(ItemPtr itemPtr) const override;
        
        BPlusTree();
        ~BPlusTree();      
    };

}