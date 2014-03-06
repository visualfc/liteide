package main

var listHTML = `<!-- Golang Package List -->
<p class="detail">
Need more packages? The
<a href="http://godashboard.appspot.com/package">Package Dashboard</a>
provides a list of <a href="/cmd/goinstall/">goinstallable</a> packages.
</p>
<h2 id="Subdirectories">Subdirectories</h2>
<p>
{{with .Dirs}}
	<p>
	<table class="layout">
	<tr>
	<th align="left" colspan="{{html .MaxHeight}}">Name</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	</tr>
	{{range .List}}
		<tr>
		{{repeat "<td width=\"25\"></td>" .Depth}}
		<td align="left" colspan="{{html .Height}}"><a href="{{.Path}}">{{html .Name}}</a></td>
		<td></td>
		<td align="left">{{html .Synopsis}}</td>
		</tr>
	{{end}}
	</table>
	</p>
{{end}}`

var listText = `$list
{{with .Dirs}}
{{range .List}}{{.Path }}
{{end}}
{{end}}`

var listLite = `$list{{with .Dirs}}{{range .List}},{{.Path}}{{end}}{{end}}`

var findHTML = `<!-- Golang Package List -->
<p class="detail">
Need more packages? The
<a href="http://godashboard.appspot.com/package">Package Dashboard</a>
provides a list of <a href="/cmd/goinstall/">goinstallable</a> packages.
</p>
<h2 id="Subdirectories">Subdirectories</h2>
<table class="layout">
	<tr>
	<th align="left">Best</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	{{with .Best}}
		<tr>
		<td align="left"><a href="{{html .Path}}">{{.Path}}</a></td>
		<td></td>
		<td align="left">{{html .Synopsis}}</td>
		</tr>
	{{end}}	
	{{with .Dirs}}
	<tr>
	<th align="left">Match</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	</tr>
	{{range .List}}
		<tr>
		<td align="left"><a href="{{html .Path}}">{{.Path}}</a></td>
		<td></td>
		<td align="left">{{html .Synopsis}}</td>
		</tr>
	{{end}}
	</table>
	</p>
{{end}}`

var findText = `$best
{{with .Best}}{{.Path}}{{end}}
$list
{{with .Dirs}}{{range .List}}{{.Path}}
{{end}}{{end}}`

var findLite = `$find,{{with .Best}}{{.Path}}{{end}}{{with .Dirs}}{{range .List}},{{.Path}}{{end}}{{end}}`
