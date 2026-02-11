#ifndef TRIE_H
#define TRIE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class TrieNode{
public:
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    bool isEndOfWord;
    std::vector<long long> nodeIds;

    TrieNode(): isEndOfWord(false){}
};

class Trie
{
private:
    std::unique_ptr<TrieNode> root;

public:
    Trie();

    void insert(const std::string& word, const std::vector<long long> &nodeIds);

    bool search(const std::string& word);

    std::vector<std::pair<std::string, std::vector<long long>>> autocomplete(const std::string& prefix, int maxResults = 5) const;

    void collectWords(TrieNode* node,const std::string& prefix, std::vector<std::pair<std::string,std::vector<long long>>> &results, int maxResults) const;
};

#endif // TRIE_H
