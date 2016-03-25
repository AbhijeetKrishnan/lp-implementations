#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

using namespace std;
typedef unordered_map<int, vector<pair<int, char>>> graph;

ofstream outp_file;
int state_no = 0;

struct frag {
    int start, end;
    graph g;
};

int precedence(char op) {
    switch (op) {
    case '*':
        return 3;
        break;
    case '.':
        return 2;
        break;
    case '|':
        return 1;
        break;
    case '(':
        return 0;
    }
}

string re2post(string infix) {
    /* convert input regex with lower-case alphabet and operators '|', '*', '()' and '.' to postfix */
    /* insert '.' between adjacent terminals */
    int len = 0;
    for (char c : infix) {
        if (c != '(' && c != ')') {
            ++len;
        }
    }
    vector<char> postfix(len);
    stack<char> s;
    int i = 0;
    for (char c : infix) {
        if (c == '*' || c == '.' || c == '|') {
            while (!s.empty() && precedence(s.top()) > precedence(c)) {
                postfix[i++] = s.top();
                s.pop();
            }
            s.push(c);
        }
        else if (c == '(') {
            s.push(c);
        }
        else if (c == ')') {
            while (s.top() != '(') {
                postfix[i++] = s.top();
                s.pop();
            }
            s.pop();
        }
        else {
            postfix[i++] = c;
        }
    }
    while (!s.empty()) {
        postfix[i++] = s.top();
        s.pop();
    }
    string ret_val(postfix.begin(), postfix.end());
    return ret_val;
}

frag terminal(char c) {
    frag ret;
    ret.start = state_no++;
    ret.end = state_no++;
    (ret.g)[ret.start].push_back(make_pair(ret.end, c));
    return ret;
}

frag concat(frag &f1, frag &f2) {
    frag ret;
    ret.start = state_no++;
    ret.end = state_no++;
    (ret.g)[ret.start].push_back(make_pair(f1.start, 'E'));
    for (auto ele : f1.g) {
        (ret.g)[ele.first].assign(ele.second.begin(), ele.second.end());
    }
    for (auto ele : f2.g) {
        (ret.g)[ele.first].assign(ele.second.begin(), ele.second.end());
    }
    (ret.g)[f1.end].push_back(make_pair(f2.start, 'E'));
    (ret.g)[f2.end].push_back(make_pair(ret.end, 'E'));
    return ret;
}

frag altern(frag &f1, frag &f2) {
    frag ret;
    ret.start = state_no++;
    ret.end = state_no++;
    (ret.g)[ret.start].push_back(make_pair(f1.start, 'E'));
    (ret.g)[ret.start].push_back(make_pair(f2.start, 'E'));
    for (auto ele : f1.g) {
        (ret.g)[ele.first].assign(ele.second.begin(), ele.second.end());
    }
    for (auto ele : f2.g) {
        (ret.g)[ele.first].assign(ele.second.begin(), ele.second.end());
    }
    (ret.g)[f1.end].push_back(make_pair(ret.end, 'E'));
    (ret.g)[f2.end].push_back(make_pair(ret.end, 'E'));
    return ret;
}

frag star(frag &f) {
    frag ret;
    ret.start = state_no++;
    ret.end = state_no++;
    (ret.g)[ret.start].push_back(make_pair(f.start, 'E'));
    for (auto ele : f.g) {
        (ret.g)[ele.first].assign(ele.second.begin(), ele.second.end());
    }
    (ret.g)[f.end].push_back(make_pair(f.start, 'E'));
    (ret.g)[f.end].push_back(make_pair(ret.end, 'E'));
    (ret.g)[ret.start].push_back(make_pair(ret.end, 'E'));
    return ret;
}

frag regex2nfa(string regex) {
    /* converts a postfix regex input to an NFA */
    state_no = 0;
    stack<frag> s;
    for (char c : regex) {
        switch (c) {
            case '*': {
                frag f = s.top();
                s.pop();
                s.push(star(f));
                break;
            }
            case '.': {
                frag f2 = s.top();
                s.pop();
                frag f1 = s.top();
                s.pop();
                s.push(concat(f1, f2));
                break;
            }
            case '|': {
                frag f2 = s.top();
                s.pop();
                frag f1 = s.top();
                s.pop();
                s.push(altern(f1, f2));
                break;
            }
            default: {
                s.push(terminal(c));
                break;
            }
        }
    }
    frag ret = s.top();
    s.pop();
    return ret;
}

void output_header() {
    /* outputs the initial code required for the simulator to file */
    outp_file << R"(#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>
#include <queue>

using namespace std;
typedef unordered_map<int, vector<pair<int, char>>> graph;

struct frag {
    int start, end;
    graph g;
};

frag nfa;
set<int> curr_states;

void compute_epsilon_closure(int state) {
    set<int> seen;
    queue<int> q;
    seen.insert(state);
    q.push(state);
    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        for (auto ele : (nfa.g)[curr]) {
            if (seen.find(ele.first) == seen.end() && ele.second == 'E') {
                curr_states.insert(ele.first);
                seen.insert(ele.first);
                q.push(ele.first);
            }
        }
    }
}

bool accepts(string input) {
    /* run NFA */
    bool ret = false;
    curr_states.insert(nfa.start);
    compute_epsilon_closure(nfa.start);
    for (int i = 0; input[i] != '\0'; ++i) {
        set<int> tmp_curr_states;
        for (auto ele : curr_states) {
            for (auto ele2 : (nfa.g)[ele]) {
                if (ele2.second == input[i]) {
                    tmp_curr_states.insert(ele2.first);
                }
            }
        }
        curr_states.swap(tmp_curr_states);
        tmp_curr_states.clear();
        for (auto ele : curr_states) {
            compute_epsilon_closure(ele);
        }
    }
    for (auto ele : curr_states) {
        if (ele == nfa.end) {
            ret = true;
            break;
        }
    }
    return ret;
}

void build_frag() {
    /* header ends here */)";
}

void output_nfa(frag &f) {
    /* output nfa in some form to file */
    outp_file << "\n\tnfa.start = " << f.start << ";\n";
    outp_file << "\tnfa.end = " << f.end << ";\n";
    for (auto ele : f.g) {
        for (auto ele2 : ele.second) {
            outp_file << "\t(nfa.g)[" << ele.first << "].push_back(make_pair(" << ele2.first << ", '" << ele2.second << "'));\n";
        }
    }
}

void output_tail() {
    /* outputs the tail end of the simulator to file */
    outp_file << R"(    /* tail begins here */
}

int main() {
    build_frag();
    string input;
    cin >> input;
    if (accepts(input)) {
        cout << "ACCEPTED\n";
    }
    else {
        cout << "REJECTED\n";
    }
    return 0;
})";
}

int main() {
    string regex;
    cin >> regex;
    regex = re2post(regex);
    /*
    cout << '"' << regex << '"' << '\n';
    for (char c : regex) {
        cout << (int)c << ' ';
    }
    cout << '\n';
    */
    outp_file.open("decider.cpp"); //change later
    output_header();

    frag nfa = regex2nfa(regex);

    cout << nfa.start << " " << nfa.end << '\n';
    cout << "From\t" << "To\t" << "Symbol\n";
    for (auto ele : nfa.g) {
        for (auto edge : ele.second) {
            cout << ele.first << "\t" << edge.first << "\t" << edge.second << "\n";
        }
    }

    output_nfa(nfa);
    output_tail();
    outp_file.close();
    return 0;
}
