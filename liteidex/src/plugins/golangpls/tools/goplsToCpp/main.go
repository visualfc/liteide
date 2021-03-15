package main

import (
	"fmt"
	"go/ast"
	"go/parser"
	"go/token"
	"io"
	"io/fs"
	"os"
	"strconv"
	"strings"
	"text/template"
	"unicode"
)

func isExported(name string) bool {
	return unicode.IsUpper(rune(name[0]))
}

func getCppType(goType string) string {
	switch goType {
	case "string", "uri.URI":
		return "QString"
	case "float32", "float64":
		return "double"
	case "uint32", "uint64":
		return "unsigned int"
	case "int32", "int64", "jsonrpc2.Code":
		return "int"
	}
	return goType
}

func ucfirst(input string) string {
	if len(input) == 0 {
		return input
	}
	return strings.ToUpper(input[:1]) + input[1:]
}

var templClass *template.Template
var templFuncs = template.FuncMap{
	"cppType": getCppType,
	"ucfirst": ucfirst,
}

func init() {
	templClass, _ = template.New("class").Funcs(templFuncs).Parse(`class {{.Name}}: {{.Parents}} {
{{range .Attributes }}	{{.Type}} *m_{{.Name}} = nullptr;
{{end}}
public:
	{{.Name}}() = default;
	virtual ~{{.Name}}() {
{{range .Attributes }}		if(m_{{.Name}}) { delete m_{{.Name}}; }
{{end}}	}
{{range .Attributes }}
	void set{{ ucfirst .Name}}({{.Type}} *{{.Name}}) { m_{{.Name}} = {{.Name}}; }
	{{.Type}} *get{{ ucfirst .Name}}() { return m_{{.Name}}; }{{end}}
	QJsonObject toJson() const {
		QJsonObject obj;
{{range .Inherited}}		QJsonObject {{.}}_json = {{.}}::toJson();
		for(const QString &key: {{.}}_json.keys()) {
			obj.insert(key, {{.}}_json.value(key));
		}
{{end}}{{range .Attributes }}		if(m_{{.Name}}) {
			{{  .Type.ToJson .Name }}
			obj.insert("{{.Name}}", {{.Name}});
		}
{{end}}
		return obj;
	}

	void fromJson(const QJsonObject &json) {
{{range .Inherited}}		{{.}}::fromJson(json);
{{end}}{{range .Attributes }}		if(json.contains("{{.Name}}")) {
			{{ .Type.FromJson (printf "json.value(\"%s\")" .Name) .Name }}
			m_{{.Name}} = {{.Name}};
		}
{{end}}
	}
};
`)
}

func printType(w io.Writer, t string) {
	if parent, ok := typesDef[t]; ok {
		parent.Print(w)
	}
}

type Attribute struct {
	Name string
	Type IType
}

type StructDef struct {
	Name       string
	Inherited  []IType
	Attributes []Attribute
	Computed   bool
	Parents    string
}

func (def *StructDef) Alias() IType {
	return nil
}

func (def *StructDef) ComputeOrder() {
	if def.Computed {
		return
	}
	def.Computed = true
	for _, inherited := range def.Inherited {
		inherited.ComputeOrder()
	}

	for _, attr := range def.Attributes {
		attr.Type.ComputeOrder()
	}
	if len(def.Inherited) == 0 {
		def.Parents = "virtual public GoPlsParams"
	} else {
		sep := ""
		for _, p := range def.Inherited {
			def.Parents += sep + " public " + p.String()
			sep = ","
		}
	}
	printOrder = append(printOrder, def)
}

func (def *StructDef) Print(w io.Writer) {
	templClass.Execute(w, def)
}

type ConstantDef struct {
	Name     string
	Type     IType
	Value    interface{}
	Computed bool
}

func (def *ConstantDef) Alias() IType {
	return def.Type
}

func (def *ConstantDef) ComputeOrder() {
	if def.Computed {
		return
	}
	def.Computed = true
	def.Type.ComputeOrder()
	printOrder = append(printOrder, def)
}

func (def *ConstantDef) Print(w io.Writer) {
	fmt.Fprintf(w, "const %s %s = %v;\n", def.Type.String(), def.Name, def.Value)
}

type TypeDef struct {
	Name     string
	Type     IType
	Computed bool
}

func (def *TypeDef) Alias() IType {
	return def.Type
}

func (def *TypeDef) ComputeOrder() {
	if def.Computed {
		return
	}
	def.Computed = true
	def.Type.ComputeOrder()
	printOrder = append(printOrder, def)
}

func (def *TypeDef) Print(w io.Writer) {
	fmt.Fprintf(w, "typedef %s %s;\n", def.Type.String(), def.Name)
}

func computeOrder(t IType) {
	if def, ok := typesDef[t.String()]; ok {
		def.ComputeOrder()
	}
}

type ListType struct {
	Type IType
}

func (list *ListType) ToJson(varname string) string {
	res := `	QJsonArray {{var}};
			for(auto it = m_{{var}}->cbegin(); it != m_{{var}}->cend(); ++it) {
				auto m_item = *it;
				{{json}};
				{{var}}.append(item);
			}
`
	res = strings.ReplaceAll(res, "{{var}}", varname)
	alias := getAliasType(list.Type)

	res = strings.ReplaceAll(res, "{{json}}", alias.ToJson("item"))
	return res
}

func (def *ListType) FromJson(json string, varname string) string {
	res := ` QJsonArray arr = {{jsonvar}}.toArray();
			auto {{var}} = new {{type}}();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				{{json}}
				{{var}}->append(item);
			}`

	res = strings.Replace(res, "{{var}}", varname, -1)
	res = strings.Replace(res, "{{jsonvar}}", json, -1)
	res = strings.Replace(res, "{{json}}", getAliasType(def.Type).FromJson("(m_item)", "item"), -1)
	res = strings.Replace(res, "{{type}}", def.String(), -1)
	return res
}

func (def *ListType) ComputeOrder() {
	computeOrder(def.Type)
}

func (def *ListType) String() string {
	return fmt.Sprintf("QList<%s*>", def.Type.String())
}

type HashType struct {
	Key   IType
	Value IType
}

func (def *HashType) ToJson(varname string) string {
	res := `QJsonObject {{var}};
			for(auto hash_it = m_{{var}}->cbegin(); hash_it != m_{{var}}->cend(); ++hash_it) {
				auto m_hash_item = *hash_it;
				{{json}}
				{{var}}.insert(hash_it.key(), hash_item);
			}`

	res = strings.Replace(res, "{{var}}", varname, -1)
	res = strings.Replace(res, "{{json}}", getAliasType(def.Value).ToJson("hash_item"), -1)
	return res
}

func (def *HashType) FromJson(json string, varname string) string {
	res := ` QJsonObject obj = {{jsonvar}}.toObject();
			QStringList objKeys = obj.keys();
			auto {{var}} = new {{type}}();
			for(auto m_it = objKeys.cbegin(); m_it != objKeys.cend(); ++m_it) {
				{{json}}
				{{var}}->insert(*m_it, hash);
			}`

	res = strings.Replace(res, "{{var}}", varname, -1)
	res = strings.Replace(res, "{{jsonvar}}", json, -1)
	res = strings.Replace(res, "{{json}}", getAliasType(def.Value).FromJson("obj.value(*m_it)", "hash"), -1)
	res = strings.Replace(res, "{{type}}", def.String(), -1)
	return res
}

func (def *HashType) ComputeOrder() {
	computeOrder(def.Key)
	computeOrder(def.Value)
}

func (def *HashType) String() string {
	return fmt.Sprintf("QHash<%s,%s*>", def.Key.String(), def.Value.String())
}

type BasicType struct {
	Name string
}

func getAliasType(name IType) IType {
	if n, ok := typesDef[name.String()]; ok {
		alias := n.Alias()
		if alias == nil {
			return name
		}
		return getAliasType(alias)
	}
	return name
}

func (def *BasicType) ToJson(varname string) string {
	st := getAliasType(def)

	switch st.String() {
	case "unsigned int", "QString", "bool", "double", "int":
		return fmt.Sprintf("QJsonValue %s = QJsonValue::fromVariant(QVariant::fromValue(*m_%s));", varname, varname)
	case "QJsonObject", "QJsonValue":
		return fmt.Sprintf("QJsonValue %s = *m_%s;", varname, varname)
	default:
	}
	if st != def {
		return st.ToJson(varname)
	}
	return fmt.Sprintf("QJsonObject %s = m_%s->toJson();", varname, varname)
}

func (def *BasicType) FromJson(json string, varname string) string {
	st := getAliasType(def)

	switch st.String() {
	case "QString":
		return fmt.Sprintf("auto %s = new QString(%s.toString());", varname, json)
	case "unsigned int", "bool", "double", "int":
		return fmt.Sprintf("auto %s = new %s(%s.toVariant().value<%s>());", varname, st.String(), json, st.String())
	case "QJsonObject":
		return fmt.Sprintf("auto %s = new QJsonObject(%s.toObject());", varname, json)
	case "QJsonValue":
		return fmt.Sprintf("auto %s = new QJsonValue(%s);", varname, json)
	default:
	}
	if st != def {
		return st.FromJson(json, varname)
	}
	return fmt.Sprintf(`auto %s = new %s();
			%s->fromJson(%s.toObject());`, varname, st.String(), varname, json)
}

func (def *BasicType) ComputeOrder() {
	computeOrder(def)
}

func (def *BasicType) String() string {
	return getCppType(def.Name)
}

func (def *BasicType) Print(w io.Writer) {
	printType(w, def.Name)
}

type IDef interface {
	Print(io.Writer)
	ComputeOrder()
	Alias() IType
}

type IType interface {
	String() string
	ComputeOrder()
	ToJson(varname string) string
	FromJson(json string, varname string) string
}

//var constants = []ConstantDef{}
var typesDef = map[string]IDef{}

var printOrder = []IDef{}

func getJsonTagName(tag string) string {
	tag = strings.Trim(tag, "`")
	fields := strings.Fields(tag)
	for _, field := range fields {
		if strings.HasPrefix(field, "json:") {
			field = strings.TrimPrefix(field, "json:")
			field = strings.Trim(field, `"`)
			return strings.Split(field, ",")[0]
		}
	}
	return tag
}

func getTypeName(expr ast.Expr) IType {
	if ident, ok := expr.(*ast.Ident); ok {
		return &BasicType{Name: ident.Name}
	} else if arr, ok := expr.(*ast.ArrayType); ok {
		return &ListType{
			Type: getTypeName(arr.Elt),
		}
	} else if hash, ok := expr.(*ast.MapType); ok {
		return &HashType{
			Key:   getTypeName(hash.Key),
			Value: getTypeName(hash.Value),
		}
	} else if sel, ok := expr.(*ast.SelectorExpr); ok {
		return &BasicType{
			Name: sel.X.(*ast.Ident).Name + "." + sel.Sel.Name,
		}
	} else if _, ok := expr.(*ast.InterfaceType); ok {
		return &BasicType{
			Name: "QJsonValue",
		}
	} else if start, ok := expr.(*ast.StarExpr); ok {
		return getTypeName(start.X)
	}
	fmt.Printf("UNKNOWN: %#v\n", expr)
	return nil
}

func declTypeConv(name string, decl *ast.TypeSpec) string {
	if st, ok := decl.Type.(*ast.StructType); ok {
		structDef := StructDef{
			Name: decl.Name.Name,
		}

		for _, field := range st.Fields.List {
			iType := getTypeName(field.Type)
			if len(field.Names) == 0 {
				structDef.Inherited = append(structDef.Inherited, iType)
			} else {
				varName := field.Names[0].Name
				if !isExported(varName) {
					continue
				}
				attr := Attribute{
					Name: getJsonTagName(field.Tag.Value),
					Type: iType,
				}
				structDef.Attributes = append(structDef.Attributes, attr)
			}
		}
		typesDef[structDef.Name] = &structDef
		return structDef.Name
	} else {
		typesDef[name] = &TypeDef{
			Name: name,
			Type: getTypeName(decl.Type),
		}
		return name
	}
	return ""
}

func main() {
	pkgs, err := parser.ParseDir(token.NewFileSet(), "protocol", func(fi fs.FileInfo) bool { return !strings.HasSuffix(fi.Name(), "_test.go") }, 0)
	if err != nil {
		fmt.Println("fail to parse package")
		return
	}

	for _, pkg := range pkgs {
		for _, file := range pkg.Files {
			for name, obj := range file.Scope.Objects {
				if !(obj.Kind == ast.Con || obj.Kind == ast.Typ) || !isExported(name) {
					continue
				}
				if obj.Kind == ast.Con {
					if decl, ok := obj.Decl.(*ast.ValueSpec); ok {
						if value, ok := decl.Values[0].(*ast.BasicLit); ok {
							def := ConstantDef{
								Name: name,
							}
							switch value.Kind {
							case token.FLOAT:
								def.Value, _ = strconv.ParseFloat(value.Value, 10)
								def.Type = &BasicType{Name: "float"}
							case token.INT:
								def.Value, _ = strconv.ParseInt(value.Value, 10, 64)
								def.Type = &BasicType{Name: "int"}
							case token.STRING:
								def.Value = value.Value
								def.Type = &BasicType{Name: "string"}
							}
							typesDef[name] = &def
						}
					}
				} else if obj.Kind == ast.Typ {
					if decl, ok := obj.Decl.(*ast.TypeSpec); ok {
						declTypeConv(name, decl)
					}
				}
			}
		}
	}

	result, _ := os.Create("../../generated.h")
	fmt.Fprintln(result, "#ifndef GENERATED_H")
	fmt.Fprintln(result, "#define GENERATED_H")
	fmt.Fprintln(result, "#include <goplstypes.h>")
	fmt.Fprintln(result, "namespace GoPlsTypes {")
	for _, t := range typesDef {
		t.ComputeOrder()
	}
	for _, t := range printOrder {
		t.Print(result)
	}
	fmt.Fprintln(result, "}")
	fmt.Fprintln(result, "#endif")
	result.Close()
}
