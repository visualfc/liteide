<!-- 功能手册 -->

# LiteIDE 功能手册

## 如何支持低版本Go1.1和Go1.2
LiteIDE的编译设置使用了-i编译参数。如果使用Go1.1或Go1.2则不支持此参数。选项->查看->LiteBuild 双击gosrc.xml进行编辑。修改BUILDARGS默认设置：

	<custom id="BuildArgsi" name="BUILDARGS" value="-i"/>
	
将value设置为空存盘重启LiteIDE即可支持Go1.1和Go1.2。

## 窗口样式设置
LiteIDE目前有两种窗口样式分离式和侧边栏式。

选项->查看->LiteApp-> 窗口样式。


## 环境设置

LiteIDE环境设置插件可以让你快速设置切换多个系统环境，以便编译构建，每个环境都可配置自己需要的环境变量。

### 选择当前环境

工具栏中的下拉菜单可以用来选择目前用于编译构建/运行的环境。

![envselect.png](../images/envselect.png)

主要系统的一些常用环境列表
* Windows - win64 win32
* Linux - linux64 linux32
* MacOSX - darwin64 darwin32

可选择使用交叉编译环境，请参看下面的交叉编译设置。

### 配置环境

环境中的变量可以通过手工进行修改。要编辑当前环境，只要按一下环境下拉列表旁的`编辑环境`按钮。

另外，也可以`查看>选项> LiteEnv`面板中对所有环境进行查看和编辑。


举例 `win32.env`:

    #win32 environment
    
    GOROOT=c:\go
    #GOBIN=
    GOARCH=386
    GOOS=windows
    
    PATH=c:\mingw32\bin;%GOROOT%\bin;%PATH%
    
    LITEIDE_GDB=gdb
    LITEIDE_MAKE=mingw32-make
    LITEIDE_TERM=%COMSPEC%
    LITEIDE_TERMARGS=
    LITEIDE_EXEC=%COMSPEC%
    LITEIDE_EXECOPT=/C


举例 `linux32.env`:

    #linux32 environment
    
    GOROOT=$HOME/go
    #GOBIN=
    GOARCH=386
    GOOS=linux
    
    PATH=$GOROOT/bin:$PATH
    
    LITEIDE_GDB=gdb
    LITEIDE_MAKE=make
    LITEIDE_TERM=/usr/bin/gnome-terminal
    LITEIDE_TERMARGS=
    LITEIDE_EXEC=/usr/bin/xterm
    LITEIDE_EXECOPT=-e

## 交叉编译

必须先建立交叉编译器所需要的平台和架构。如果你不这样做，将出现类似下面的错误：

    go build runtime: linux/amd64 must be bootstrapped using make.bat

请查看 "Building compilers" subsection for examples.

### 环境变量

交叉编译将把项目为编译为另一个操作系统或体系结构，需要定义环境中的以下变量：

* `$GOOS` - 目标操作系统 (默认值等同于`$GOHOSTOS`).  可选值包括:
    * darwin (Mac OS X 10.6 或更高版本)
    * freebsd
    * linux
    * netbsd
    * openbsd
    * plan9
    * windows
* `$GOARCH` - 目标平台体系 (默认值等同于`$GOHOSTARCH`).  可选值包括:
    * amd64 (64-bit x86, 最成熟)
    * 386 (32-bit x86)
    * arm (32-bit ARM)
* `$GOARM` - ARM 架构运行库 (默认值: 6).
    * 设置 `$GOARM` 为5 时将导致链接器使用软件浮点实现，而不是使用硬件浮点支持。
* `$CGO_ENABLED`
    * 交叉编辑时值自动设置为0。

### 构建编译器

要构建编译器到交叉编译，需要转到源目录中并运行相应的脚本。下面的例子演示了如何构建一些常见的编译器，你必须自己机器上相应设置来修改环境变量。

在Windows平台上交叉编译 64-bit Linux (使用MinGW和GCC):

    > set GOARCH=amd64
    > set GOOS=linux
    > set CGO_ENABLED=0
    > cd %GOROOT%\src
    > all.bat

在Windows平台上交叉编译 32-bit Linux (使用MinGW和GCC):

    > export GOARCH=386
    > export GOOS=windows
    > export CGO_ENABLED=0
    > cd $GOROOT/src
    > ./all.bash

在Mac OS X平台上交叉编译 ARM :

    > export GOARCH=arm
    > export GOOS=linux
    > export CGO_ENABLED=0
    > cd $GOROOT/src
    > ./all.bash

## Go语言代码格式化
存盘时自动格式化	

### Goimports样式格式化
LiteIDE 查看->选项->GolangFmt-> 使用Goimports代替gofmt进行代码格式化

这个工具自动更新您的Go语言import行，增加缺少的pkg和移除未引用的pkg。

## Gocode代码完成
Go语言代码完成使用gocode <https://github.com/nsf/gocode>，gocode的工作原理是从项目的依赖库.a文件中读取，所以不能代码完成时请更新项目的依赖库。

### 更新依赖库
更新当前项目的依赖库, 点击编译菜单或工具栏`Get`(go get)

### 自动更新依赖库
LiteIDE 查看->选项->Gocode-> 自动更新依赖库

### 编译

在LiteIDE中执行交叉编译，通过选择上述相应的交叉编译环境。并且确保环境已经设置了正确的变量。包括 `GOARCH`，`GOOS`，`GOARM`和`CGO_ENABLED`，如上面所述。

一旦交叉目标环境中已被选中并切换，只需要重新编译文件/项目即可。

举例在Windows平台上配置交叉环境 64-bit Linux:

    GOROOT=c:\go
    GOBIN=
    GOARCH=amd64
    GOOS=linux
    CGO_ENABLED=0
    
    PATH=%GOBIN%;%GOROOT%\bin;%PATH%
    ...

## 编译系统
LiteIDE的编译系统是高度可配置的，可使用XML文件自定义生成相应的命令操作。

### 配置编译行为

要修改现有的编译选项，打开`查看>选项> LiteBuild`。你可以在这个目录中添加自己的图片来定义新的图标。双击一个XML文件进行编辑。每个XML文件都使用Mime格式定义了针对特定文件类型的相应操作。

举例 Go语言的 XML 配置 (`gosrc.xml`):

	<?xml version="1.0" encoding="UTF-8"?>
	<mime-info xmlns='http://www.freedesktop.org/standards/shared-mime-info'>
		<mime-type type="text/x-gosrc" id="gosrc" work="$(EDITOR_DIR)" ver="1">
			<config id="Go" name="GO" value="go"/>
			<config id="GoExec" name="GOEXEC" value="$(LITEAPPDIR)/goexec"/>
			<config id="ErrRegex" name="ERRREGEX" value="(\w?:?[\w\d_\-\\/\.]+):(\d+):"/>
			<custom id="TargetArgs" name="TARGETARGS" value=""/>
			<custom id="BuildArgs" name="BUILDARGS" value=""/>
			<custom id="InstallArgs" name="INSTALLARGS" value=""/>
			<action id="Build" img="blue/build.png" key="Ctrl+B;F7" cmd="$(GO)" args="build $(BUILDARGS)" save="all" output="true" codec="" regex="$(ERRREGEX)" navigate="true"/>
			<action id="Install" menu="Build" img="blue/install.png" key="Ctrl+F8" cmd="$(GO)" args="install $(INSTALLARGS)" save="all" output="true" regex="$(ERRREGEX)" navigate="true"/>
			<action id="BuildTests" menu="Build" img="blue/buildtest.png" cmd="$(GO)" args="test -c -gcflags &quot;-N -l&quot;" save="all" output="true" codec="" regex="$(ERRREGEX)"/>
			<action id="Clean" menu="Build" img="blue/clean.png" cmd="$(GO)" args="clean" save="all" output="true"/>
			<action id="CleanAll" menu="Build" img="blue/cleanall.png" cmd="$(GO)" args="clean -i" save="all" output="true"/>
			<action id="BuildAndRun" img="blue/buildrun.png" key="Ctrl+R;Ctrl+F7" task="Build;Run" killold="true"/>
			<action id="Run" menu="BuildAndRun" img="blue/run.png" key="Ctrl+F5" cmd="$(EDITOR_DIRNAME_GO)" args="$(TARGETARGS)" output="true" codec="utf-8" readline="true"/>
			<action id="RunTerm" menu="BuildAndRun" img="blue/runterm.png" key="Ctrl+Shift+F5" cmd="$(LITEIDE_EXEC)" args="$(LITEIDE_EXECOPT) $(GOEXEC) $(EDITOR_DIRNAME_GO) $(TARGETARGS)" output="false" readline="true"/>
			<action id="FileRun" menu="BuildAndRun" img="gray/filerun.png" key="Alt+F6" cmd="$(GO)" args="run $(EDITOR_NAME)" save="editor" output="true" codec="utf-8" readline="true"/>
			<action id="Test" img="blue/test.png" key = "Ctrl+T" cmd="$(GO)" args="test" save="all" output="true" codec="utf-8" regex="$(ERRREGEX)" navigate="true"/>
			<action id="Bench" menu="Test" img="blue/testbench.png" cmd="$(GO)" args="test -test.bench=.*" save="all" output="true" codec="utf-8" regex="$(ERRREGEX)" navigate="true"/>
			<action id="Get" menu="Test" img="blue/get.png" cmd="$(GO)" args="get -v ." save="all" output="true" codec="utf-8"/>
			<action id="Fmt" menu="Test" img="blue/fmt.png" cmd="$(GO)" args="fmt" save="all" output="true" regex="$(ERRREGEX)" navigate="true"/>
			<action id="Vet" menu="Test" img="blue/vet.png" cmd="$(GO)" args="vet" save="all" output="true" regex="$(ERRREGEX)" navigate="true"/>
			<target id="Target" cmd="$(EDITOR_DIRNAME_GO)" args="$(TARGETARGS)" work="$(EDITOR_DIR)"/>
		</mime-type>
	</mime-info>

## Go文档查看

LiteIDE可使用godoc来输出需要Go文档并浏览。可以查看标准包文档或自定义包文档。打开使用`查看> Godoc文档查看`可以查看关于此的更多细节。


## 查找和替换

编辑器提供了搜索和替换功能，并支持正则表达式。例如：

    Find: (Colo)(u)(r)
    Replace: \1\3

The above example will replace all instances of `Colour` with `Color`.
上面的例子将取代`Colour`为`Color`

文件系统的搜索功能可通过 `查找 -> 文件搜索`实现.

## 调试
LiteIDE调试Go语言需要安装GDB(Windows上可使用MinGW).

环境变量`LITEIDE_GDB`可指定使用哪一个gdb来执行调试。在Windows平台上，32位环境使用`gdb.exe`而64位环境默认使用`gdb64.exe`。在环境配置文件中可对这个环境变量进行手工配置。

## 键盘映射
您可以修改用于LiteIDE所有的功能快捷键。打开 `查看 -> 选项 -> LiteApp -> 键盘` 并修改快捷键。双击该快捷键列表进行编辑，然后按'应用'以执行更改。

快捷键必须遵循特定的格式。快捷键通常可以使用逗号（`，`）分隔，即按顺序按下对应的快捷键以启动功能。而定义支持多个快捷键则用分号（';'）进行分隔。

举例:
* `Ctrl+B`
* `Ctrl+Shift+B`
* `Ctrl+K,Ctrl+U`
* `Ctrl+Y;Ctrl+Shift+Z`
