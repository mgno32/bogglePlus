#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <stack>
#include <utility>

#include <cstring>
#include <ctime>

using namespace std;

string to_lower(string str){
    string res;
    for (int i = 0;i < str.size(); ++i){
        char c = str[i];
        if (c >= 'A' && c <= 'Z')c = c - 'A' + 'a';
        res += c;
    }
    return res;
}

struct Node{
	bool isWord;
	Node *parent;
	Node *children[26];
	Node(Node *parent = 0):parent(parent){
		isWord = false;
		memset(children, 0, sizeof(children));
	}
};

class Trie{
	public:
		Trie(){
			root = new Node();
			pointer = root;
		}
		~Trie(){
			DelNode(root);
		}
		void push(const string &word){
			Node *p = root;
			for (size_t i = 0;i < word.size();++i){
				char c = word[i];
				//转为小写
				if (c >= 'A' && c <= 'Z')c = c - 'A' + 'a';
				c -= 'a';
				// c [0, 26)
				if (!p -> children[c])p -> children[c] = new Node(p);
				p = p -> children[c];
			}
			p -> isWord = true;
		}
		void reset(){
			pointer = root;
		}
		bool go(char c){
			if (c >= 'A' && c <= 'Z')c = c - 'A' + 'a';
			c -= 'a';
			if (pointer -> children[c]){
				pointer = pointer -> children[c];
				return true;
			}
			//没有该节点
			return false;
		}
		void toback(){
			pointer = pointer -> parent;
		}
		bool isWord(){
			return pointer -> isWord;
		}
		Node* GetPointer(){
			return pointer;
		}
		void SetPointer(Node *p){
			pointer = p;
		}
	private:
		void DelNode(Node *root){
			for (int i = 0;i < 26; ++i){
				if (root -> children[i]){
					DelNode(root -> children[i]);
				}
			}
			delete root;
		}
	private:
		Node *root;
		Node *pointer;
};

const string dice16[16] = { 
"LRYTTE", "VTHRWE", "EGHWNE", "SEOTIS", 
"ANAEEG", "IDSYTT", "OATTOW", "MTOICU", 
"AFPKFS", "XLDERI", "HCPOAS", "ENSIEU", 
"YLDEVR", "ZNRNHL", "NMIQHU", "OBBAOJ" 
}; 
 
class BoggleBoard{ 
	public: 
		BoggleBoard(string inputfile); 
		BoggleBoard(int n = 4, int m = 4){ 
			row = n; 
			col = m; 
			data = vector<vector<char> >(row, vector<char>(col)); 
			for (int r = 0; r < row;++r){ 
				for (int c = 0;c < col;++c){ 
					data[r][c] = dice16[rand() % 16][rand() % 6]; 
				} 
			} 
		} 
		char get(int i, int j){ 
			return data[i][j]; 
		} 
		int rows(){ 
			return row; 
		} 
		int cols(){ 
			return col; 
		} 
	private: 
		vector<vector<char> > data; 
		int row; 
		int col; 
}; 
 
const int GRID_DELTA[8][2] = {
	{-1,-1},{0,-1},{1,-1},
	{-1,0},{1,0},
	{-1,1},{0,1},{1,1}
};

class BoggleSolver{ 
	public: 
		BoggleSolver(string dictionary_file){
			ifstream fin(dictionary_file.c_str());
			if (fin.fail()){
				cout << "Open File: " << dictionary_file << " Failed! " << endl;
				string info = "打开文件失败，请确定";
				info += dictionary_file;
				info += "文件在当前目录";
				throw info;
			}
			string word;
			fin >> word;
			while (!fin.eof()){
				trie.push(word);
				fin >> word;
			}
		}
		set<string> getAllWords(BoggleBoard board){
			set<string> res;
			for (int r = 0;r < board.rows(); ++r){
				for (int c = 0;c < board.cols(); ++c){
					DFS(r, c, board, res);
				}
			}
			return res;
		}	
		int score(string w); 
	private:
		void DFS(int r, int c, BoggleBoard &board, set<string> &res){
			vector<vector<bool> > vis(board.rows(), vector<bool>(board.cols(), false));
			stack<Rec> st;
			trie.reset();
			char ch = board.get(r, c);
			if (!trie.go(ch))return;
			string str;
			str += ch;
			st.push(Rec(r, c, 0, str));
			vis[r][c] = true;
			if (trie.isWord())res.insert(str);
			while (!st.empty()){
				Rec rec = st.top();
				st.pop();
				if (rec.step == 8){
					vis[rec.r][rec.c] = false;
					trie.toback();
					continue;
				}

				int nr = rec.r + GRID_DELTA[rec.step][0];
				int nc = rec.c + GRID_DELTA[rec.step][1];

				if (rec.step < 8){
					rec.step ++;
					st.push(rec);
				}
				//是否能进一步深入?
				if (nr >= 0 && nc >= 0 && nr < board.rows() && nc < board.cols()){
					if (!vis[nr][nc]){
						char ch = board.get(nr, nc);
						if (trie.go(ch)){
							vis[nr][nc] = true;
							string nstr = rec.str;
							nstr += ch;
							st.push(Rec(nr, nc, 0, nstr));
							if (trie.isWord()){
								res.insert(nstr);
							}
						}
					}
				}
			}
		}
	private:
		Trie trie;
		//记录调用信息
		struct Rec{
			int r, c;
			int step;
			string str;
			Rec(){}
			Rec(int r, int c, int step, string str):r(r),c(c),step(step),str(str){}
		};
};


class BoggleSolverSimple{ 
	public: 
		BoggleSolverSimple(string dictionary_file){
			ifstream fin(dictionary_file.c_str());
			string word;
			fin >> word;
			while(!fin.eof()){
				dict.insert(word);
				fin >> word;
			}
		}
		set<string> getAllWords(BoggleBoard board){
			vis = vector<vector<bool> >(board.rows(), vector<bool>(board.cols(), false));
			set<string> res;
			for (int r = 0;r < board.rows(); ++r){
				for (int c = 0;c < board.cols(); ++c){
					vis[r][c] = true;
					string str;
					str += board.get(r, c);
					if (dict.count(str))res.insert(str);
					DFS(r,c,board,str,res);
					vis[r][c] = false;
				}
			}
			return res;
		}
		int score(string w); 
	private:
		void DFS(int r, int c, BoggleBoard &board, string str, set<string> &res){
			for (int i = 0;i < 8; ++i){
				int nr = r + GRID_DELTA[i][0];
				int nc = c + GRID_DELTA[i][1];
				if (nr >= 0 && nc >= 0 && nr < board.rows() && nc < board.cols()){
					if (!vis[nr][nc]){
						vis[nr][nc] = true;
						char ch = board.get(nr, nc);
						string nstr = str;
						nstr += ch;
						if (dict.count(nstr))res.insert(nstr);
						DFS(nr,nc,board,nstr,res);
						vis[nr][nc] = false;
					}
				}	
			}
		}
	private:
		set<string> dict;
		vector<vector<bool> > vis;
};


int main(){
	srand(time(0));
	const string filename = "dictionary-yawl.txt";
	vector<string> dict;
	ifstream fin(filename.c_str());
	string word;
	fin >> word;
	while (!fin.eof()){
		dict.push_back(word);
		fin >> word;
	}

	Trie trie;
	for (size_t i = 0;i < dict.size(); ++i){
		string word = dict[i];
		trie.push(word);
	}

	BoggleBoard bog;
	for (int r = 0;r < 4;++r){
		for (int c = 0;c < 4;++c){
			cout << bog.get(r,c) << " ";
		}
		cout << endl;
	}

	BoggleSolver solver(filename);
	BoggleSolverSimple solverSimple(filename);
	clock_t t = clock();
	set<string> se = solver.getAllWords(bog);
	cout << "Used Time: "<< double((clock() - t)*1.0/CLOCKS_PER_SEC) << endl;
	clock_t t2 = clock();
	set<string> se2 = solverSimple.getAllWords(bog);
	cout << "Used Time: "<< double((clock() - t2)*1.0/CLOCKS_PER_SEC) << endl;
	if (se.size() != se2.size()){
		cout << "Size diff" << endl;
	}else{
		cout << "Size same" << endl;
	}
	int wrong = 0;
	cout << "Quick: " << se.size() << endl;
	cout << "Simple: " << se2.size() << endl;
	auto i1 = se.begin(); 
	auto i2 = se2.begin();
	for (; i1 != se.end() && i2 != se2.end(); ++i1, ++i2){
		if (!se2.count(*i1))cout << "Simple hasn't " << *i1 << endl;
		if (!se.count(*i2))cout << "Quick hasn't " << *i2 << endl;
	}
	cout << "Wrong Time: " << wrong << endl;

	return 0;

}
