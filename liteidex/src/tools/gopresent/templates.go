package main

var action_tmpl = `
{/*
This is the action template.
It determines how the formatting actions are rendered.
*/}

{{define "section"}}
  <h{{len .Number}} id="TOC_{{.FormattedNumber}}">{{.FormattedNumber}} {{.Title}}</h{{len .Number}}>
  {{range .Elem}}{{elem $.Template .}}{{end}}
{{end}}

{{define "list"}}
  <ul>
  {{range .Bullet}}
    <li>{{style .}}</li>
  {{end}}
  </ul>
{{end}}

{{define "text"}}
  {{if .Pre}}
  <div class="code"><pre>{{range .Lines}}{{.}}{{end}}</pre></div>
  {{else}}
  <p>
    {{range $i, $l := .Lines}}{{if $i}}{{template "newline"}}
    {{end}}{{style $l}}{{end}}
  </p>
  {{end}}
{{end}}

{{define "code"}}
  <div class="code{{if playable .}} playground{{end}}" contenteditable="true" spellcheck="false">{{.Text}}</div>
{{end}}

{{define "image"}}
<div class="image">
  <img src="{{.URL}}"{{with .Height}} height="{{.}}"{{end}}{{with .Width}} width="{{.}}"{{end}}>
</div>
{{end}}

{{define "iframe"}}
<iframe src="{{.URL}}"{{with .Height}} height="{{.}}"{{end}}{{with .Width}} width="{{.}}"{{end}}></iframe>
{{end}}

{{define "link"}}<p class="link"><a href="{{.URL}}" target="_blank">{{style .Label}}</a></p>{{end}}

{{define "html"}}{{.HTML}}{{end}}
`

var article_tmpl = `
{/* This is the article template. It defines how articles are formatted. */}

{{define "root"}}
<!DOCTYPE html>
<html>
  <head>
    <title>{{.Title}}</title>
    <link type="text/css" rel="stylesheet" href="static/article.css">
    <meta charset='utf-8'>
  </head>

  <body>
    <div id="topbar" class="wide">
      <div class="container">
        <div id="heading">{{.Title}}
          {{with .Subtitle}}{{.}}{{end}}
        </div>
      </div>
    </div>
    <div id="page" class="wide">
      <div class="container">
        {{with .Sections}}
          <div id="toc">
            {{template "TOC" .}}
          </div>
        {{end}}

        {{range .Sections}}
          {{elem $.Template .}}
        {{end}}{{/* of Section block */}}

        <h2>Authors</h2>
        {{range .Authors}}
          <div class="author">
            {{range .Elem}}{{elem $.Template .}}{{end}}
          </div>
        {{end}}
      </div>
    </div>
    <script src='/play.js'></script>
  </body>
</html>
{{end}}

{{define "TOC"}}
  <ul>
  {{range .}}
    <li><a href="#TOC_{{.FormattedNumber}}">{{.Title}}</a></li>
    {{with .Sections}}{{template "TOC" .}}{{end}}
  {{end}}
  </ul>
{{end}}

{{define "newline"}}
{{/* No automatic line break. Paragraphs are free-form. */}}
{{end}}
`

var slides_tmpl = `
{/* This is the slide template. It defines how presentations are formatted. */}

{{define "root"}}
<!DOCTYPE html>
<html>
  <head>
    <title>{{.Title}}</title>
    <meta charset='utf-8'>
    <script src='static/slides.js'></script>
  </head>

  <body style='display: none'>

    <section class='slides layout-widescreen'>
      
      <article>
        <h1>{{.Title}}</h1>
        {{with .Subtitle}}<h3>{{.}}</h3>{{end}}
        {{if not .Time.IsZero}}<h3>{{.Time.Format "2 January 2006"}}</h3>{{end}}
        {{range .Authors}}
          <div class="presenter">
            {{range .TextElem}}{{elem $.Template .}}{{end}}
          </div>
        {{end}}
      </article>
      
  {{range $i, $s := .Sections}}
  <!-- start of slide {{$s.Number}} -->
      <article>
      {{if $s.Elem}}
        <h3>{{$s.Title}}</h3>
        {{range $s.Elem}}{{elem $.Template .}}{{end}}
      {{else}}
        <h2>{{$s.Title}}</h2>
      {{end}}
      </article>
  <!-- end of slide {{$i}} -->
  {{end}}{{/* of Slide block */}}

      <article>
        <h3>Thank you</h1>
        {{range .Authors}}
          <div class="presenter">
            {{range .Elem}}{{elem $.Template .}}{{end}}
          </div>
        {{end}}
      </article>

  </body>
  {{if .PlayEnabled}}
  <script src='/play.js'></script>
  {{end}}
</html>
{{end}}

{{define "newline"}}
<br>
{{end}}
`
