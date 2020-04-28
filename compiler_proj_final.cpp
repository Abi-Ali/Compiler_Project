#include <bits/stdc++.h>

using namespace std;

struct rule {
	string before;
	vector<string> after;
};

struct trans {
	int vertex_from;
	int vertex_to;
	char trans_symbol;
};
class NFA {
public:
	vector<int> vertex;
	vector<trans> transitions;
	int final_state;

	NFA() {

	}

	int get_vertex_count() {
		return vertex.size();
	}

	void set_vertex(int no_vertex) {
		for(int i = 0; i < no_vertex; i++) {
			vertex.push_back(i);
		}
	}

	void set_transition(int vertex_from, int vertex_to, char trans_symbol) {
		trans new_trans;
		new_trans.vertex_from = vertex_from;
		new_trans.vertex_to = vertex_to;
		new_trans.trans_symbol = trans_symbol;
		transitions.push_back(new_trans);
	}

	void set_final_state(int fs) {
		final_state = fs;
	}

	int get_final_state() {
		return final_state;
	}

	void display() {
		trans new_trans;
		cout<<"\n";
		for(int i = 0; i < transitions.size(); i++) {
			new_trans = transitions.at(i);
			cout<<"q"<<new_trans.vertex_from<<" --> q"<<new_trans.vertex_to<<" : Symbol - "<<new_trans.trans_symbol<<endl;
		}
		cout<<"\nThe final state is q"<<get_final_state()<<endl;
	}
};



NFA concat(NFA a, NFA b) {
	NFA result;
	result.set_vertex(a.get_vertex_count() + b.get_vertex_count());
	int i;
	trans new_trans;

	for(i = 0; i < a.transitions.size(); i++) {
		new_trans = a.transitions.at(i);
		result.set_transition(new_trans.vertex_from, new_trans.vertex_to, new_trans.trans_symbol);
	}

	result.set_transition(a.get_final_state(), a.get_vertex_count(), '^');

	for(i = 0; i < b.transitions.size(); i++) {
		new_trans = b.transitions.at(i);
		result.set_transition(new_trans.vertex_from + a.get_vertex_count(), new_trans.vertex_to + a.get_vertex_count(), new_trans.trans_symbol);
	}

	result.set_final_state(a.get_vertex_count() + b.get_vertex_count() - 1);

	return result;
}


NFA kleene(NFA a) {
	NFA result;
	int i;
	trans new_trans;
	
	result.set_vertex(a.get_vertex_count() + 2);

	result.set_transition(0, 1, '^');

	for(i = 0; i < a.transitions.size(); i++) {
		new_trans = a.transitions.at(i);
		result.set_transition(new_trans.vertex_from + 1, new_trans.vertex_to + 1, new_trans.trans_symbol);
	}

	result.set_transition(a.get_vertex_count(), a.get_vertex_count() + 1, '^');
	result.set_transition(a.get_vertex_count(), 1, '^');
	result.set_transition(0, a.get_vertex_count() + 1, '^');

	result.set_final_state(a.get_vertex_count() + 1);

	return result;
}


NFA or_selection(vector<NFA> selections, int no_of_selections) {
	NFA result;
	int vertex_count = 2;
	int i, j;
	NFA med;
	trans new_trans;

	for(i = 0; i < no_of_selections; i++) {
		vertex_count += selections.at(i).get_vertex_count();
	}

	result.set_vertex(vertex_count);
	
	int adder_track = 1;

	for(i = 0; i < no_of_selections; i++) {
		result.set_transition(0, adder_track, '^');
		med = selections.at(i);
		for(j = 0; j < med.transitions.size(); j++) {
			new_trans = med.transitions.at(j);
			result.set_transition(new_trans.vertex_from + adder_track, new_trans.vertex_to + adder_track, new_trans.trans_symbol);
		}
		adder_track += med.get_vertex_count();

		result.set_transition(adder_track - 1, vertex_count - 1, '^');
	}

	result.set_final_state(vertex_count - 1);

	return result;
}


NFA re_to_nfa(string re) {
	stack<char> operators;
	stack<NFA> operands;
	char op_sym;
	int op_count;
	char cur_sym;
	NFA *new_sym;
	
	for(string::iterator it = re.begin(); it != re.end(); ++it) {
		cur_sym = *it;
		if(cur_sym != '(' && cur_sym != ')' && cur_sym != '*' && cur_sym != '|' && cur_sym != '.') {
			new_sym = new NFA();
			new_sym->set_vertex(2);
			new_sym->set_transition(0, 1, cur_sym);
			new_sym->set_final_state(1);
			operands.push(*new_sym);
			delete new_sym;
		} else {
			if(cur_sym == '*') {
				NFA star_sym = operands.top();
				operands.pop();
				operands.push(kleene(star_sym));
			} else if(cur_sym == '.') {
				operators.push(cur_sym);
			} else if(cur_sym == '|') {
				operators.push(cur_sym);
			} else if(cur_sym == '(') {
				operators.push(cur_sym);
			} else {
				op_count = 0;
				char c;
				op_sym = operators.top();
				if(op_sym == '(') continue;
				do {
					operators.pop();
					op_count++;
				} while(operators.top() != '(');
				operators.pop();
				NFA op1;
				NFA op2;
				vector<NFA> selections;
				if(op_sym == '.') {
					for(int i = 0; i < op_count; i++) {
						op2 = operands.top();
						operands.pop();
						op1 = operands.top();
						operands.pop();
						operands.push(concat(op1, op2));
					}
				} else if(op_sym == '|'){
					selections.assign(op_count + 1, NFA());
					int tracker = op_count;
					for(int i = 0; i < op_count + 1; i++) {
						selections.at(tracker) = operands.top();
						tracker--;
						operands.pop();
					}
					operands.push(or_selection(selections, op_count+1));
				} else {
					
				}
			}
		}
	}

	return operands.top();
}

class ParsingTable
{
public:
	

	set<string> nonterminals;
	set<string> terminals;

	map<string,int> number;
	map<string,set<string> > first;
	map<string,set<string> > follow;
	set<string> firstSubstring; 

	map<pair<string,string>,string> ptable;
		
	void TakeInput();	
	void findFirstOfTerminals();
	void findFirstOfNonTerminals(string);
	void findTerminals();
	void findNonterminals();
	void findFirstOfSubstring(string);
	void findParsingTable();
	void findFollow(int);
	
};
struct rule *grammer[10];
static int countrules = 0;

void ParsingTable::TakeInput()
{
	int t;
	char  line[50];
	char *pch;
	cout<<"\n Enter no of rules in grammer ::";
	scanf("%d", &t);
	getchar();
	cout<<"\n Enter rules ::\n";
	while(t--) {
		grammer[countrules] = new rule;
		scanf("%[^\n]s", line);
		getchar();
		pch = strtok(line," >|");
		grammer[countrules]->before = pch;
		pch = strtok (NULL, " >|");
		pch = strtok (NULL, " >|");
		while (pch != NULL) {
			grammer[countrules]->after.push_back(pch);
			pch = strtok (NULL, " >|");
		}
		countrules++;
	}
}

void ParsingTable::findNonterminals() {
	for(int i = 0; i < countrules; i++) {
		nonterminals.insert(grammer[i]->before);
		number[grammer[i]->before] = i;
	}
	//return;
}

void ParsingTable::findTerminals() {
	vector<string> currg;
	string temp;
	for(int i = 0; i < countrules; i++) {
		currg = grammer[i]->after;
		for(int j = 0; j < currg.size(); j++) {
			//cout << currg[j] << endl;
			for(int k = 0; k < currg[j].length(); k++) {
				if(currg[j][k] < 'A' || currg[j][k] > 'Z') {
					temp = currg[j][k];
					terminals.insert(temp);
				}
			}
		}
	}
	return;
}

void ParsingTable::findFirstOfTerminals() {
	set<string> currg;
	for(set<string>::iterator it = terminals.begin(); it != terminals.end(); it++) {
		currg.insert(*it);
		first[*it] = currg;
		currg.clear();
	}
	//return;
}

void ParsingTable::findFirstOfNonTerminals(string key) {
	map<string,int>::iterator it = number.find(key);
	if(it == number.end() || first.find(key) != first.end())
		return;
	string temp;
	vector<string> currg;
	set<string> keyfirst;
	currg = grammer[it->second]->after;
	int flag = 1;
	for(int i = 0; i < currg.size(); i++) {
		for(int j = 0; j < currg[i].length(); j++) {
			temp = currg[i][j];
	//		cout << "here it" << endl;
			if(terminals.find(temp) != terminals.end()) {
				keyfirst.insert(temp);
				break;
			} else {
				findFirstOfNonTerminals(temp);
				if(flag) {
					for(set<string>::iterator it = first[temp].begin(); it != first[temp].end(); it++) {
						keyfirst.insert(*it);
					}
					if(first[temp].find("#") == first[temp].end()) 
						flag = 0;
				}
			}
		}
	}
	//cout << "here end" << endl;
	first[key] = keyfirst;
	for(set<string>::iterator it = nonterminals.begin(); it != nonterminals.end(); it++) {
		//cout << "here end it" << endl;
		if(first.find(*it) == first.end()) {
			findFirstOfNonTerminals(*it);
		}
	}
	//return;
}

void ParsingTable::findFirstOfSubstring(string param) {
	string temp;
	int flag ,i;
	for(i=0; i < param.length(); i++) {
		temp = param[i];
		flag = 0;
		for(set<string>::iterator it = first[temp].begin(); it != first[temp].end() ;it++) {
			if(*it == "#") {
				flag = 1;
			} else {
				firstSubstring.insert(*it);
			}

		}
		if(flag == 0)
			break;
	}
	if(flag == 1 && i == param.length())
		firstSubstring.insert("#");
	//return;
}

void ParsingTable::findFollow(int index) {
	if(index >= countrules)
		return;
	int flag;
	string tempx;
	for(int i = 0; i < grammer[index]->after.size(); i++) {
		string nter = grammer[index]->after[i];
		int j;
		flag = 0;
		int count = 0;
		for(j = 0; j < nter.length()-1; j++) {
			if(nter[j] >= 'A' && nter[j] <= 'Z') {
				findFirstOfSubstring(nter.substr(j+1,nter.length()-(j+1) ) );
				for(set<string>::iterator it = firstSubstring.begin(); it != firstSubstring.end(); it++) {
					if(*it == "#") {
						flag = 1;
					} else {
						tempx = nter[j];
						if(follow[tempx].insert(*it).second == true)
							count++;
					}
				}
				firstSubstring.clear();
			}
			if(flag == 1) {
				for(set<string>::iterator it2 = follow[grammer[index]->before].begin(); it2 != follow[grammer[index]->before].end() ;it2++) {
					tempx = nter[j];
					if(follow[tempx].insert(*it2).second == true)
						count++;

				}
			}
		}
		if(nter[j] >= 'A' && nter[j] <= 'Z') {
			for(set<string>::iterator it2 = follow[grammer[index]->before].begin(); it2 != follow[grammer[index]->before].end() ;it2++) {
				tempx = nter[j];
				if(follow[tempx].insert(*it2).second == true)
					count++;

			}
		}
		tempx = nter[j];
		if((number[tempx] <= index) && count > 0)
			findFollow(number[tempx]);
	}
	findFollow(index+1);
	//return;
}

void ParsingTable::findParsingTable() {
	int flag;
	for(int i = 0; i < countrules; i++) {
		for(vector<string>::iterator it2 = grammer[i]->after.begin(); it2 != grammer[i]->after.end(); it2++) {
			findFirstOfSubstring(*it2);
			flag = 0;
			for(set<string>::iterator it = firstSubstring.begin(); it != firstSubstring.end(); it++) {
				if(*it == "#") {
					flag = 1;
				} else {
					ptable[make_pair(grammer[i]->before,*it)] = *it2;
				}
			}
			if(flag == 1) {
				//printf("here %d\n", follow[grammer[i]->before].size());
				for(set<string>::iterator it = follow[grammer[i]->before].begin(); it != follow[grammer[i]->before].end(); it++) {
					ptable[make_pair(grammer[i]->before,*it)] = *it2;
				}	
			}
			firstSubstring.clear();
		}
	}
	//return;
}

int main() {
	int ch;
	cout<<"\n--------------------------Compiler Project-------------------";
	cout<<"\n 1. First and Follow \n\n 2. LL(1) Parsing Table \n\n 3. RE to NFA \n\n";
	//cout<<" Enter your choice :: ";
	//cin>>ch;
	ParsingTable P,P1;
	NFA a, b;
	do
	{
		cout<<" Enter your choice :: ";
		cin>>ch;
		switch(ch)
		{
			
			case 1:
				//ParsingTable P;
				P.TakeInput();
				P.findNonterminals();
				P.findTerminals();
				
				P.findFirstOfTerminals();
				P.findFirstOfNonTerminals(grammer[0]->before);
				
				cout << "First " << endl;
				for(map<string,set<string> >::iterator it = P.first.begin(); it != P.first.end(); it++) {
					string tempx = it->first;
					if(tempx != "#") {
						cout << it->first << " -> ( ";
						for(set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
							cout << *it2 << " ";
						}
						cout << " )\n";
					}
				}
				P.follow[grammer[0]->before].insert("$"); 
				P.findFollow(0);
				
				cout << "Follow " << endl;
				for(map<string,set<string> >::iterator it = P.follow.begin(); it != P.follow.end(); it++) {
					cout << it->first << " -> ( ";
					for(set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
						cout << *it2 << " ";
					}
					cout << " )\n";
				}
				for(int i =0; i < countrules; i++)
					delete(grammer[i]);

				break;
			case 2:
				//ParsingTable P;
				P1.TakeInput();
				P1.findNonterminals();
				P1.findTerminals();
				
				P1.findFirstOfTerminals();
				P1.findFirstOfNonTerminals(grammer[0]->before);
				
				P1.follow[grammer[0]->before].insert("$"); //insert $ in follow of starting symbol assume first rule contains startnig symbol
				P1.findFollow(0);
				
				P1.findParsingTable();
				for(map<pair<string,string>,string>::iterator it = P1.ptable.begin(); it != P1.ptable.end(); it++) {
					cout << "M[" << it->first.first << "," << it->first.second << "] = " << it->first.first  << "->" << it->second << endl;
				} 
				for(int i =0; i < countrules; i++)
					delete(grammer[i]);
				
				break;
			case 3:
				    cout<<"\n\nThe Thompson's Construction Algorithm takes a regular expression as an input "
					<<"and returns its corresponding Non-Deterministic Finite Automaton \n\n";
					cout<<"\n\nThe basic building blocks for constructing the NFA are : \n";
					cout<<"\nFor the regular expression segment : (a)";
					a.set_vertex(2);
					a.set_transition(0, 1, 'a');
					a.set_final_state(1);
					a.display();
				//	getch();

					cout<<"\nFor the regular expression segment : (b)";
					b.set_vertex(2);
					b.set_transition(0, 1, 'b');
					b.set_final_state(1);
					b.display();
				//	getch();

					cout<<"\nFor the regular expression segment [Concatenation] : (a.b)";
					NFA ab = concat(a, b);
					ab.display();
				//	getch();

					cout<<"\nFor the regular expression segment [Kleene Closure] : (a*)";
					NFA a_star = kleene(a);
					a_star.display();
					cout<<"\nFor the regular expression segment [Or] : (a|b)";
					int no_of_selections;
					no_of_selections = 2;
					vector<NFA> selections(no_of_selections, NFA());
					selections.at(0) = a;
					selections.at(1) = b;
					NFA a_or_b = or_selection(selections, no_of_selections);
					a_or_b.display();	
				//	getch();


					string re;
					set<char> symbols;

					cout<<"\n*****\t*****\t*****\n";
					cout<<"\nFORMAT : \n"
						<<"> Explicitly mention concatenation with a '.' operator \n"
						<<"> Enclose every concatenation and or section by parantheses \n"
						<<"> Enclose the entire regular expression with parantheses \n\n";

					cout<<"For example : \nFor the regular expression (a.(b|c))  -- \n";
					NFA example_nfa = re_to_nfa("(a.(b|c))");
					example_nfa.display();
					
					cout<<"\n\nEnter the regular expression in the above mentioned format - \n\n";
					cin>>re;


					cout<<"\n\nThe required NFA has the transitions : \n\n";
					
					NFA required_nfa;
					required_nfa = re_to_nfa(re);
					required_nfa.display();	
					break;
			
			
		}

	}while(ch<4);
	
}
/*
5
E -> TB
B -> +TB | #
T -> FC
C -> *FC | #
F -> (E) | i

4
S -> A
A -> Bb | Cd
B -> aB | #
C -> cC | #

1
S -> aSbS | bSaS | # 

3
S -> iE+SQ | a
Q -> eS | #
E -> b

3
S -> aABb
A -> c | #
B -> d | #
*/