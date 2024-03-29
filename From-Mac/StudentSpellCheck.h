#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck();
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
    struct TrieNode {
        char value;
        TrieNode* children[27];
        bool isEndOfWord;
    };
    TrieNode* m_root;
    
    //helper functions
    TrieNode* getNode();
    bool isValidWord(std::string word);
    void deleteTrie(TrieNode* node);
};

#endif  // STUDENTSPELLCHECK_H_
