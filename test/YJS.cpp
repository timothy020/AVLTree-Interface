// YJS.cpp: 定义应用程序的入口点。
//

#include "YJS.h"


using YJS_NAMESPACE::Doc;
using YJS_NAMESPACE::YAlloc;

constexpr int MAX = 259778; //648


int main()
{	

	Doc test(YAlloc::BTREE);
	Doc test2(YAlloc::LISTNEW);
	Doc test3(YAlloc::AVL);

	std::ifstream infile("output_for_cin.txt", std::ios::in);
	if (!infile) {
		// std::cerr << "无法打开文件" << std::endl;
		std::cerr << "can't open file" << std::endl;
		return 1;
	}

	int n;
	infile >> n;
	
	std::vector<std::tuple<int, int, char>> data(n);
	std::vector<pair<int,int>> pairs;

	int a = 0, b = 0, c = 0;
	for (int i = 0; i < n; ++i) {
		infile >> a >> b;
		if (b == 0) {
			infile >> c;
			data[i] = std::make_tuple(a, b, c%26+97);
			// pairs.push_back({a,c});
		}
		else
			data[i] = std::make_tuple(a, b, '\0');
	}

	std::cout << MAX <<'/' << n << std::endl;

	//-----------------------test1---------------------------------
	cout<<"MAX = " << MAX<<endl;
	clock_t beginTest = clock();
	for (int i = 0; i < MAX; i++){
		std::apply([&test](int a, int b, char c) {
			if (b == 0)
				test.localInsert(a, c);
			else
				test.localDelete(a);
		}, data[i]);
	}
	clock_t endTest = clock();
	
	std::cout << test.getText() << std::endl;
	double duration = double(endTest - beginTest) / CLK_TCK;
	std::cout << "test time: " << duration << std::endl;

	//-----------------------test2---------------------------------
	// cout<<"MAX = " << MAX <<endl;
	// string ans;
	// beginTest = clock();
	// for (int i = 0; i < MAX; i++) {
	// 	std::apply([&test2, &i, &ans](int a, int b, char c) {
	// 		if (b == 0) {
	// 			// ans.push_back(c);
	// 			// cout<<i<<":"<<c<<" ";
	// 			test2.localInsert(a, c);
	// 		}
	// 		else
	// 			test2.localDelete(a);
	// 		}, data[i]);

	// 	//std::cout << std::get<0>(p)<< ' ' << std::get<1>(p) << ' ' << (char)std::get<2>(p) << std::endl;
	// }
	// endTest = clock();
	// cout<<endl;

	// std::cout << test2.getText() << std::endl;
	// duration = double(endTest - beginTest) / CLK_TCK;
	// std::cout << "test2 time: " << duration << std::endl;
	
	
	//-----------------------test3---------------------------------
	cout<<"MAX = " << MAX<<endl;
	beginTest = clock();
	for (int i = 0; i < MAX; i++) {
		// cout<<"MAX = " << MAX << " i = "<< i <<" data[i] = " <<(char)(get<2>(data[i])%26 +97)<<endl;
		std::apply([&test3, &i](int a, int b, char c) {
			if (b == 0) {
				test3.localInsert(a, c);
				}
			else {
				test3.localDelete(a);
				}
			}, data[i]);
	}

	//---------使用pairs测试-----------
	// for(int i = 0; i < MAX-1; i++) {
	// 	auto p = pairs[i];
	// 	int a = p.first; char c = p.second;
	// 	cout<<a<<" "<<c<<endl;
	// 	test3.localInsert(a, c);
	// }

	// --------绘制树结构--------
	// AVLTree* tree = (AVLTree*)test3.yStruct;
	// tree->draw(tree->getRoot(), new AVLNode());
    // cout<<endl;
    // cout<<"success"<<endl;

	// cout<< "ans = " << ans <<endl;
	// cout<<"ans.size() = "<<ans.size()<<endl;
	// cout<<ans<<endl;
	// for (int i = 0; i < ans.size(); ++i) {
	// 	test3.localInsert(i, ans[i]);
	// 	// test3.localInsert(i, '\0');
    // }

	// for (int i = 0; i < 3; ++i) {
    //     // message = CharacterMessage( (char)(i % 26 + 97), gidFactory.produceGid());
    //     // tree->insertByPos(i, message);
	// 	test3.localDelete(0);
    // }

	endTest = clock();

	std::cout << test3.getText() << std::endl;
	duration = double(endTest - beginTest) / CLK_TCK;
	std::cout << "test3 time: " << duration << std::endl;

	std::cout << "\ntime: " << duration << std::endl;
	std::cout << "Test res:" << (test.getText() == test3.getText()) << std::endl;
	std::cout << "Test size:" << test.getText().size() << std::endl;
	// std::cout << "Test size:" << test2.getText().size() << std::endl;
	std::cout << "Test size:" << test3.getText().size() << std::endl;
    ((AVLTree*)test3.yStruct)->printMemoryInfo();
	// string str = test3.getText();
	// str.resize(test2.getText().size());
	// cout<<str<<endl;
	// std::cout << "验证:" << (test2.getText() == str) << std::endl;
	// system("pause");

	return 0;
}
