#!/bin/bash
VIM=vim
FAKEVIM=${FAKEVIM:-test/test}
diff=meld
cmdfile=fakevim_test_cmd.log
INDENT=${INDENT:-'    '}
options="set smartindent|set autoindent|set nocindent"

print_help() {
    echo "USAGE: $0 FILE CMD..."
    echo "  Run input in both Vim and FakeVim and compare result."
    echo "  Results are stored in FILE.vim and FILE.fakevim."
    echo "  Tests for FakeVim in Qt Creator are stored in \"$cmdfile\" file."
}

print() {
    for arg in "$@"; do
        if [ "$arg" == "N" ]; then
            printf '\n'
        else
            printf "%s" "$arg"
        fi
    done >> "$cmdfile"
}

print_content() {
    local file=$1
    sed \
        -e 's/"/\\"/g' \
        -e 's/^/'"$INDENT"'"/' \
        -e 's/$/" N/' "$file" \
        >> "$cmdfile"
}

vim_exec() {
    for cmd in "$@"; do
        printf "%s" "|map \\X $cmd|normal \\X"
    done
}

run_vim() {
    local file=$1
    shift
    "$VIM" \
        -c "$options" \
        -c "$(vim_exec "$@")" \
        -c "normal i|" \
        -c "wq" "$file"
}

run_fakevim() {
    local file=$1
    shift
    find_fakevim
    "$FAKEVIM" "$file" \
        ":$options|set nopasskeys|set nopasscontrolkey<CR>" \
        "$@" "<ESC><ESC>i|<ESC>" \
        ":wq<CR>"
}

find_fakevim() {
    if [ ! -x "$FAKEVIM" ]; then
        dir=$(dirname "$(readlink -f "$0")")
        FAKEVIM=$(find "$dir" -type f -executable -name test | head -1)
    fi
}

print_test() {
    local header=$1
    local file=$2
    local footer=$3

    print "$header" N
    print_content "$file"
    print "$footer" N
}

same() {
    cmp "$@" >/dev/null 2>&1
}

main() {
    set -e

    if [ "$#" -lt 2 ]; then
        print_help
        exit 1
    fi

    local file=$1
    shift # rest are commands

    rm -f "$cmdfile"
    #cp ~/.vimrc fakevimrc

    print_test 'data.setText(' "$file" ');'

    # run command through Vim
    local vimoutfile=${file}.vim
    cp "$file" "$vimoutfile"
    run_vim "$vimoutfile" "$@"

    print_test "KEYS(\"$*\"," "$vimoutfile" ');'

    local fakevimoutfile=${file}.fakevim
    cp "$file" "$fakevimoutfile"
    run_fakevim "$fakevimoutfile" "$@"

    if same  "$fakevimoutfile" "$vimoutfile"; then
        echo OK, same result from Vim and FakeVim.
    else
        echo FAILED, different result from Vim and FakeVim.
        $diff "$fakevimoutfile" "$vimoutfile"
    fi

    reset
    cat "$cmdfile"
    sed 's/^/    /' "$cmdfile" | xclip -i
}

main "$@"

