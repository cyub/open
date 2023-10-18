package main

import (
	"fmt"

	trietree "github.com/cyub/open/pkg/trie_tree"
)

func main() {
	t := trietree.NewTrieTree()
	words := []string{"油管", "脸书", "城管", "美国人"}
	for _, word := range words {
		t.Insert(word)
	}
	strs := []string{"youtube油管", "facebook书", "城管来了", "美国", "美国美人", "来自美国人群中"}
	for _, str := range strs {
		fmt.Println(str, t.Contains(str))
	}
}
