<!-- Changes -->

## LiteIDE X Version
  LiteIDE is a simple, open source, cross-platform Go IDE.

### 2014.03.06 Ver x21
* LiteIDE
	* fast highlighter for MacOS X 10.9
	* add auto reload files option, if modified on the drive.
	* remove x20 always open in new window mode.
	* fix folder expand state
	* fix for case sensitive path comparison on windows
* GolangAst
	* add type factor to funcs
	* add import symbol, and view document menu
	* add context menu
	* add symbol extra info
* LiteBuild
	* add golint support
* LiteEnv
	* recreate system.env
* LiteEditor
	* fix if/else fold
	* new color scheme gist-github (thanks William Kennedy <bill@ardanstudios.com>)
* GolangFmt
	* fix timeout
	* fix GOPATH

### 2013.12.09 Ver x20
* LiteIDE
	* add folders manager
	* always open folder with new window (option)
	* editor add navigate area for code format or build error
	* add new GolangPresent plugin
	* tool windows use instantpopup
	* update editor tab context menu
	* add evilworks.qss style (by Vedran Vuk <vedranvuk82@gmail.com>)
* LiteEditor
	* add error navigate area
	* update kate highlighter
	* add gopresent.xml syntax
	* add Visual Studio Theme (by William Kennedy <bill@ardanstudios.com>)
	* fix goto match braces	
	* update editor menu
* LiteEnv:
	* auto set $PATH for GOROOT/bin and GOBIN and GOPATH/bin and GOPATH/bin/GOOS_GOARCH
	* fix to use utf8 encoding
* GolangCode:
	* auto update depends package (option)
	* fix close gocode on last window close
* GolangFmt:
	* use sync code format (option)
	* use goimports instead gofmt (option)		
	* code format error show in editor navigate area
* LiteDebug:
	* external cmd setting
	* fix command input foucus
* Markdown:
	* add new Github_Style.css (by Slene <vslene@gmail.com>)
* FileBrowser:
	* option show hidden files
* LiteBuild:
	* fix build menu enable	
	* build error show in editor navigate area
	* add gopm(Go Package Manager) tool <https://github.com/gpmgo/gopm>
* GolangPresent:
	* support golang present editor
	* verify present error show in editor navigate area
	* export golang present to html
* Welcome:
	* update document
	* add zh_CN document
	
### 2013.7.17 Ver x19
* LiteIDE
	* MacOS X 10.8 Retina support
	* add simple webkit browser
	* update macosx tool window short key to Ctrl+Alt+1~9
* LiteDebug	
	* add debug go test support, thanks for GoEddie <https://github.com/GoEddie>
* GdbDebugger	
	* gdb debugger --tty mode support (windows default)
	* debugger command input foucus
* LiteEditor		
	* add wheel zooming option 
	* fix editor extra width
	* fix init editor update
* LiteFind	
	* fix find hide
* Markdown
	* markdown preview fast sync	

### 2013.5.21 Ver x18.2
* fast for gocode
* add option to close gocode on exit
* auto append GOPATH/bin to liteide env PATH
* editor extern highlighting "TODO"
* go code auto-complete ignores strings and comments

### 2013.5.10
* compiled compatibility for qt5.02

### 2013.5.8 Ver x18.1
* optimization gocode plugin
* execute preload litebuild/command/*.api
* execute enable tab key to compiler

### 2013.5.7 Ver x18
* English spelling and grammar corrections throughout, thanks for Nik-U <https://github.com/Nik-U>
* LiteIDE:
	* Improved event log
* litefind:
	* Find/Replace commands highlight existing text
	* enable find file
	* add close button	
* liteeditor:
	* Word wrap support
	* Added option to trim whitespace on save
	* Added option to hide edit toolbar
	* update kate and color scheme
	* fix kate/highlight tab
	* fix editor indent guide by tabsize
* liteapp:
	* fix full screen bug
* litebuild:
	* execute use combbox	
	* add close button
* litedebug:
	* add close button
* markdown:
	* enable markdown(sundown) extend

### 2013.2.8 Ver x17
* LiteIDE:
	* move execute from filesystem to bottom tool
	* escape for any bottom tool window (find , execute ...)
	* macosx lion full screen
	* add color theme highcontrastdark.xml and app theme black.qss, thanks for addxtoy <pollux@lavabit.com>
	* add tr liteide_de.ts , thanks for Helge Plaschke <https://github.com/HelgePlaschke>
	* fix keybord scheme 
* liteeditor:
	* show tab ident guide
	* show find scope expression
	* show selection scope expression
	* show file eof - option
	* show line wrap (markdown editor)
	* update current line color
	* any time focus if need
	* fix zoom tab width
	* fix open file cursor position start
	* fix set color theme not request restart
* markdown:
	* fix export path
* filesystem:
	* fix double clicked item and execute
* litedebug:
	* save/restore start editor
	* fix breakpoint move in editor
	* fix debug target args
* litefind:
	* new find and replace
	* fix find editor and unfold
	* fix backword find	
* litebuild:		
	* add test bench	
* document:
	* enable zoom font size Ctrl++/Ctrl+-/Ctrl+0
	* fix find crash
* gopath setup:
	* fix invalid path

	
### 2013.1.16 Ver x16
* support keybord mapping scheme
* fix: lookup go command in $PATH
* fix: remove GOBIN enviroment set
* fix: empty editor cursor position	
* fix: double click error output to jump source and editor set focus
  
### 2013.1.10 Ver x15.2
* LiteIDE:
	* use new logo and icon
	* support go version hg-tip
* LiteApp:
	* update logo
	* update splash	
	* add action close same/other folder files
	* fix full screen restore
* LiteEnv:
	* add user env files	
* LiteEditor:
	* fix `Insert` key
	* update sublime color scheme
* GolangDoc:
	* support go tip version
	* show error output	
* GolangCode:
	* fix code completer icon visible
* Golang Fmt:
	* fix timeout option invalid
* Markdown:
	* fix critical messsage
	* add css Documents.css	
	
### 2012.12.28 Ver x15.1
* LiteIDE:
	* update logo
* LiteApp:
	* plugins custom load
	* custom style use css(Qt Style Sheets)
	* add style coffee
	* fix template `doc.go`
	* tabs middle button close editor
	* fix memory leak
* LiteEditor:
	* font zoom support
	* add sublime color scheme, thanks for `Henson Lu`
	* custom tab width and tab to spaces for mimetype
* LiteEnv:
	* add cross-compiler   
* GolangDoc:
	* fix goto source

### 2012.12.18 Ver x15
* LiteIDE:
	* add markdown plugin
	* changed LiteApp to dynamic library
	* redesign editor menu and toolbar
	* update doc to markdown format  
	* add html render twin-engine
		* internal QHtmlBrowser
		* plugin QtWebKit, if find qt webkit dynamic library
* LiteApp:
	* changed to dynamic library  
	* support full screen mode
	* editor tabs change to toolbar
	* add toolbar icon size option
	* add splash visible option
	* add editor tab bar close button visible option
	* add tab context menu close left or right tabs
* LiteEditor:
	* add overwrite mode (key Insert)
	* add  custom tab width option
	* redesign editor menu , context menu and toolbar
	* add right margin line visible option
* GolangFmt:
	* fix comment offset, remove spaces  
* LiteBuild: 
	* id BuildAndRun before kill old process
	* redesign build toolbar and menu
	* support GOBIN
* LiteDebug: 
	* add debug before rebuild option
	* add debug external application action
* Markdown: 
	* markdown editor support
		* action h1-h6, bold, italic, code, list, quote, hr, link, image
		* syntax highlighting
		* live preview
		* sync scroll
		* export html
		* export pdf
		* custom css , css files from [Mou](http://mouapp.com) 
	* markdown batch 
		* separate to html
		* separate to pdf
		* merge to html
		* merge to pdf
* GolangDoc: 
	* fix url parser


### 2012.11.15 Ver x14.1
* LiteIDE : quick start
* LiteApp : multi instance support
* GolangDoc: deply load mode
* GolangPackage: deply load mode
* GolangAst : fix classview 
* LiteEditor: fix if/else fold
* GolangDoc : goapi enable _test

### 2012.11.6 Ver x14.0
* tools/goapi : new goapi tool
* GolangDoc: View Expression Information (F1)
* GolangDoc: Jump to Declaration (F2)
* GolangDoc: lookup all GOPATH pkg api
* LiteApp: fix editor navigate 
* LiteEditor : add action Move to New Windows
* LiteFind : focus in find text and enter
* LiteEditor: fix code completer func test ()
* GolangCode : auto restart gocode if liteide GOPATH changed
* GolanPlay: enable gocode and fmt
* GolangFmt: auto popup gofmt messsage option
* LiteBuild : fix build config custom view
* LiteBuild : BUILDARGS / INSTALLARGS support set 
  example -gcflags "-N -l" for debug
  example -ldflags "-s" for release
* remove: Makefile plugin , Profile plugin, x64 Project

### 2012.10.10 Ver x13.2
* GolangCode : the best support for gocode
* LiteApp : add editor tab context menu
* LiteApp : update view menu
* LiteApp : fix check modify and save
* GolangFmt : fix fmt editor style restore
* GolangDoc : fix go/doc/*.html parser meta "Path="
* GolangAst : fix update model style restore
* Welcome : update page
* LiteEnv : edit enviroment action
* LiteBuild : update gosrc.xml
* LiteBuild : fix double click goto line regexp 
* LiteBuild : show enviroment go env
* LiteBuild : execute error use red font
* LiteEditor : color theme set currnet line background
* LiteEditor : add color theme darktango.xml(Dumitru Ungureanu)
* LiteEditor : go wordapi update, keyword types and funcs
* LiteEditor : code completer func auto append ()
* LiteEditor : fix tab indent
* FileSystem : fix execute find path
* LiteFind : fix current directory 

### 2012.9.24 Ver x13.1
* LiteBuild : fix build load crash.
* LiteApp : fix double click error output regexp
* LiteApp : fix folder drop to liteide
* LiteDebug : watch vars load /save by project
* LiteDebug : breakpoint load /save by file
* LiteDebug : add remove all breakpoint action
* LiteDebug : add / remove global or local watch point
* LiteEditor : optimization editor mark
* LiteEditor : enable drop text
* GolangPlay : enable gofmt

### 2012.9.21 Ver x13
* LiteApp : any folder, any file to build
* LiteApp : update window style, add editor toolbar and status
* LiteEditor : remove editor toolbar 
* LiteEditor : extra use editor font
* LiteBuild : new build toolbar and menu
* LiteBuild : clean action tags
* GolangFmt : sync execute gofmt
* GolangDoc : deply filter index
* GolangAst : classview for folder
* Outline : alltime expand
* FileBrowser : rename to FileSystem
* FileSystem : sync editor file to folder
* FileSystem : add go command line edit (Ctrl+`)
* FileSystem : double click execute file and execute on build output
* Windows : include diff bin

### 2012.9.14 Ver x12.5
* LiteApp : move dock style, change to idea style tool windows.
* LiteApp : double click editor and maximized or restore editor
* LiteApp : esc key to hide bottom tool windows
* LiteApp : auto load last session
* LiteEditor : add edit navigate histroy
* LiteEditor : extra use color scheme
* GolangFmt : show error log and goto line

### 2012.7.12 Ver X12.2
* GolangFmt : fix fold error diff bug
* LiteDebug : add var watch, example main.var os.Stdout
* Welcome : add load last selection
* LiteEditor : add fold or unfold all action
* LiteEditor : add duplicate action
* LiteEditor : add color theme railscast.xml(Alexander Rødseth)
* LiteFind : default find use editor selection
* LiteEnv : fix windows env LITEIEDE_EXECOPT=/C
* LiteApp : fix icon on windows xp unable show

### 2012.6.28 Ver X12.1.1
* LiteEditor : fix fold bug - line include "head{}end"
* LiteEditor : kate go.xml breace match add "[]"

### 2012.6.27 Ver X12.1
* LiteIDE : LiteIDE use new logo
* LiteEditor : fold / unfold
* LiteEditor : color scheme custom selection 
* LiteEditor : kate go.xml remove linecontinue
* LiteEditor : goto line
* LiteApp : fix recent file load bug
* GdbDebugger : fix frame if not find fullname
* LiteIDE : update faq document
* LiteIDE : add install document

### 2012.6.13 Ver X12 1.875
* LiteIDE : reset resource directory
* LiteEditor : goto next/prev block
* LiteEditor : code comment
* LiteEditor : braces match
* LiteEditor: LRLF check by \n
* LiteEditor: no print char conver to .
* LiteEditor: move tooltip to status
* GolangFmt : enable gofmt diff
* GolangFmt : save auto gofmt
* LiteBuild: project build toolbar and editor build toolbar.
* LiteFind: add file search
* PackageBrowser: fast load

### 2012.5.16 Ver X11 1.71875 beta4.1
* LiteBuild: fix execute args for gosrc/gopro/makefile
* LiteApp: add option - when close project auto close project editors
* PackageBrowser : double click pkg and load project
* LiteEditor: add option - completer case sensitive

### 2012.5.15 Ver X11 1.71875 beta4
* GolangPackage : enable local package
* GolangDoc : golang api fmt#Println -> fmt.Println
* LiteBuild : fix regexp error
* FileSystem: (windows) fix rename abc to Abc false
* PackageProject : remove auto reload timer
* LiteEditor: (linux) redo enable CTRL+Y

### 2012.5.2 Ver X11 1.71875 beta3
* GolangPackage : new golang package plugin for GO1
* GolangPackage : PackageBrowser and PackageProject for GO1
* GolangTool : remove the plugin
* GolangDoc: add golang api for GO1
* GolangDoc: add golang api filter
* GolangAst: add filter
* GolangAst: classview and outline
* LiteEditor : completer option
* LiteApp : new file wizard support GO1
* Welcome : new recent page
* gopromake : update pkg depend

### 2012.3.9 Ver X11 1.71875 beta2
* GolangPlay : new go playground plugin
* GolangTool : sync editor, sync project

### 2012.3.1 Ver X11 1.71875 beta1
* GolangTool : new golang plugin for GO1, import GOPATH project
* GolangDoc : update to GO1
* LiteBuild : update to GO1
* tools : udpate all tools for GO1

### 2011.9.26 Ver X10 1.5625
* LiteEditor : add line mark api
* LiteDebug : new debug toolbar
* LiteDebug : insert/remove break
* LiteDebug : breakpoint and current line mark
* GdbDebugger : async record view
* GdbDebugger : variables expand tree
* GdbDebugger : call stack view
* GdbDebugger : load library view
* TerminalEdit : provided to LiteBuild and LiteDebug 

### 2011.9.14 Ver X9 1.40625
* LiteDebug : new debug manager plugin
* GdbDebugger : new gdb debugger plugin
* LiteBuild : add build config and custom
* LiteFind : fix regexp use match case

### 2011.9.1 Ver X8 1.25
* LiteFind : new find and replace plugin, remove CodeSearch plugin
* LiteEditor : fix word api completer three parts error
* LiteEditor : fix word api completer sorted
* LiteApp : fix main window state load/save on linux
* LiteApp : change path to standard linux path
* LiteBuild : add build option, example execute arguments
* FileBrowser : add new file wizard

### 2011.8.12 Ver X7 1.09375
* Welcome : new html page, quick link, session, recent projects and files
* GolangDoc : generic document nav
* GolangDoc : scheme entry [file|list|find|pdoc]
* FileBrowser : add "View Godoc Here"
* LiteBuild : auto clean output if line more than 1024
* LiteEditor : word completer sorted
* LiteEditor : auto braces last state check
* GolangAst : show type struct field
* GolangAst : editor state keep self

### 2011.7.28 Ver X6 0.9375
* LiteEnv : new plugin, liteide enviroment setup
* Welcome : add LiteIDE document browser
* GolangDoc : add package find, example input "zip" and find
* GolangDoc : add godoc /src/pkg/ and /src/cmd/ tree
* GolangDoc : add go/doc/ document browser
* GolangDoc : add document browser text search
* LiteEditor : add export html file
* FileBrowser : add menu item desktop explorer
* LiteEditor : add enable automatic insert brackets
* LiteEditor : add option :  display  and behavior
* LiteApp : fix session load last editor or welcome page
* LiteApp : fix find mimetype incorrect
* GolangCode : fix application exit gocode close
* FileBrowser : fix context menu popup on root
* LiteBuild : fix ouput html format to plaintext
* LiteBuild : fix build does not handle spaces in file paths
* LiteBuild : fix output append \n->next append
* LiteBuild : fix enviroment depend of LiteBuild

### 2011.7.7 X5 v0.78125
* add CodeSearch plugin : Editor Search and Replace
* add GolangDoc plugin : Golang Package Find
* add Search and Replace Regex: (Colo)(u)(r) -> \1\3
* add FileBorwser top dir setup
* add LiteEditor Copy Syntax Color support (HTML mimedata)
* add LiteEditor ReadOnly
* add MainWindow all DockWidget saveState
* add EditorManager CTRL+TAB switch
* update tools/gopromake to r58
* update tools/goexec to r58
* update tools/goastview to r58
* fix filebrowser rename show oldname
* fix ENV c:/go/bin;%PATH% order, GOBIN is priority
* fix FileBrowser sort incorrect on MACOSX
* fix GoFmt reload editor display pos keep
* fix Editor Close Ask BUG ret==SaveAll
* fix Editor word writed ,completer only one match auto hide

### 2011.6.30 X4 v0.625
* add interface zh_cn
* add filebrowser pluing add/change/remove 
* add editor color style scheme setup
* add editor codec check/reload
* add editor pdf export
* add editor print and preview
* editor style line LRLF of source
* kate go.xml support "number" style
* liteapp last session save/load

### 2011.6.21 X3 v0.46875
* add file browser plugin
* source editor pre link project build
* source editor outside modify/remove watcher

### 2011.5.20 X2 v0.3125
* add gocode plugin
* add build/makefile
* add build/lua stdoutput:setvbuf("no")
* fix build task stop on error
* fix build process write \n

### 2011.5.12 X1 v0.15625
* New LiteIDE X version

------

## LiteIDE Old version

### 2011.2.25  v0.2.1
* add build on MacOSX10.6
* support Makefile and .pro project
* project support GCOPT and GLOPT
* global option page
* global output pane
* set GOROOT option
* run shell
* run debug
* run process can write
* run process can kill
* clean project opt temp or all
* new project wizard : .pro/package/Makefile
* new example goinfo, how to use local or search package

### 2011.1.25 v0.1.8
* goastview plugin

### 2011.1.18 v0.1.6
* add build on Linux64

### 2011.1.12 v0.1.5
* build error and jump to source line

### 2011.1.11 v0.1.2
* gopromake use goroutines

### 2011.1.10 v0.1
* init version 