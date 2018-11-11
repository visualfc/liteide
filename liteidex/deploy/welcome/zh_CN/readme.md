<!-- 欢迎使用 LiteIDE X -->

LiteIDE X
=========

### 简介

_LiteIDE 是一个轻量级的开源跨平台 Go语言 IDE._

* 版本: X35.2
* 作者: [七叶 (visualfc)](mailto:visualfc@gmail.com)

### 功能

* 核心功能
	* 系统环境管理
	* MIME类型管理
	* 可配置编译命令
	* 支持文件搜索替换和恢复
	* 快速打开文件、符号和命令
	* 插件系统
	
* 高级代码编辑器
	* 代码编辑支持 Go语言、Markdown 和 Golang Present
	* 快速代码导航工具
	* 语法高亮和配色方案
	* 代码完成
	* 代码折叠
	* 显示保存修订
	* 重读文件使用代码差异方式
	
* Go 语言支持
	* 支持 Go1.11 Go modules
	* 支持 Go1.5 Go vendor
	* 支持 Go1 GOPATH
	* Go 编译环境管理
	* 使用标准 Go 工具编译和测试
	* 自定义 GOPATH 支持 系统定义、IDE 定义和项目定义
	* 自定义项目编译配置
	* Go 包浏览器
	* Go 类视图和大纲
	* Go 文档搜索和 API 索引
	* 代码导航和信息提示
	* 代码查找引用
	* 代码重构和恢复
	* 集成 [gocode](https://github.com/visualfc/gocode) clone of [nsf/gocode](https://github.com/nsf/gocode)
	* 集成 [gomodifytags](https://github.com/fatih/gomodifytags)
	* 支持代码查询工具 guru
	* 调试支持 GDB 和 [Delve](https://github.com/derekparker/delve)
	
### 系统支持
* Windows x86 (32-bit or 64-bit)
* Linux x86 (32-bit or 64-bit)
* MacOS X10.6 or higher (64-bit)
* FreeBSD 9.2 or higher (32-bit or 64-bit)
* OpenBSD 5.6 or higher (64-bit)

### LiteIDE 命令行
	liteide [files|folder] [--select-env id] [--local-setting] [--user-setting] [--reset-setting]
	 
	--select-env [system|win32|cross-linux64|...]     选择初始环境ID
	--local-setting   强制使用本地配置
	--user-setting    强制使用用户配置
	--reset-setting   重置当前配置 ( 清除配置文件 )
	
### 更新 liteide 工具支持新的 Go语言版本

	go get -u github.com/visualfc/gotools
	go get -u github.com/visualfc/gocode
	
	Windows/Linux: 复制 GOPATH/bin gotools 和 gocode 到 liteide/bin
	MacOS: 复制 GOPATH/bin gotools 和 gocode 到 LiteIDE.app/Contents/MacOS	

### 网址
* LiteIDE 主页
	* <http://liteide.org>
* 源代码下载
	* <https://github.com/visualfc/liteide>
* 发行版下载
	* <http://sourceforge.net/projects/liteide/files>
* Google用户组
	* <https://groups.google.com/group/liteide-dev>
* FAQ
	* <https://github.com/visualfc/liteide/blob/master/liteidex/deploy/welcome/zh_CN/guide.md>	
* 支持LiteIDE
	* <https://visualfc.github.com/support>
