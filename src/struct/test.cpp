#include "AVLTree.h"
#include "test/YJS.h"
#include <memory>
#include <cstring>
using namespace std;

class GidFactory {
public:
    static int clientId;
    static int counter;
    Id produceGid() {
        return Id(clientId, counter++);
    }
};
int GidFactory::clientId = 123456;
int GidFactory::counter = 0;

GidFactory gidFactory;
AVLTree* tree = new AVLTree();
// Doc test3(YAlloc::AVL);

void addAll(int MAX);
void deleteAll(int MAX);
void reverse();
void addStr(string ans);
void testDelete();

int main() {
    
    tree->printMemoryInfo();
    
    int MAX = 65;
    string ans = "document";
    // addStr(ans);
    // addAll(MAX);
    ItemMessage message = ItemMessage( (char)(0 % 26 + 97), gidFactory.produceGid(),
                                Id(-1,-1), Id(-1,-1));
    tree->insertByPos(0, message);
    // tree->draw(tree->getRoot(), new AVLNode());

    message = ItemMessage( (char)(1 % 26 + 97), gidFactory.produceGid(),
                                Id(-1,-1), Id(-1,-1));
    tree->insertByPos(1, message);
    // tree->draw(tree->getRoot(), new AVLNode());

    tree->deleteByPos(1);
    // tree->draw(tree->getRoot(), new AVLNode());

    message = ItemMessage( (char)(2 % 26 + 97), gidFactory.produceGid(),
                                Id(-1,-1), Id(-1,-1));
    tree->insertByPos(1, message);
    // tree->draw(tree->getRoot(), new AVLNode());

    // deleteAll(MAX);
    // testDelete();
    reverse();
    tree->printMemoryInfo();
    cout<<"----------------------success-------------------------"<<endl;

    // tree->draw(tree->getRoot(), new AVLNode());
    // cout<<endl;
    // cout<<"success"<<endl;

    return 0;
}

void addStr(string ans) {
    cout<<"---------------------addStr----------------------------"<<endl;
    for (int i = 0; i < ans.size(); ++i) {
        ItemMessage message = ItemMessage( ans[i], gidFactory.produceGid(),
                                Id(-1,-1), Id(-1,-1));
        tree->insertByPos(i, message);
    }
}


void addAll(int MAX) {
    cout<<"---------------------addAll----------------------------"<<endl;
    for (int i = 0; i < MAX; ++i) {
        ItemMessage message = ItemMessage( (char)(i % 26 + 97), gidFactory.produceGid(),
                                Id(-1,-1), Id(-1,-1));
        // if(i == 59) tree->insertByPos(i, message);
        tree->insertByPos(i, message);
    }
}

void deleteAll(int MAX) {
    cout<<"---------------------deleteAll----------------------------"<<endl;
    for (int i = 0; i < MAX; ++i) {
        tree->deleteByPos(0);
        // test3.localDelete(i);
    }
}

void testDelete() {
    for(int i = 0; i < 5; i++) {
        ItemMessage message = ItemMessage( (char)(i % 26 + 97), gidFactory.produceGid(),
                                Id(-1,-1), Id(-1,-1));

        if(i == 3) {
			cout<<"Call Insert!!!!!!!!!!!!!! i == "<<i<<" c == "<<(char)(i % 26 + 97)<<endl;
            tree->insertByPos(i, message);
			cout<<"Call Delete!!!!!!!!!!!!!! i == "<<i<<endl;
            // ItemMessage message = ItemMessage( (char)(i % 26 + 97), gidFactory.produceGid(),
            //                     Id(-1,-1), Id(-1,-1));
            tree->deleteByPos(i);
			cout<<"Call Insert!!!!!!!!!!!!!! i == "<<i<<" c == "<<(char)(i % 26 + 97)<<endl;
            message = ItemMessage( (char)(i % 26 + 97), gidFactory.produceGid(),
                                Id(-1,-1), Id(-1,-1));
            tree->insertByPos(i, message);
        }
        else {
            tree->insertByPos(i, message);
			cout<<"Call Insert!!!!!!!!!!!!!! i == "<<i<<" c == "<<(char)(i % 26 + 97)<<endl;
        }
        tree->draw(tree->getRoot(), new AVLNode());
    }
}


void reverse() {
    cout<<"----------------------reverse-----------------------------"<<endl;
    ItemPtr _headTemp = tree->begin(), _endTemp = tree->end();
    // cout<<_headTemp.first->context[_headTemp.second]<<endl;
    // cout<<_endTemp.first->context[_endTemp.second-1]<<endl;
    
    std::string ans;
    int i = 0;
    while (_headTemp != _endTemp) {
        ans += _headTemp.first->context[_headTemp.second];
        i++;
        _headTemp = tree->successor(_headTemp);
        // cout<<ans<<" "<<_headTemp.first<<" "<<_headTemp.second<<" "
        //     <<_endTemp.first<<" "<<_endTemp.second<<endl;
    }
    cout<<"final ans: "<< ans << endl;
    cout<<"ans.size = "<< ans.size() <<" strlen(ans) = " << strlen(ans.c_str()) <<" i = "<<i<<endl;
}
