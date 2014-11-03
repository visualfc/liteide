// +build !go1.2

package main

import "go/ast"

// Go 1.1 users don't get fancy package grouping.
// But this is still gofmt-compliant:

var sortImports = ast.SortImports
