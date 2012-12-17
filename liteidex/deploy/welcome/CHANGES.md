<!-- Changes -->

## LiteIDE X Version
	LiteIDE is a simple, open source, cross-platform Go IDE.
	
### 2012.12.12 Ver x15
* Markdown: add markdown support plugin
### 2012.12.17 Ver x15
LiteIDE: add markdown support

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
* LiteEditor: fix code compelter func test ()
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
* LiteEditor : code compelter func auto append ()
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
* LiteEditor: add option - compelter case sensitive

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
* LiteEditor : compelter option
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
* LiteEditor : fix word api complete three parts error
* LiteEditor : fix word api complete sorted
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
* LiteEditor : word compelter sorted
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
* fix Editor word writed ,compelter only one match auto hide

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