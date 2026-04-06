package tree_sitter_usl_test

import (
	"testing"

	tree_sitter "github.com/smacker/go-tree-sitter"
	"github.com/tree-sitter/tree-sitter-usl"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_usl.Language())
	if language == nil {
		t.Errorf("Error loading Usl grammar")
	}
}
