# Gopls plugin

This plugin owns the persistent `gopls serve` process and the LSP document
state for Go editors. It provides completion, navigation, references,
implementations, diagnostics, hover, signature help, rename, formatting, code
actions, and import edits.

The existing `golangcode` and `golangedit` backends remain available as legacy
fallbacks. When gopls initializes successfully, the plugin pauses gocode
completion. If gopls is unavailable or exits unexpectedly, gocode completion is
restored.
