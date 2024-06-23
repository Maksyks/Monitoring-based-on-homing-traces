#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

std::map<std::string, int> input_map = {
    {"maksyks", 0}, {"123", 1}, {"STAT", 2},
    {"LIST", 3}, {"RETR", 4}, {"QUIT", 5}, {"wrong_input", 6},
    {"mword", 6}, {"0000", 6}
};

std::map<std::string, int> output_map = {
    {"+OK", 1}, {"-ERR", 0}
};

struct Transition {
    unsigned int s1;
    unsigned int trin;
    unsigned int trout;
    unsigned int s2;
};

class Node {
public:
    std::vector<unsigned int> node;
    std::vector<Node*> successor;
    std::pair<int, int> symbols; // пара входного и выходного символов

    Node() = default;
};

class FSM {
private:
    unsigned int s;
    unsigned int i;
    unsigned int o;
    unsigned int tr;
    Transition* Transit;
    Node* root;

public:
    FSM(std::string FSMpr);
    Node* createTree(std::vector<std::vector<unsigned int>>& ht);
    Node* createTreeRecursion(std::vector<unsigned int>& states, std::vector<std::vector<unsigned int>>& ht, std::vector<unsigned int>& w_vec, int level);
    void setRoot(Node* rt) { root = rt; }
    int traverseTree(std::vector<std::pair<int, int>>& io_pairs);
    void printTree();
    void printNode(Node* node, int level);
};

void FSM::printNode(Node* node, int level)
{
    for (int i = 0; i < level; i++) {
        std::cout << "  ";
    }
    std::cout << "State(s): ";
    for (unsigned int i = 0; i < node->node.size(); i++) {
        std::cout << node->node[i] << " ";
    }
    std::cout << " (Symbols: " << node->symbols.first << ", " << node->symbols.second << ")";
    std::cout << std::endl;
    for (unsigned int i = 0; i < node->successor.size(); i++) {
        printNode(node->successor[i], level + 1);
    }
}

void FSM::printTree()
{
    if (root == NULL) {
        std::cout << "Tree is empty" << std::endl;
        return;
    }
    std::cout << "Homing tree:" << std::endl;
    this->printNode(root, 0);
}

FSM::FSM(std::string FSMpr) {
    std::ifstream fin(FSMpr);
    if (!fin.is_open())
        std::cout << "File opening error";
    else
        std::cout << "File is open" << "\n";

    fin >> s >> i >> o >> tr;
    Transit = new Transition[tr];
    for (unsigned int j = 0; j < tr; j++) {
        fin >> Transit[j].s1 >> Transit[j].trin >> Transit[j].s2 >> Transit[j].trout;
    }
    fin.close();
}

Node* FSM::createTree(std::vector<std::vector<unsigned int>>& ht) {
    std::vector<unsigned int> states;
    std::vector<unsigned int> w_vec;
    int level = 0;
    for (unsigned int j = 0; j < s; j++)
        states.push_back(j);
    return createTreeRecursion(states, ht, w_vec, level);
}

Node* FSM::createTreeRecursion(std::vector<unsigned int>& states, std::vector<std::vector<unsigned int>>& ht, std::vector<unsigned int>& w_vec, int level) {
    Node* obj_node = new Node;
    obj_node->node = states;
    level++;

    if (level == 5)
        return obj_node;

    for (unsigned int f_in = 0; f_in < i; f_in++) {
        for (unsigned int f_out = 0; f_out < o; f_out++) {
            std::vector<unsigned int> new_states;
            for (unsigned int m = 0; m < tr; m++) {
                for (unsigned int y = 0; y < states.size(); y++) {
                    if (Transit[m].s1 == states[y] && Transit[m].trin == f_in && Transit[m].trout == f_out) {
                        bool found = false;
                        for (unsigned int n = 0; n < new_states.size(); n++) {
                            if (new_states[n] == Transit[m].s2) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            new_states.push_back(Transit[m].s2);
                        }
                    }
                }
            }

            if (new_states.size() > 1 && new_states != states) {
                std::vector<unsigned int> new_w_vec = w_vec;
                new_w_vec.push_back(f_in);
                new_w_vec.push_back(f_out);
                Node* new_node = createTreeRecursion(new_states, ht, new_w_vec, level);
                new_node->symbols = std::make_pair(f_in, f_out);
                obj_node->successor.push_back(new_node);
            } else {
                if (new_states.size() == 1) {
                    std::vector<unsigned int> new_w_vec = w_vec;
                    new_w_vec.push_back(f_in);
                    new_w_vec.push_back(f_out);
                    new_w_vec.insert(new_w_vec.begin(), new_states[0]);
                    ht.push_back(new_w_vec);
                    new_w_vec.erase(new_w_vec.begin());
                    new_w_vec.pop_back();
                    new_w_vec.pop_back();
                }
                if (!new_states.empty())
                {
                    Node* new_node = new Node;
                    new_node->node = new_states;
                    new_node->symbols = std::make_pair(f_in, f_out);
                    obj_node->successor.push_back(new_node);
                }
            }
        }
    }

    if (w_vec.size()) {
        w_vec.pop_back();
        w_vec.pop_back();
    }
    level--;
    return obj_node;
}

int FSM::traverseTree(std::vector<std::pair<int, int>>& io_pairs) {
    std::vector<Node*> temporary;
    temporary.push_back(root);
    int server_status = -1;

    for (const auto& io_pair : io_pairs) {
        int input = io_pair.first;
        int output = io_pair.second;
        std::vector<Node*> new_temporary;

        for (auto& current : temporary) {
            for (auto& node : current->successor) {
                if (node->symbols.first == input && node->symbols.second == output && !node->node.empty()) {
                    new_temporary.push_back(node);
                }
            }
        }

        temporary = new_temporary;

        for (auto& node : temporary) {
            if (node->successor.empty() && node->node.size() == 1) {
                server_status = node->node[0];
                std::cout << "//////SERVER STATUS: " << server_status << " ///////" << std::endl;
                io_pairs.clear();
                return server_status;
            }
        }

        if (temporary.empty()) 
        {
            std::cout << "////// ERROR: UNKNOWN RESPONSE ///////" << std::endl;
            io_pairs.clear();
            break;
        }
    }

    return server_status;
}
