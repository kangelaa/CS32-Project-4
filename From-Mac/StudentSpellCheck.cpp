#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cctype>

using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

//Constructor
StudentSpellCheck::StudentSpellCheck(){
    m_root = getNode(); //initializes m_root to point to first TrieNode w/ value = '.', isEndOfWorld = false, and array of nullptrs
}

//Destructor
StudentSpellCheck::~StudentSpellCheck() {
    deleteTrie(m_root);
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
    deleteTrie(m_root);     //clear previous dictionary from Trie structure (if any) by deleting all TrieNodes
    m_root = getNode();     //reassign m_root to new "dummy" node
    ifstream infile(dictionaryFile);    //create ifstream object called infile from dictionaryFile
    if (!infile){   //return false if infile could not be loaded/created
        return false;
    }
    string s;
    while (getline(infile, s)){     //keep reading in each line of file (dictionary words) until end
        TrieNode* curNode = m_root;     //set curNode equal to root (top of Trie)
        for (int i=0; i<s.size(); i++){     //iterate through each char in string/word
            char ch = tolower(s[i]);        //turn char to lowercase
            if (!isalpha(ch) && ch != '\''){        //ignore non-alpha/apostrophe chars
                continue;
            }
            int index = ch - 'a';       //set index for children array based on alphabet # (a=0, b=1, etc.)
            if (ch == '\''){        //set index for apostrophe as 26
                index = 26;
            }
            if (curNode->children[index] == nullptr){   //if there isn't already a TrieNode created for that char/pos, create one
                curNode->children[index] = getNode();
            }
            curNode = curNode->children[index];     //update curNode to point to the next TrieNode (corresponding to the current ch)
            curNode->value = ch;    //set value for that TrieNode as the ch
        }
        curNode->isEndOfWord = true;    //mark last node as leaf node/end of word
    }
	return true; 
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
    if (isValidWord(word)){ //if spellcheck passes, return true
        return true;
    } else {    //otherwise, add spelling suggestions:
        suggestions.clear();    //clear suggestions vector
        for (int i=0; i<word.size(); i++){  // iterate through the word
            string potentialWord = word;    //with each new letter pos, reassign potentialWord to the original word
            for (int j=0; j<27 && suggestions.size()<=max_suggestions; j++){    //iterating through all the possible 27 options for letters/apostrophes with a cap at max_suggestions, trying out each option for the current pos within word
                char sub = j + 'a'; //get char to sub in from the index j 
                if (j==26){
                    sub = '\'';
                }
                potentialWord[i] = sub; //sub in the char at the current pos within word that's being tested
                if (isValidWord(potentialWord)){
                    suggestions.push_back(potentialWord);   //push the potential word into spelling suggestions vector
                }
            }
        }
        return false;
    }
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
    problems.clear();
    string word = "";   //create a temp string to hold each word
    for (int i=0; i<line.size(); i++){  //for each char in the line
        char ch = tolower(line[i]);     //get the lowercase version of the char
        if (!isalpha(ch) && ch != '\''){    //if the char isn't a letter/apostrophe:
            if (word != ""){    //and the word string isn't empty
                if (!isValidWord(word)){ //if the entire word isn't present in trie or the current node isn't signified as the end of a word
                    int end = i-1;  //set end pos of word
                    int start = end - (word.size()-1);    //set start pos of word
                    Position pos;   //create a Position object pos and assign its data members appropriate values
                    pos.start = start;
                    pos.end = end;
                    problems.push_back(pos);    //push the Position object to the problems vector
                }
                word = "";  //reset the word to be an empty string
            }
        } else {
            word += ch; //if the char is a letter/apostrophe, add it to the word string
        }
    }
    if (word != ""){    //at end of line, if there's still a final word that hasn't been checked, check it
        if (!isValidWord(word)){ //if the entire word isn't present in trie or the current node isn't signified as the end of a word
            int end = line.size()-1;  //set end pos of word
            int start = end - (word.size()-1);    //set start pos of word
            Position pos;   //create a Position object pos and assign its data members appropriate values
            pos.start = start;
            pos.end = end;
            problems.push_back(pos);    //push the Position object to the problems vector
        }
    }
}

//private function implementations

//returns a pointer to initialized new TrieNode
StudentSpellCheck::TrieNode* StudentSpellCheck::getNode(){  //all new nodes are initialized w/ value = '.', isEndOfWord = false, and children[27] all = nullptr's
    TrieNode* ptrToNode = new TrieNode;
    ptrToNode->value = '.';
    for (int i=0; i<27; i++){
        ptrToNode->children[i] = nullptr;
    }
    ptrToNode->isEndOfWord = false;
    return ptrToNode;
}

//returns true if valid word, false if not
bool StudentSpellCheck::isValidWord(string word){
    TrieNode* curNode = m_root; //create a new pointer to the head node
    int j;
    for (j=0; j<word.size(); j++){     //for each letter in the word
        char chCheck = tolower(word[j]);   //check the lowercase version of each char
        int indexCheck = chCheck - 'a';     //get the correct index for each char
        if (chCheck == '\''){
            indexCheck = 26;
        }
        if (curNode->children[indexCheck] == nullptr || curNode->children[indexCheck]->value != chCheck){  //if letter at pos doesn't exist in tree, word isn't present -> break from loop
            break;
        } else {    //else continue w/ next letter in word
            curNode = curNode->children[indexCheck];
        }
    }
    if (j == word.size() && curNode->isEndOfWord){ //if the entire word is present in trie and the current node is signified as the end of a word, return true
        return true;
    } else {
        return false;   //otherwise if not a valid word, return false
    }
}

//function that utilizes recursion to delete all the nodes within the Trie
void StudentSpellCheck::deleteTrie(TrieNode* node){
    if (node == nullptr){
        return;
    }
    for (int i=0; i<27; i++){
        deleteTrie(node->children[i]);
    }
    delete node;
}

