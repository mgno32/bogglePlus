/*
 * 疑问：　Q是否代表Qu
 *         getAllWords 中的单词是否长度大于等于３？
 *         结果单词的大小写？ 
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <stack>
#include <utility>

#include <cstdio>
#include <cstdlib>
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

string to_upper(string str){
    string res;
    for (int i = 0;i < str.size(); ++i){
        char c = str[i];
        if (c >= 'a' && c <= 'z')c = c - 'a' + 'A';
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
				ToID(c);
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
			ToID(c);
			if (pointer -> children[c]){
				pointer = pointer -> children[c];
				return true;
			}
			//没有该节点
			return false;
		}
		inline void toback(){
			pointer = pointer -> parent;
		}
		inline bool isWord(){
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
		void ToID(char &c){
			if (c >= 'a' && c <= 'z')c -= 'a';
			else c -= 'A';
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
		BoggleBoard(string inputfile){
			ifstream fin(inputfile.c_str());
			fin >> row >> col;
			data = vector<vector<char> >(row, vector<char>(col)); 
			for (int r = 0;r < row; ++r){
				for (int c = 0;c < col; ++c){
					string temp;
					fin >> temp;
					data[r][c] = temp[0];
				}
			}
		}
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
				cout << info << endl;
				return;
			}
			string word;
			fin >> word;
			while (!fin.eof()){
				//A valid word must contain at least 3 letters. 
				if (word.size() >= 3){
					word = to_upper(word);
					dict.insert(word); // upper
					string temp;
					//Qu -> Q
					for (int i = 0;i < word.size(); ++i){
						if (word[i] == 'Q' && i + 1 < word.size() && word[i + 1] == 'U'){
							temp += 'Q';
							++i;
						}else{
							temp += word[i];
						}
					}
					trie.push(temp);
				}
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
		int score(string w){
			if (!dict.count(to_upper(w)))return 0;
			int len = w.size();
			if (len <= 2)return 0;
			if (len <= 4)return 1;
			if (len == 5)return 2;
			if (len == 6)return 3;
			if (len == 7)return 5;
			return 11;
		}	
	private:
		void DFS(int r, int c, BoggleBoard &board, set<string> &res){
			vector<vector<bool> > vis(board.rows(), vector<bool>(board.cols(), false));
			stack<Rec> st;
			trie.reset();
			char ch = board.get(r, c);
			if (!trie.go(ch))return;
			string str;
			str += ch;
			st.push(Rec(r, c, 0));
			vis[r][c] = true;
			if (trie.isWord())res.insert(str);
			while (!st.empty()){
				Rec rec = st.top();
				st.pop();
				if (rec.step == 8){
					vis[rec.r][rec.c] = false;
					trie.toback();
					str.erase(str.end() - 1);
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
							str += ch;
							st.push(Rec(nr, nc, 0));
							if (trie.isWord()){
								string temp;
								for (int i = 0;i < str.size();++i){
									if (str[i] != 'Q'){
										temp += str[i];
									}else{
										temp += "QU";
									}
								}
								res.insert(temp);
							}
						}
					}
				}
			}
		}
	private:
		Trie trie;
		set<string> dict;
		//记录调用信息
		struct Rec{
			int r, c;
			int step;
			Rec(){}
			Rec(int r, int c, int step):r(r),c(c),step(step){}
		};
};

