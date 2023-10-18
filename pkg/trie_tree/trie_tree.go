package trietree

type TrieTree struct {
	root Node
}

func NewTrieTree() *TrieTree {
	return &TrieTree{
		root: make(map[string]Node),
	}
}

func (t *TrieTree) Insert(words string) {
	node := t.root
	for _, r := range words {
		word := string(r)
		if _, ok := node[word]; ok {
			node = node[word]
		} else {
			node[word] = make(map[string]Node)
			node = node[word]
		}
	}

	for k := range node {
		delete(node, k)
	}
}

func (t *TrieTree) Contains(text string) bool {
	node := t.root
	for _, r := range text {
		item := string(r)
		if _, ok := node[item]; !ok {
			node = t.root
			continue
		}
		node = node[item]
		if len(node) == 0 {
			return true
		}

	}
	return false
}

type Node map[string]Node
