package main

import (
	"code.google.com/p/go.tools/present"
	"fmt"
	"os"
	"path/filepath"
)

func isSkipURL(url string) bool {
	if filepath.HasPrefix(url, "http://") {
		return true
	}
	if filepath.HasPrefix(url, "https://") {
		return true
	}
	return false
}

func verify_path(root string, url string) error {
	if isSkipURL(url) {
		return nil
	}
	path := url
	if !filepath.IsAbs(url) {
		path = filepath.Join(root, path)
	}
	_, err := os.Stat(path)
	if err != nil {
		return fmt.Errorf("path %s not find", url)
	}
	return nil
}

func verify_doc(root string, doc *present.Doc) error {
	for _, section := range doc.Sections {
		for _, elem := range section.Elem {
			switch i := elem.(type) {
			case present.Image:
				if err := verify_path(root, i.URL); err != nil {
					return err
				}
			}
		}
	}
	return nil
}
