#ifndef GENERATED_H
#define GENERATED_H
#include <goplstypes.h>
namespace GoPlsTypes {
class FileCreate: virtual public GoPlsParams {
	QString *m_uri = nullptr;

public:
	FileCreate() = default;
	virtual ~FileCreate() {
		if(m_uri) { delete m_uri; }
	}

	void setUri(QString *uri) { m_uri = uri; }
	QString *getUri() { return m_uri; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}

	}
};
class CreateFilesParams: virtual public GoPlsParams {
	QList<FileCreate*> *m_files = nullptr;

public:
	CreateFilesParams() = default;
	virtual ~CreateFilesParams() {
		if(m_files) { qDeleteAll(*m_files); delete m_files; }
	}

	void setFiles(QList<FileCreate*> *files) { m_files = files; }
	QList<FileCreate*> *getFiles() { return m_files; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_files) {
				QJsonArray files;
			for(auto it = m_files->cbegin(); it != m_files->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				files.append(item);
			}

			obj.insert("files", files);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("files")) {
			 QJsonArray arr = json.value("files").toArray();
			auto files = new QList<FileCreate*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new FileCreate();
			item->fromJson((m_item).toObject());
				files->append(item);
			}
			m_files = files;
		}

	}
};
class ImplementationTextDocumentClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	bool *m_linkSupport = nullptr;

public:
	ImplementationTextDocumentClientCapabilities() = default;
	virtual ~ImplementationTextDocumentClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_linkSupport) { delete m_linkSupport; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setLinkSupport(bool *linkSupport) { m_linkSupport = linkSupport; }
	bool *getLinkSupport() { return m_linkSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_linkSupport) {
			QJsonValue linkSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_linkSupport));
			obj.insert("linkSupport", linkSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("linkSupport")) {
			auto linkSupport = new bool(json.value("linkSupport").toVariant().value<bool>());
			m_linkSupport = linkSupport;
		}

	}
};
typedef ImplementationTextDocumentClientCapabilities TextDocumentClientCapabilitiesImplementation;
const int DiagnosticSeverityWarning = 2;
class Position: virtual public GoPlsParams {
	unsigned int *m_line = nullptr;
	unsigned int *m_character = nullptr;

public:
	Position() = default;
	virtual ~Position() {
		if(m_line) { delete m_line; }
		if(m_character) { delete m_character; }
	}

	void setLine(unsigned int *line) { m_line = line; }
	unsigned int *getLine() { return m_line; }
	void setCharacter(unsigned int *character) { m_character = character; }
	unsigned int *getCharacter() { return m_character; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_line) {
			QJsonValue line = QJsonValue::fromVariant(QVariant::fromValue(*m_line));
			obj.insert("line", line);
		}
		if(m_character) {
			QJsonValue character = QJsonValue::fromVariant(QVariant::fromValue(*m_character));
			obj.insert("character", character);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("line")) {
			auto line = new unsigned int(json.value("line").toVariant().value<unsigned int>());
			m_line = line;
		}
		if(json.contains("character")) {
			auto character = new unsigned int(json.value("character").toVariant().value<unsigned int>());
			m_character = character;
		}

	}
};
class Range: virtual public GoPlsParams {
	Position *m_start = nullptr;
	Position *m_end = nullptr;

public:
	Range() = default;
	virtual ~Range() {
		if(m_start) { delete m_start; }
		if(m_end) { delete m_end; }
	}

	void setStart(Position *start) { m_start = start; }
	Position *getStart() { return m_start; }
	void setEnd(Position *end) { m_end = end; }
	Position *getEnd() { return m_end; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_start) {
			QJsonObject start = m_start->toJson();
			obj.insert("start", start);
		}
		if(m_end) {
			QJsonObject end = m_end->toJson();
			obj.insert("end", end);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("start")) {
			auto start = new Position();
			start->fromJson(json.value("start").toObject());
			m_start = start;
		}
		if(json.contains("end")) {
			auto end = new Position();
			end->fromJson(json.value("end").toObject());
			m_end = end;
		}

	}
};
class SelectionRange: virtual public GoPlsParams {
	Range *m_range = nullptr;
	SelectionRange *m_parent = nullptr;

public:
	SelectionRange() = default;
	virtual ~SelectionRange() {
		if(m_range) { delete m_range; }
		if(m_parent) { delete m_parent; }
	}

	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setParent(SelectionRange *parent) { m_parent = parent; }
	SelectionRange *getParent() { return m_parent; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_parent) {
			QJsonObject parent = m_parent->toJson();
			obj.insert("parent", parent);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("parent")) {
			auto parent = new SelectionRange();
			parent->fromJson(json.value("parent").toObject());
			m_parent = parent;
		}

	}
};
typedef double TextDocumentSyncKind;
class SaveOptions: virtual public GoPlsParams {
	bool *m_includeText = nullptr;

public:
	SaveOptions() = default;
	virtual ~SaveOptions() {
		if(m_includeText) { delete m_includeText; }
	}

	void setIncludeText(bool *includeText) { m_includeText = includeText; }
	bool *getIncludeText() { return m_includeText; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_includeText) {
			QJsonValue includeText = QJsonValue::fromVariant(QVariant::fromValue(*m_includeText));
			obj.insert("includeText", includeText);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("includeText")) {
			auto includeText = new bool(json.value("includeText").toVariant().value<bool>());
			m_includeText = includeText;
		}

	}
};
class TextDocumentSyncOptions: virtual public GoPlsParams {
	bool *m_openClose = nullptr;
	TextDocumentSyncKind *m_change = nullptr;
	bool *m_willSave = nullptr;
	bool *m_willSaveWaitUntil = nullptr;
	SaveOptions *m_save = nullptr;

public:
	TextDocumentSyncOptions() = default;
	virtual ~TextDocumentSyncOptions() {
		if(m_openClose) { delete m_openClose; }
		if(m_change) { delete m_change; }
		if(m_willSave) { delete m_willSave; }
		if(m_willSaveWaitUntil) { delete m_willSaveWaitUntil; }
		if(m_save) { delete m_save; }
	}

	void setOpenClose(bool *openClose) { m_openClose = openClose; }
	bool *getOpenClose() { return m_openClose; }
	void setChange(TextDocumentSyncKind *change) { m_change = change; }
	TextDocumentSyncKind *getChange() { return m_change; }
	void setWillSave(bool *willSave) { m_willSave = willSave; }
	bool *getWillSave() { return m_willSave; }
	void setWillSaveWaitUntil(bool *willSaveWaitUntil) { m_willSaveWaitUntil = willSaveWaitUntil; }
	bool *getWillSaveWaitUntil() { return m_willSaveWaitUntil; }
	void setSave(SaveOptions *save) { m_save = save; }
	SaveOptions *getSave() { return m_save; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_openClose) {
			QJsonValue openClose = QJsonValue::fromVariant(QVariant::fromValue(*m_openClose));
			obj.insert("openClose", openClose);
		}
		if(m_change) {
			QJsonValue change = QJsonValue::fromVariant(QVariant::fromValue(*m_change));
			obj.insert("change", change);
		}
		if(m_willSave) {
			QJsonValue willSave = QJsonValue::fromVariant(QVariant::fromValue(*m_willSave));
			obj.insert("willSave", willSave);
		}
		if(m_willSaveWaitUntil) {
			QJsonValue willSaveWaitUntil = QJsonValue::fromVariant(QVariant::fromValue(*m_willSaveWaitUntil));
			obj.insert("willSaveWaitUntil", willSaveWaitUntil);
		}
		if(m_save) {
			QJsonObject save = m_save->toJson();
			obj.insert("save", save);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("openClose")) {
			auto openClose = new bool(json.value("openClose").toVariant().value<bool>());
			m_openClose = openClose;
		}
		if(json.contains("change")) {
			auto change = new double(json.value("change").toVariant().value<double>());
			m_change = change;
		}
		if(json.contains("willSave")) {
			auto willSave = new bool(json.value("willSave").toVariant().value<bool>());
			m_willSave = willSave;
		}
		if(json.contains("willSaveWaitUntil")) {
			auto willSaveWaitUntil = new bool(json.value("willSaveWaitUntil").toVariant().value<bool>());
			m_willSaveWaitUntil = willSaveWaitUntil;
		}
		if(json.contains("save")) {
			auto save = new SaveOptions();
			save->fromJson(json.value("save").toObject());
			m_save = save;
		}

	}
};
class WorkDoneProgressOptions: virtual public GoPlsParams {
	bool *m_workDoneProgress = nullptr;

public:
	WorkDoneProgressOptions() = default;
	virtual ~WorkDoneProgressOptions() {
		if(m_workDoneProgress) { delete m_workDoneProgress; }
	}

	void setWorkDoneProgress(bool *workDoneProgress) { m_workDoneProgress = workDoneProgress; }
	bool *getWorkDoneProgress() { return m_workDoneProgress; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_workDoneProgress) {
			QJsonValue workDoneProgress = QJsonValue::fromVariant(QVariant::fromValue(*m_workDoneProgress));
			obj.insert("workDoneProgress", workDoneProgress);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("workDoneProgress")) {
			auto workDoneProgress = new bool(json.value("workDoneProgress").toVariant().value<bool>());
			m_workDoneProgress = workDoneProgress;
		}

	}
};
class ReferenceOptions:  public WorkDoneProgressOptions {

public:
	ReferenceOptions() = default;
	virtual ~ReferenceOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
typedef double SignatureHelpTriggerKind;
typedef QString DocumentURI;
class ApplyWorkspaceEditResponse: virtual public GoPlsParams {
	bool *m_applied = nullptr;
	QString *m_failureReason = nullptr;
	unsigned int *m_failedChange = nullptr;

public:
	ApplyWorkspaceEditResponse() = default;
	virtual ~ApplyWorkspaceEditResponse() {
		if(m_applied) { delete m_applied; }
		if(m_failureReason) { delete m_failureReason; }
		if(m_failedChange) { delete m_failedChange; }
	}

	void setApplied(bool *applied) { m_applied = applied; }
	bool *getApplied() { return m_applied; }
	void setFailureReason(QString *failureReason) { m_failureReason = failureReason; }
	QString *getFailureReason() { return m_failureReason; }
	void setFailedChange(unsigned int *failedChange) { m_failedChange = failedChange; }
	unsigned int *getFailedChange() { return m_failedChange; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_applied) {
			QJsonValue applied = QJsonValue::fromVariant(QVariant::fromValue(*m_applied));
			obj.insert("applied", applied);
		}
		if(m_failureReason) {
			QJsonValue failureReason = QJsonValue::fromVariant(QVariant::fromValue(*m_failureReason));
			obj.insert("failureReason", failureReason);
		}
		if(m_failedChange) {
			QJsonValue failedChange = QJsonValue::fromVariant(QVariant::fromValue(*m_failedChange));
			obj.insert("failedChange", failedChange);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("applied")) {
			auto applied = new bool(json.value("applied").toVariant().value<bool>());
			m_applied = applied;
		}
		if(json.contains("failureReason")) {
			auto failureReason = new QString(json.value("failureReason").toString());
			m_failureReason = failureReason;
		}
		if(json.contains("failedChange")) {
			auto failedChange = new unsigned int(json.value("failedChange").toVariant().value<unsigned int>());
			m_failedChange = failedChange;
		}

	}
};
const QString SemanticTokenEnumMember = "enumMember";
const int MessageTypeLog = 4;
const QString UniquenessLevelDocument = "document";
class RegularExpressionsClientCapabilities: virtual public GoPlsParams {
	QString *m_engine = nullptr;
	QString *m_version = nullptr;

public:
	RegularExpressionsClientCapabilities() = default;
	virtual ~RegularExpressionsClientCapabilities() {
		if(m_engine) { delete m_engine; }
		if(m_version) { delete m_version; }
	}

	void setEngine(QString *engine) { m_engine = engine; }
	QString *getEngine() { return m_engine; }
	void setVersion(QString *version) { m_version = version; }
	QString *getVersion() { return m_version; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_engine) {
			QJsonValue engine = QJsonValue::fromVariant(QVariant::fromValue(*m_engine));
			obj.insert("engine", engine);
		}
		if(m_version) {
			QJsonValue version = QJsonValue::fromVariant(QVariant::fromValue(*m_version));
			obj.insert("version", version);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("engine")) {
			auto engine = new QString(json.value("engine").toString());
			m_engine = engine;
		}
		if(json.contains("version")) {
			auto version = new QString(json.value("version").toString());
			m_version = version;
		}

	}
};
class MarkdownClientCapabilities: virtual public GoPlsParams {
	QString *m_parser = nullptr;
	QString *m_version = nullptr;

public:
	MarkdownClientCapabilities() = default;
	virtual ~MarkdownClientCapabilities() {
		if(m_parser) { delete m_parser; }
		if(m_version) { delete m_version; }
	}

	void setParser(QString *parser) { m_parser = parser; }
	QString *getParser() { return m_parser; }
	void setVersion(QString *version) { m_version = version; }
	QString *getVersion() { return m_version; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_parser) {
			QJsonValue parser = QJsonValue::fromVariant(QVariant::fromValue(*m_parser));
			obj.insert("parser", parser);
		}
		if(m_version) {
			QJsonValue version = QJsonValue::fromVariant(QVariant::fromValue(*m_version));
			obj.insert("version", version);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("parser")) {
			auto parser = new QString(json.value("parser").toString());
			m_parser = parser;
		}
		if(json.contains("version")) {
			auto version = new QString(json.value("version").toString());
			m_version = version;
		}

	}
};
class GeneralClientCapabilities: virtual public GoPlsParams {
	RegularExpressionsClientCapabilities *m_regularExpressions = nullptr;
	MarkdownClientCapabilities *m_markdown = nullptr;

public:
	GeneralClientCapabilities() = default;
	virtual ~GeneralClientCapabilities() {
		if(m_regularExpressions) { delete m_regularExpressions; }
		if(m_markdown) { delete m_markdown; }
	}

	void setRegularExpressions(RegularExpressionsClientCapabilities *regularExpressions) { m_regularExpressions = regularExpressions; }
	RegularExpressionsClientCapabilities *getRegularExpressions() { return m_regularExpressions; }
	void setMarkdown(MarkdownClientCapabilities *markdown) { m_markdown = markdown; }
	MarkdownClientCapabilities *getMarkdown() { return m_markdown; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_regularExpressions) {
			QJsonObject regularExpressions = m_regularExpressions->toJson();
			obj.insert("regularExpressions", regularExpressions);
		}
		if(m_markdown) {
			QJsonObject markdown = m_markdown->toJson();
			obj.insert("markdown", markdown);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("regularExpressions")) {
			auto regularExpressions = new RegularExpressionsClientCapabilities();
			regularExpressions->fromJson(json.value("regularExpressions").toObject());
			m_regularExpressions = regularExpressions;
		}
		if(json.contains("markdown")) {
			auto markdown = new MarkdownClientCapabilities();
			markdown->fromJson(json.value("markdown").toObject());
			m_markdown = markdown;
		}

	}
};
typedef QString UniquenessLevel;
class TextDocumentContentChangeEvent: virtual public GoPlsParams {
	Range *m_range = nullptr;
	unsigned int *m_rangeLength = nullptr;
	QString *m_text = nullptr;

public:
	TextDocumentContentChangeEvent() = default;
	virtual ~TextDocumentContentChangeEvent() {
		if(m_range) { delete m_range; }
		if(m_rangeLength) { delete m_rangeLength; }
		if(m_text) { delete m_text; }
	}

	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setRangeLength(unsigned int *rangeLength) { m_rangeLength = rangeLength; }
	unsigned int *getRangeLength() { return m_rangeLength; }
	void setText(QString *text) { m_text = text; }
	QString *getText() { return m_text; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_rangeLength) {
			QJsonValue rangeLength = QJsonValue::fromVariant(QVariant::fromValue(*m_rangeLength));
			obj.insert("rangeLength", rangeLength);
		}
		if(m_text) {
			QJsonValue text = QJsonValue::fromVariant(QVariant::fromValue(*m_text));
			obj.insert("text", text);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("rangeLength")) {
			auto rangeLength = new unsigned int(json.value("rangeLength").toVariant().value<unsigned int>());
			m_rangeLength = rangeLength;
		}
		if(json.contains("text")) {
			auto text = new QString(json.value("text").toString());
			m_text = text;
		}

	}
};
const QString PowershellLanguage = "powershell";
const QString CreateResourceOperation = "create";
typedef QList<QString*> Strings;
const int MessageTypeError = 1;
typedef double SymbolKind;
typedef double SymbolTag;
class CallHierarchyItem: virtual public GoPlsParams {
	QString *m_name = nullptr;
	SymbolKind *m_kind = nullptr;
	QList<SymbolTag*> *m_tags = nullptr;
	QString *m_detail = nullptr;
	DocumentURI *m_uri = nullptr;
	Range *m_range = nullptr;
	Range *m_selectionRange = nullptr;
	QJsonValue *m_data = nullptr;

public:
	CallHierarchyItem() = default;
	virtual ~CallHierarchyItem() {
		if(m_name) { delete m_name; }
		if(m_kind) { delete m_kind; }
		if(m_tags) { qDeleteAll(*m_tags); delete m_tags; }
		if(m_detail) { delete m_detail; }
		if(m_uri) { delete m_uri; }
		if(m_range) { delete m_range; }
		if(m_selectionRange) { delete m_selectionRange; }
		if(m_data) { delete m_data; }
	}

	void setName(QString *name) { m_name = name; }
	QString *getName() { return m_name; }
	void setKind(SymbolKind *kind) { m_kind = kind; }
	SymbolKind *getKind() { return m_kind; }
	void setTags(QList<SymbolTag*> *tags) { m_tags = tags; }
	QList<SymbolTag*> *getTags() { return m_tags; }
	void setDetail(QString *detail) { m_detail = detail; }
	QString *getDetail() { return m_detail; }
	void setUri(DocumentURI *uri) { m_uri = uri; }
	DocumentURI *getUri() { return m_uri; }
	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setSelectionRange(Range *selectionRange) { m_selectionRange = selectionRange; }
	Range *getSelectionRange() { return m_selectionRange; }
	void setData(QJsonValue *data) { m_data = data; }
	QJsonValue *getData() { return m_data; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_name) {
			QJsonValue name = QJsonValue::fromVariant(QVariant::fromValue(*m_name));
			obj.insert("name", name);
		}
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_tags) {
				QJsonArray tags;
			for(auto it = m_tags->cbegin(); it != m_tags->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				tags.append(item);
			}

			obj.insert("tags", tags);
		}
		if(m_detail) {
			QJsonValue detail = QJsonValue::fromVariant(QVariant::fromValue(*m_detail));
			obj.insert("detail", detail);
		}
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_selectionRange) {
			QJsonObject selectionRange = m_selectionRange->toJson();
			obj.insert("selectionRange", selectionRange);
		}
		if(m_data) {
			QJsonValue data = *m_data;
			obj.insert("data", data);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("name")) {
			auto name = new QString(json.value("name").toString());
			m_name = name;
		}
		if(json.contains("kind")) {
			auto kind = new double(json.value("kind").toVariant().value<double>());
			m_kind = kind;
		}
		if(json.contains("tags")) {
			 QJsonArray arr = json.value("tags").toArray();
			auto tags = new QList<SymbolTag*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				tags->append(item);
			}
			m_tags = tags;
		}
		if(json.contains("detail")) {
			auto detail = new QString(json.value("detail").toString());
			m_detail = detail;
		}
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("selectionRange")) {
			auto selectionRange = new Range();
			selectionRange->fromJson(json.value("selectionRange").toObject());
			m_selectionRange = selectionRange;
		}
		if(json.contains("data")) {
			auto data = new QJsonValue(json.value("data"));
			m_data = data;
		}

	}
};
class TextDocumentIdentifier: virtual public GoPlsParams {
	DocumentURI *m_uri = nullptr;

public:
	TextDocumentIdentifier() = default;
	virtual ~TextDocumentIdentifier() {
		if(m_uri) { delete m_uri; }
	}

	void setUri(DocumentURI *uri) { m_uri = uri; }
	DocumentURI *getUri() { return m_uri; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}

	}
};
class OptionalVersionedTextDocumentIdentifier:  public TextDocumentIdentifier {
	int *m_version = nullptr;

public:
	OptionalVersionedTextDocumentIdentifier() = default;
	virtual ~OptionalVersionedTextDocumentIdentifier() {
		if(m_version) { delete m_version; }
	}

	void setVersion(int *version) { m_version = version; }
	int *getVersion() { return m_version; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentIdentifier_json = TextDocumentIdentifier::toJson();
		for(const QString &key: TextDocumentIdentifier_json.keys()) {
			obj.insert(key, TextDocumentIdentifier_json.value(key));
		}
		if(m_version) {
			QJsonValue version = QJsonValue::fromVariant(QVariant::fromValue(*m_version));
			obj.insert("version", version);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentIdentifier::fromJson(json);
		if(json.contains("version")) {
			auto version = new int(json.value("version").toVariant().value<int>());
			m_version = version;
		}

	}
};
class TextEdit: virtual public GoPlsParams {
	Range *m_range = nullptr;
	QString *m_newText = nullptr;

public:
	TextEdit() = default;
	virtual ~TextEdit() {
		if(m_range) { delete m_range; }
		if(m_newText) { delete m_newText; }
	}

	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setNewText(QString *newText) { m_newText = newText; }
	QString *getNewText() { return m_newText; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_newText) {
			QJsonValue newText = QJsonValue::fromVariant(QVariant::fromValue(*m_newText));
			obj.insert("newText", newText);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("newText")) {
			auto newText = new QString(json.value("newText").toString());
			m_newText = newText;
		}

	}
};
class TextDocumentEdit: virtual public GoPlsParams {
	OptionalVersionedTextDocumentIdentifier *m_textDocument = nullptr;
	QList<TextEdit*> *m_edits = nullptr;

public:
	TextDocumentEdit() = default;
	virtual ~TextDocumentEdit() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_edits) { qDeleteAll(*m_edits); delete m_edits; }
	}

	void setTextDocument(OptionalVersionedTextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	OptionalVersionedTextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setEdits(QList<TextEdit*> *edits) { m_edits = edits; }
	QList<TextEdit*> *getEdits() { return m_edits; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_edits) {
				QJsonArray edits;
			for(auto it = m_edits->cbegin(); it != m_edits->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				edits.append(item);
			}

			obj.insert("edits", edits);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("textDocument")) {
			auto textDocument = new OptionalVersionedTextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("edits")) {
			 QJsonArray arr = json.value("edits").toArray();
			auto edits = new QList<TextEdit*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new TextEdit();
			item->fromJson((m_item).toObject());
				edits->append(item);
			}
			m_edits = edits;
		}

	}
};
typedef QString ChangeAnnotationIdentifier;
class ChangeAnnotation: virtual public GoPlsParams {
	QString *m_label = nullptr;
	bool *m_needsConfirmation = nullptr;
	QString *m_description = nullptr;

public:
	ChangeAnnotation() = default;
	virtual ~ChangeAnnotation() {
		if(m_label) { delete m_label; }
		if(m_needsConfirmation) { delete m_needsConfirmation; }
		if(m_description) { delete m_description; }
	}

	void setLabel(QString *label) { m_label = label; }
	QString *getLabel() { return m_label; }
	void setNeedsConfirmation(bool *needsConfirmation) { m_needsConfirmation = needsConfirmation; }
	bool *getNeedsConfirmation() { return m_needsConfirmation; }
	void setDescription(QString *description) { m_description = description; }
	QString *getDescription() { return m_description; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_label) {
			QJsonValue label = QJsonValue::fromVariant(QVariant::fromValue(*m_label));
			obj.insert("label", label);
		}
		if(m_needsConfirmation) {
			QJsonValue needsConfirmation = QJsonValue::fromVariant(QVariant::fromValue(*m_needsConfirmation));
			obj.insert("needsConfirmation", needsConfirmation);
		}
		if(m_description) {
			QJsonValue description = QJsonValue::fromVariant(QVariant::fromValue(*m_description));
			obj.insert("description", description);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("label")) {
			auto label = new QString(json.value("label").toString());
			m_label = label;
		}
		if(json.contains("needsConfirmation")) {
			auto needsConfirmation = new bool(json.value("needsConfirmation").toVariant().value<bool>());
			m_needsConfirmation = needsConfirmation;
		}
		if(json.contains("description")) {
			auto description = new QString(json.value("description").toString());
			m_description = description;
		}

	}
};
class WorkspaceEdit: virtual public GoPlsParams {
	QHash<DocumentURI,QList<TextEdit*>*> *m_changes = nullptr;
	QList<TextDocumentEdit*> *m_documentChanges = nullptr;
	QHash<ChangeAnnotationIdentifier,ChangeAnnotation*> *m_changeAnnotations = nullptr;

public:
	WorkspaceEdit() = default;
	virtual ~WorkspaceEdit() {
		if(m_changes) { qDeleteAll(m_changes->values()); delete m_changes; }
		if(m_documentChanges) { qDeleteAll(*m_documentChanges); delete m_documentChanges; }
		if(m_changeAnnotations) { qDeleteAll(m_changeAnnotations->values()); delete m_changeAnnotations; }
	}

	void setChanges(QHash<DocumentURI,QList<TextEdit*>*> *changes) { m_changes = changes; }
	QHash<DocumentURI,QList<TextEdit*>*> *getChanges() { return m_changes; }
	void setDocumentChanges(QList<TextDocumentEdit*> *documentChanges) { m_documentChanges = documentChanges; }
	QList<TextDocumentEdit*> *getDocumentChanges() { return m_documentChanges; }
	void setChangeAnnotations(QHash<ChangeAnnotationIdentifier,ChangeAnnotation*> *changeAnnotations) { m_changeAnnotations = changeAnnotations; }
	QHash<ChangeAnnotationIdentifier,ChangeAnnotation*> *getChangeAnnotations() { return m_changeAnnotations; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_changes) {
			QJsonObject changes;
			for(auto hash_it = m_changes->cbegin(); hash_it != m_changes->cend(); ++hash_it) {
				auto m_hash_item = *hash_it;
					QJsonArray hash_item;
			for(auto it = m_hash_item->cbegin(); it != m_hash_item->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				hash_item.append(item);
			}

				changes.insert(hash_it.key(), hash_item);
			}
			obj.insert("changes", changes);
		}
		if(m_documentChanges) {
				QJsonArray documentChanges;
			for(auto it = m_documentChanges->cbegin(); it != m_documentChanges->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				documentChanges.append(item);
			}

			obj.insert("documentChanges", documentChanges);
		}
		if(m_changeAnnotations) {
			QJsonObject changeAnnotations;
			for(auto hash_it = m_changeAnnotations->cbegin(); hash_it != m_changeAnnotations->cend(); ++hash_it) {
				auto m_hash_item = *hash_it;
				QJsonObject hash_item = m_hash_item->toJson();
				changeAnnotations.insert(hash_it.key(), hash_item);
			}
			obj.insert("changeAnnotations", changeAnnotations);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("changes")) {
			 QJsonObject obj = json.value("changes").toObject();
			QStringList objKeys = obj.keys();
			auto changes = new QHash<DocumentURI,QList<TextEdit*>*>();
			for(auto m_it = objKeys.cbegin(); m_it != objKeys.cend(); ++m_it) {
				 QJsonArray arr = obj.value(*m_it).toArray();
			auto hash = new QList<TextEdit*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new TextEdit();
			item->fromJson((m_item).toObject());
				hash->append(item);
			}
				changes->insert(*m_it, hash);
			}
			m_changes = changes;
		}
		if(json.contains("documentChanges")) {
			 QJsonArray arr = json.value("documentChanges").toArray();
			auto documentChanges = new QList<TextDocumentEdit*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new TextDocumentEdit();
			item->fromJson((m_item).toObject());
				documentChanges->append(item);
			}
			m_documentChanges = documentChanges;
		}
		if(json.contains("changeAnnotations")) {
			 QJsonObject obj = json.value("changeAnnotations").toObject();
			QStringList objKeys = obj.keys();
			auto changeAnnotations = new QHash<ChangeAnnotationIdentifier,ChangeAnnotation*>();
			for(auto m_it = objKeys.cbegin(); m_it != objKeys.cend(); ++m_it) {
				auto hash = new ChangeAnnotation();
			hash->fromJson(obj.value(*m_it).toObject());
				changeAnnotations->insert(*m_it, hash);
			}
			m_changeAnnotations = changeAnnotations;
		}

	}
};
class ApplyWorkspaceEditParams: virtual public GoPlsParams {
	QString *m_label = nullptr;
	WorkspaceEdit *m_edit = nullptr;

public:
	ApplyWorkspaceEditParams() = default;
	virtual ~ApplyWorkspaceEditParams() {
		if(m_label) { delete m_label; }
		if(m_edit) { delete m_edit; }
	}

	void setLabel(QString *label) { m_label = label; }
	QString *getLabel() { return m_label; }
	void setEdit(WorkspaceEdit *edit) { m_edit = edit; }
	WorkspaceEdit *getEdit() { return m_edit; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_label) {
			QJsonValue label = QJsonValue::fromVariant(QVariant::fromValue(*m_label));
			obj.insert("label", label);
		}
		if(m_edit) {
			QJsonObject edit = m_edit->toJson();
			obj.insert("edit", edit);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("label")) {
			auto label = new QString(json.value("label").toString());
			m_label = label;
		}
		if(json.contains("edit")) {
			auto edit = new WorkspaceEdit();
			edit->fromJson(json.value("edit").toObject());
			m_edit = edit;
		}

	}
};
class Registration: virtual public GoPlsParams {
	QString *m_id = nullptr;
	QString *m_method = nullptr;
	QJsonValue *m_registerOptions = nullptr;

public:
	Registration() = default;
	virtual ~Registration() {
		if(m_id) { delete m_id; }
		if(m_method) { delete m_method; }
		if(m_registerOptions) { delete m_registerOptions; }
	}

	void setId(QString *id) { m_id = id; }
	QString *getId() { return m_id; }
	void setMethod(QString *method) { m_method = method; }
	QString *getMethod() { return m_method; }
	void setRegisterOptions(QJsonValue *registerOptions) { m_registerOptions = registerOptions; }
	QJsonValue *getRegisterOptions() { return m_registerOptions; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_id) {
			QJsonValue id = QJsonValue::fromVariant(QVariant::fromValue(*m_id));
			obj.insert("id", id);
		}
		if(m_method) {
			QJsonValue method = QJsonValue::fromVariant(QVariant::fromValue(*m_method));
			obj.insert("method", method);
		}
		if(m_registerOptions) {
			QJsonValue registerOptions = *m_registerOptions;
			obj.insert("registerOptions", registerOptions);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("id")) {
			auto id = new QString(json.value("id").toString());
			m_id = id;
		}
		if(json.contains("method")) {
			auto method = new QString(json.value("method").toString());
			m_method = method;
		}
		if(json.contains("registerOptions")) {
			auto registerOptions = new QJsonValue(json.value("registerOptions"));
			m_registerOptions = registerOptions;
		}

	}
};
class RegistrationParams: virtual public GoPlsParams {
	QList<Registration*> *m_registrations = nullptr;

public:
	RegistrationParams() = default;
	virtual ~RegistrationParams() {
		if(m_registrations) { qDeleteAll(*m_registrations); delete m_registrations; }
	}

	void setRegistrations(QList<Registration*> *registrations) { m_registrations = registrations; }
	QList<Registration*> *getRegistrations() { return m_registrations; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_registrations) {
				QJsonArray registrations;
			for(auto it = m_registrations->cbegin(); it != m_registrations->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				registrations.append(item);
			}

			obj.insert("registrations", registrations);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("registrations")) {
			 QJsonArray arr = json.value("registrations").toArray();
			auto registrations = new QList<Registration*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new Registration();
			item->fromJson((m_item).toObject());
				registrations->append(item);
			}
			m_registrations = registrations;
		}

	}
};
const QString CppLanguage = "cpp";
const QString ErlangLanguage = "erlang";
typedef QString MarkupKind;
typedef QList<MarkupKind*> MarkupKinds;
typedef QList<QJsonValue*> Interfaces;
class ParameterInformation: virtual public GoPlsParams {
	QString *m_label = nullptr;
	QJsonValue *m_documentation = nullptr;

public:
	ParameterInformation() = default;
	virtual ~ParameterInformation() {
		if(m_label) { delete m_label; }
		if(m_documentation) { delete m_documentation; }
	}

	void setLabel(QString *label) { m_label = label; }
	QString *getLabel() { return m_label; }
	void setDocumentation(QJsonValue *documentation) { m_documentation = documentation; }
	QJsonValue *getDocumentation() { return m_documentation; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_label) {
			QJsonValue label = QJsonValue::fromVariant(QVariant::fromValue(*m_label));
			obj.insert("label", label);
		}
		if(m_documentation) {
			QJsonValue documentation = *m_documentation;
			obj.insert("documentation", documentation);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("label")) {
			auto label = new QString(json.value("label").toString());
			m_label = label;
		}
		if(json.contains("documentation")) {
			auto documentation = new QJsonValue(json.value("documentation"));
			m_documentation = documentation;
		}

	}
};
typedef QList<ParameterInformation*> ParameterInformations;
const QString MethodWillRenameFiles = "workspace/willRenameFiles";
const QString SemanticTokenClass = "class";
const int SymbolKindEnumMember = 22;
typedef QJsonValue ProgressToken;
class WorkDoneProgressParams: virtual public GoPlsParams {
	ProgressToken *m_workDoneToken = nullptr;

public:
	WorkDoneProgressParams() = default;
	virtual ~WorkDoneProgressParams() {
		if(m_workDoneToken) { delete m_workDoneToken; }
	}

	void setWorkDoneToken(ProgressToken *workDoneToken) { m_workDoneToken = workDoneToken; }
	ProgressToken *getWorkDoneToken() { return m_workDoneToken; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_workDoneToken) {
			QJsonValue workDoneToken = *m_workDoneToken;
			obj.insert("workDoneToken", workDoneToken);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("workDoneToken")) {
			auto workDoneToken = new QJsonValue(json.value("workDoneToken"));
			m_workDoneToken = workDoneToken;
		}

	}
};
class PartialResultParams: virtual public GoPlsParams {
	ProgressToken *m_partialResultToken = nullptr;

public:
	PartialResultParams() = default;
	virtual ~PartialResultParams() {
		if(m_partialResultToken) { delete m_partialResultToken; }
	}

	void setPartialResultToken(ProgressToken *partialResultToken) { m_partialResultToken = partialResultToken; }
	ProgressToken *getPartialResultToken() { return m_partialResultToken; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_partialResultToken) {
			QJsonValue partialResultToken = *m_partialResultToken;
			obj.insert("partialResultToken", partialResultToken);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("partialResultToken")) {
			auto partialResultToken = new QJsonValue(json.value("partialResultToken"));
			m_partialResultToken = partialResultToken;
		}

	}
};
class DocumentColorParams:  public WorkDoneProgressParams, public PartialResultParams {
	TextDocumentIdentifier *m_textDocument = nullptr;

public:
	DocumentColorParams() = default;
	virtual ~DocumentColorParams() {
		if(m_textDocument) { delete m_textDocument; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}

	}
};
typedef QString FailureHandlingKind;
const int SymbolKindClass = 5;
class SymbolKindCapabilities: virtual public GoPlsParams {
	QList<SymbolKind*> *m_valueSet = nullptr;

public:
	SymbolKindCapabilities() = default;
	virtual ~SymbolKindCapabilities() {
		if(m_valueSet) { qDeleteAll(*m_valueSet); delete m_valueSet; }
	}

	void setValueSet(QList<SymbolKind*> *valueSet) { m_valueSet = valueSet; }
	QList<SymbolKind*> *getValueSet() { return m_valueSet; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_valueSet) {
				QJsonArray valueSet;
			for(auto it = m_valueSet->cbegin(); it != m_valueSet->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				valueSet.append(item);
			}

			obj.insert("valueSet", valueSet);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("valueSet")) {
			 QJsonArray arr = json.value("valueSet").toArray();
			auto valueSet = new QList<SymbolKind*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				valueSet->append(item);
			}
			m_valueSet = valueSet;
		}

	}
};
const int TextDocumentSaveReasonAfterDelay = 2;
const int CompletionItemKindFile = 17;
const int SymbolKindVariable = 13;
const int CompletionItemKindOperator = 24;
const QString MethodTextDocumentRangeFormatting = "textDocument/rangeFormatting";
const QString Version = "3.15.3";
const QString ShaderlabLanguage = "shaderlab";
const QString TypeScriptReactLanguage = "typescriptreact";
const int DiagnosticTagDeprecated = 2;
const int SymbolKindBoolean = 17;
typedef QString WorkDoneProgressKind;
class SemanticTokens: virtual public GoPlsParams {
	QString *m_resultId = nullptr;
	QList<unsigned int*> *m_data = nullptr;

public:
	SemanticTokens() = default;
	virtual ~SemanticTokens() {
		if(m_resultId) { delete m_resultId; }
		if(m_data) { qDeleteAll(*m_data); delete m_data; }
	}

	void setResultId(QString *resultId) { m_resultId = resultId; }
	QString *getResultId() { return m_resultId; }
	void setData(QList<unsigned int*> *data) { m_data = data; }
	QList<unsigned int*> *getData() { return m_data; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_resultId) {
			QJsonValue resultId = QJsonValue::fromVariant(QVariant::fromValue(*m_resultId));
			obj.insert("resultId", resultId);
		}
		if(m_data) {
				QJsonArray data;
			for(auto it = m_data->cbegin(); it != m_data->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				data.append(item);
			}

			obj.insert("data", data);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("resultId")) {
			auto resultId = new QString(json.value("resultId").toString());
			m_resultId = resultId;
		}
		if(json.contains("data")) {
			 QJsonArray arr = json.value("data").toArray();
			auto data = new QList<unsigned int*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new unsigned int((m_item).toVariant().value<unsigned int>());
				data->append(item);
			}
			m_data = data;
		}

	}
};
const int SignatureHelpTriggerKindTriggerCharacter = 2;
const int DocumentHighlightKindWrite = 3;
class ShowMessageRequestClientCapabilitiesMessageActionItem: virtual public GoPlsParams {
	bool *m_additionalPropertiesSupport = nullptr;

public:
	ShowMessageRequestClientCapabilitiesMessageActionItem() = default;
	virtual ~ShowMessageRequestClientCapabilitiesMessageActionItem() {
		if(m_additionalPropertiesSupport) { delete m_additionalPropertiesSupport; }
	}

	void setAdditionalPropertiesSupport(bool *additionalPropertiesSupport) { m_additionalPropertiesSupport = additionalPropertiesSupport; }
	bool *getAdditionalPropertiesSupport() { return m_additionalPropertiesSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_additionalPropertiesSupport) {
			QJsonValue additionalPropertiesSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_additionalPropertiesSupport));
			obj.insert("additionalPropertiesSupport", additionalPropertiesSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("additionalPropertiesSupport")) {
			auto additionalPropertiesSupport = new bool(json.value("additionalPropertiesSupport").toVariant().value<bool>());
			m_additionalPropertiesSupport = additionalPropertiesSupport;
		}

	}
};
const QString ABAPLanguage = "abap";
const QString SemanticTokenModifierModification = "modification";
class ReferencesOptions:  public WorkDoneProgressOptions {

public:
	ReferencesOptions() = default;
	virtual ~ReferencesOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
class StaticRegistrationOptions: virtual public GoPlsParams {
	QString *m_id = nullptr;

public:
	StaticRegistrationOptions() = default;
	virtual ~StaticRegistrationOptions() {
		if(m_id) { delete m_id; }
	}

	void setId(QString *id) { m_id = id; }
	QString *getId() { return m_id; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_id) {
			QJsonValue id = QJsonValue::fromVariant(QVariant::fromValue(*m_id));
			obj.insert("id", id);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("id")) {
			auto id = new QString(json.value("id").toString());
			m_id = id;
		}

	}
};
const int SymbolKindEvent = 24;
class DidChangeConfigurationWorkspaceClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	DidChangeConfigurationWorkspaceClientCapabilities() = default;
	virtual ~DidChangeConfigurationWorkspaceClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
typedef DidChangeConfigurationWorkspaceClientCapabilities WorkspaceClientCapabilitiesDidChangeConfiguration;
const QString MethodExit = "exit";
const QString MethodTextDocumentPrepareRename = "textDocument/prepareRename";
class DidCloseTextDocumentParams: virtual public GoPlsParams {
	TextDocumentIdentifier *m_textDocument = nullptr;

public:
	DidCloseTextDocumentParams() = default;
	virtual ~DidCloseTextDocumentParams() {
		if(m_textDocument) { delete m_textDocument; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}

	}
};
const QString MarkdownLanguage = "markdown";
class CodeLensParams:  public WorkDoneProgressParams, public PartialResultParams {
	TextDocumentIdentifier *m_textDocument = nullptr;

public:
	CodeLensParams() = default;
	virtual ~CodeLensParams() {
		if(m_textDocument) { delete m_textDocument; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}

	}
};
class DocumentRangeFormattingClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	DocumentRangeFormattingClientCapabilities() = default;
	virtual ~DocumentRangeFormattingClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
const QString MethodWorkspaceApplyEdit = "workspace/applyEdit";
typedef double FileChangeType;
class FileEvent: virtual public GoPlsParams {
	FileChangeType *m_type = nullptr;
	QString *m_uri = nullptr;

public:
	FileEvent() = default;
	virtual ~FileEvent() {
		if(m_type) { delete m_type; }
		if(m_uri) { delete m_uri; }
	}

	void setType(FileChangeType *type) { m_type = type; }
	FileChangeType *getType() { return m_type; }
	void setUri(QString *uri) { m_uri = uri; }
	QString *getUri() { return m_uri; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_type) {
			QJsonValue type = QJsonValue::fromVariant(QVariant::fromValue(*m_type));
			obj.insert("type", type);
		}
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("type")) {
			auto type = new double(json.value("type").toVariant().value<double>());
			m_type = type;
		}
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}

	}
};
class DidChangeWatchedFilesParams: virtual public GoPlsParams {
	QList<FileEvent*> *m_changes = nullptr;

public:
	DidChangeWatchedFilesParams() = default;
	virtual ~DidChangeWatchedFilesParams() {
		if(m_changes) { qDeleteAll(*m_changes); delete m_changes; }
	}

	void setChanges(QList<FileEvent*> *changes) { m_changes = changes; }
	QList<FileEvent*> *getChanges() { return m_changes; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_changes) {
				QJsonArray changes;
			for(auto it = m_changes->cbegin(); it != m_changes->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				changes.append(item);
			}

			obj.insert("changes", changes);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("changes")) {
			 QJsonArray arr = json.value("changes").toArray();
			auto changes = new QList<FileEvent*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new FileEvent();
			item->fromJson((m_item).toObject());
				changes->append(item);
			}
			m_changes = changes;
		}

	}
};
class WorkspaceSymbolParams:  public WorkDoneProgressParams, public PartialResultParams {
	QString *m_query = nullptr;

public:
	WorkspaceSymbolParams() = default;
	virtual ~WorkspaceSymbolParams() {
		if(m_query) { delete m_query; }
	}

	void setQuery(QString *query) { m_query = query; }
	QString *getQuery() { return m_query; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_query) {
			QJsonValue query = QJsonValue::fromVariant(QVariant::fromValue(*m_query));
			obj.insert("query", query);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("query")) {
			auto query = new QString(json.value("query").toString());
			m_query = query;
		}

	}
};
const QString SemanticTokenComment = "comment";
typedef QList<SymbolTag*> SymbolTags;
class TextDocumentPositionParams: virtual public GoPlsParams {
	TextDocumentIdentifier *m_textDocument = nullptr;
	Position *m_position = nullptr;

public:
	TextDocumentPositionParams() = default;
	virtual ~TextDocumentPositionParams() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_position) { delete m_position; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setPosition(Position *position) { m_position = position; }
	Position *getPosition() { return m_position; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_position) {
			QJsonObject position = m_position->toJson();
			obj.insert("position", position);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("position")) {
			auto position = new Position();
			position->fromJson(json.value("position").toObject());
			m_position = position;
		}

	}
};
class ReferenceContext: virtual public GoPlsParams {
	bool *m_includeDeclaration = nullptr;

public:
	ReferenceContext() = default;
	virtual ~ReferenceContext() {
		if(m_includeDeclaration) { delete m_includeDeclaration; }
	}

	void setIncludeDeclaration(bool *includeDeclaration) { m_includeDeclaration = includeDeclaration; }
	bool *getIncludeDeclaration() { return m_includeDeclaration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_includeDeclaration) {
			QJsonValue includeDeclaration = QJsonValue::fromVariant(QVariant::fromValue(*m_includeDeclaration));
			obj.insert("includeDeclaration", includeDeclaration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("includeDeclaration")) {
			auto includeDeclaration = new bool(json.value("includeDeclaration").toVariant().value<bool>());
			m_includeDeclaration = includeDeclaration;
		}

	}
};
class ReferenceParams:  public TextDocumentPositionParams, public WorkDoneProgressParams, public PartialResultParams {
	ReferenceContext *m_context = nullptr;

public:
	ReferenceParams() = default;
	virtual ~ReferenceParams() {
		if(m_context) { delete m_context; }
	}

	void setContext(ReferenceContext *context) { m_context = context; }
	ReferenceContext *getContext() { return m_context; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_context) {
			QJsonObject context = m_context->toJson();
			obj.insert("context", context);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("context")) {
			auto context = new ReferenceContext();
			context->fromJson(json.value("context").toObject());
			m_context = context;
		}

	}
};
class ShowDocumentClientCapabilities: virtual public GoPlsParams {
	bool *m_support = nullptr;

public:
	ShowDocumentClientCapabilities() = default;
	virtual ~ShowDocumentClientCapabilities() {
		if(m_support) { delete m_support; }
	}

	void setSupport(bool *support) { m_support = support; }
	bool *getSupport() { return m_support; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_support) {
			QJsonValue support = QJsonValue::fromVariant(QVariant::fromValue(*m_support));
			obj.insert("support", support);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("support")) {
			auto support = new bool(json.value("support").toVariant().value<bool>());
			m_support = support;
		}

	}
};
typedef ShowDocumentClientCapabilities ClientCapabilitiesShowDocument;
const int WatchKindChange = 2;
typedef double InsertTextMode;
class CompletionTextDocumentClientCapabilitiesItemInsertTextModeSupport: virtual public GoPlsParams {
	QList<InsertTextMode*> *m_valueSet = nullptr;

public:
	CompletionTextDocumentClientCapabilitiesItemInsertTextModeSupport() = default;
	virtual ~CompletionTextDocumentClientCapabilitiesItemInsertTextModeSupport() {
		if(m_valueSet) { qDeleteAll(*m_valueSet); delete m_valueSet; }
	}

	void setValueSet(QList<InsertTextMode*> *valueSet) { m_valueSet = valueSet; }
	QList<InsertTextMode*> *getValueSet() { return m_valueSet; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_valueSet) {
				QJsonArray valueSet;
			for(auto it = m_valueSet->cbegin(); it != m_valueSet->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				valueSet.append(item);
			}

			obj.insert("valueSet", valueSet);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("valueSet")) {
			 QJsonArray arr = json.value("valueSet").toArray();
			auto valueSet = new QList<InsertTextMode*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				valueSet->append(item);
			}
			m_valueSet = valueSet;
		}

	}
};
typedef CompletionTextDocumentClientCapabilitiesItemInsertTextModeSupport TextDocumentClientCapabilitiesCompletionItemInsertTextModeSupport;
const QString MethodCallHierarchyOutgoingCalls = "callHierarchy/outgoingCalls";
typedef QString ResourceOperationKind;
class CreateFileOptions: virtual public GoPlsParams {
	bool *m_overwrite = nullptr;
	bool *m_ignoreIfExists = nullptr;

public:
	CreateFileOptions() = default;
	virtual ~CreateFileOptions() {
		if(m_overwrite) { delete m_overwrite; }
		if(m_ignoreIfExists) { delete m_ignoreIfExists; }
	}

	void setOverwrite(bool *overwrite) { m_overwrite = overwrite; }
	bool *getOverwrite() { return m_overwrite; }
	void setIgnoreIfExists(bool *ignoreIfExists) { m_ignoreIfExists = ignoreIfExists; }
	bool *getIgnoreIfExists() { return m_ignoreIfExists; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_overwrite) {
			QJsonValue overwrite = QJsonValue::fromVariant(QVariant::fromValue(*m_overwrite));
			obj.insert("overwrite", overwrite);
		}
		if(m_ignoreIfExists) {
			QJsonValue ignoreIfExists = QJsonValue::fromVariant(QVariant::fromValue(*m_ignoreIfExists));
			obj.insert("ignoreIfExists", ignoreIfExists);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("overwrite")) {
			auto overwrite = new bool(json.value("overwrite").toVariant().value<bool>());
			m_overwrite = overwrite;
		}
		if(json.contains("ignoreIfExists")) {
			auto ignoreIfExists = new bool(json.value("ignoreIfExists").toVariant().value<bool>());
			m_ignoreIfExists = ignoreIfExists;
		}

	}
};
class CreateFile: virtual public GoPlsParams {
	ResourceOperationKind *m_kind = nullptr;
	DocumentURI *m_uri = nullptr;
	CreateFileOptions *m_options = nullptr;
	ChangeAnnotationIdentifier *m_annotationId = nullptr;

public:
	CreateFile() = default;
	virtual ~CreateFile() {
		if(m_kind) { delete m_kind; }
		if(m_uri) { delete m_uri; }
		if(m_options) { delete m_options; }
		if(m_annotationId) { delete m_annotationId; }
	}

	void setKind(ResourceOperationKind *kind) { m_kind = kind; }
	ResourceOperationKind *getKind() { return m_kind; }
	void setUri(DocumentURI *uri) { m_uri = uri; }
	DocumentURI *getUri() { return m_uri; }
	void setOptions(CreateFileOptions *options) { m_options = options; }
	CreateFileOptions *getOptions() { return m_options; }
	void setAnnotationId(ChangeAnnotationIdentifier *annotationId) { m_annotationId = annotationId; }
	ChangeAnnotationIdentifier *getAnnotationId() { return m_annotationId; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}
		if(m_options) {
			QJsonObject options = m_options->toJson();
			obj.insert("options", options);
		}
		if(m_annotationId) {
			QJsonValue annotationId = QJsonValue::fromVariant(QVariant::fromValue(*m_annotationId));
			obj.insert("annotationId", annotationId);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("kind")) {
			auto kind = new QString(json.value("kind").toString());
			m_kind = kind;
		}
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}
		if(json.contains("options")) {
			auto options = new CreateFileOptions();
			options->fromJson(json.value("options").toObject());
			m_options = options;
		}
		if(json.contains("annotationId")) {
			auto annotationId = new QString(json.value("annotationId").toString());
			m_annotationId = annotationId;
		}

	}
};
typedef double DiagnosticTag;
class PublishDiagnosticsClientCapabilitiesTagSupport: virtual public GoPlsParams {
	QList<DiagnosticTag*> *m_valueSet = nullptr;

public:
	PublishDiagnosticsClientCapabilitiesTagSupport() = default;
	virtual ~PublishDiagnosticsClientCapabilitiesTagSupport() {
		if(m_valueSet) { qDeleteAll(*m_valueSet); delete m_valueSet; }
	}

	void setValueSet(QList<DiagnosticTag*> *valueSet) { m_valueSet = valueSet; }
	QList<DiagnosticTag*> *getValueSet() { return m_valueSet; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_valueSet) {
				QJsonArray valueSet;
			for(auto it = m_valueSet->cbegin(); it != m_valueSet->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				valueSet.append(item);
			}

			obj.insert("valueSet", valueSet);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("valueSet")) {
			 QJsonArray arr = json.value("valueSet").toArray();
			auto valueSet = new QList<DiagnosticTag*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				valueSet->append(item);
			}
			m_valueSet = valueSet;
		}

	}
};
typedef PublishDiagnosticsClientCapabilitiesTagSupport TextDocumentClientCapabilitiesPublishDiagnosticsTagSupport;
typedef QString CodeActionKind;
class CodeActionClientCapabilitiesKind: virtual public GoPlsParams {
	QList<CodeActionKind*> *m_valueSet = nullptr;

public:
	CodeActionClientCapabilitiesKind() = default;
	virtual ~CodeActionClientCapabilitiesKind() {
		if(m_valueSet) { qDeleteAll(*m_valueSet); delete m_valueSet; }
	}

	void setValueSet(QList<CodeActionKind*> *valueSet) { m_valueSet = valueSet; }
	QList<CodeActionKind*> *getValueSet() { return m_valueSet; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_valueSet) {
				QJsonArray valueSet;
			for(auto it = m_valueSet->cbegin(); it != m_valueSet->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				valueSet.append(item);
			}

			obj.insert("valueSet", valueSet);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("valueSet")) {
			 QJsonArray arr = json.value("valueSet").toArray();
			auto valueSet = new QList<CodeActionKind*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				valueSet->append(item);
			}
			m_valueSet = valueSet;
		}

	}
};
class CodeActionClientCapabilitiesLiteralSupport: virtual public GoPlsParams {
	CodeActionClientCapabilitiesKind *m_codeActionKind = nullptr;

public:
	CodeActionClientCapabilitiesLiteralSupport() = default;
	virtual ~CodeActionClientCapabilitiesLiteralSupport() {
		if(m_codeActionKind) { delete m_codeActionKind; }
	}

	void setCodeActionKind(CodeActionClientCapabilitiesKind *codeActionKind) { m_codeActionKind = codeActionKind; }
	CodeActionClientCapabilitiesKind *getCodeActionKind() { return m_codeActionKind; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_codeActionKind) {
			QJsonObject codeActionKind = m_codeActionKind->toJson();
			obj.insert("codeActionKind", codeActionKind);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("codeActionKind")) {
			auto codeActionKind = new CodeActionClientCapabilitiesKind();
			codeActionKind->fromJson(json.value("codeActionKind").toObject());
			m_codeActionKind = codeActionKind;
		}

	}
};
class CodeActionClientCapabilitiesResolveSupport: virtual public GoPlsParams {
	QList<QString*> *m_properties = nullptr;

public:
	CodeActionClientCapabilitiesResolveSupport() = default;
	virtual ~CodeActionClientCapabilitiesResolveSupport() {
		if(m_properties) { qDeleteAll(*m_properties); delete m_properties; }
	}

	void setProperties(QList<QString*> *properties) { m_properties = properties; }
	QList<QString*> *getProperties() { return m_properties; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_properties) {
				QJsonArray properties;
			for(auto it = m_properties->cbegin(); it != m_properties->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				properties.append(item);
			}

			obj.insert("properties", properties);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("properties")) {
			 QJsonArray arr = json.value("properties").toArray();
			auto properties = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				properties->append(item);
			}
			m_properties = properties;
		}

	}
};
class CodeActionClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	CodeActionClientCapabilitiesLiteralSupport *m_codeActionLiteralSupport = nullptr;
	bool *m_isPreferredSupport = nullptr;
	bool *m_disabledSupport = nullptr;
	bool *m_dataSupport = nullptr;
	CodeActionClientCapabilitiesResolveSupport *m_resolveSupport = nullptr;
	bool *m_honorsChangeAnnotations = nullptr;

public:
	CodeActionClientCapabilities() = default;
	virtual ~CodeActionClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_codeActionLiteralSupport) { delete m_codeActionLiteralSupport; }
		if(m_isPreferredSupport) { delete m_isPreferredSupport; }
		if(m_disabledSupport) { delete m_disabledSupport; }
		if(m_dataSupport) { delete m_dataSupport; }
		if(m_resolveSupport) { delete m_resolveSupport; }
		if(m_honorsChangeAnnotations) { delete m_honorsChangeAnnotations; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setCodeActionLiteralSupport(CodeActionClientCapabilitiesLiteralSupport *codeActionLiteralSupport) { m_codeActionLiteralSupport = codeActionLiteralSupport; }
	CodeActionClientCapabilitiesLiteralSupport *getCodeActionLiteralSupport() { return m_codeActionLiteralSupport; }
	void setIsPreferredSupport(bool *isPreferredSupport) { m_isPreferredSupport = isPreferredSupport; }
	bool *getIsPreferredSupport() { return m_isPreferredSupport; }
	void setDisabledSupport(bool *disabledSupport) { m_disabledSupport = disabledSupport; }
	bool *getDisabledSupport() { return m_disabledSupport; }
	void setDataSupport(bool *dataSupport) { m_dataSupport = dataSupport; }
	bool *getDataSupport() { return m_dataSupport; }
	void setResolveSupport(CodeActionClientCapabilitiesResolveSupport *resolveSupport) { m_resolveSupport = resolveSupport; }
	CodeActionClientCapabilitiesResolveSupport *getResolveSupport() { return m_resolveSupport; }
	void setHonorsChangeAnnotations(bool *honorsChangeAnnotations) { m_honorsChangeAnnotations = honorsChangeAnnotations; }
	bool *getHonorsChangeAnnotations() { return m_honorsChangeAnnotations; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_codeActionLiteralSupport) {
			QJsonObject codeActionLiteralSupport = m_codeActionLiteralSupport->toJson();
			obj.insert("codeActionLiteralSupport", codeActionLiteralSupport);
		}
		if(m_isPreferredSupport) {
			QJsonValue isPreferredSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_isPreferredSupport));
			obj.insert("isPreferredSupport", isPreferredSupport);
		}
		if(m_disabledSupport) {
			QJsonValue disabledSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_disabledSupport));
			obj.insert("disabledSupport", disabledSupport);
		}
		if(m_dataSupport) {
			QJsonValue dataSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_dataSupport));
			obj.insert("dataSupport", dataSupport);
		}
		if(m_resolveSupport) {
			QJsonObject resolveSupport = m_resolveSupport->toJson();
			obj.insert("resolveSupport", resolveSupport);
		}
		if(m_honorsChangeAnnotations) {
			QJsonValue honorsChangeAnnotations = QJsonValue::fromVariant(QVariant::fromValue(*m_honorsChangeAnnotations));
			obj.insert("honorsChangeAnnotations", honorsChangeAnnotations);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("codeActionLiteralSupport")) {
			auto codeActionLiteralSupport = new CodeActionClientCapabilitiesLiteralSupport();
			codeActionLiteralSupport->fromJson(json.value("codeActionLiteralSupport").toObject());
			m_codeActionLiteralSupport = codeActionLiteralSupport;
		}
		if(json.contains("isPreferredSupport")) {
			auto isPreferredSupport = new bool(json.value("isPreferredSupport").toVariant().value<bool>());
			m_isPreferredSupport = isPreferredSupport;
		}
		if(json.contains("disabledSupport")) {
			auto disabledSupport = new bool(json.value("disabledSupport").toVariant().value<bool>());
			m_disabledSupport = disabledSupport;
		}
		if(json.contains("dataSupport")) {
			auto dataSupport = new bool(json.value("dataSupport").toVariant().value<bool>());
			m_dataSupport = dataSupport;
		}
		if(json.contains("resolveSupport")) {
			auto resolveSupport = new CodeActionClientCapabilitiesResolveSupport();
			resolveSupport->fromJson(json.value("resolveSupport").toObject());
			m_resolveSupport = resolveSupport;
		}
		if(json.contains("honorsChangeAnnotations")) {
			auto honorsChangeAnnotations = new bool(json.value("honorsChangeAnnotations").toVariant().value<bool>());
			m_honorsChangeAnnotations = honorsChangeAnnotations;
		}

	}
};
typedef CodeActionClientCapabilities TextDocumentClientCapabilitiesCodeAction;
const QString MethodTextDocumentDocumentSymbol = "textDocument/documentSymbol";
const QString LuaLanguage = "lua";
const QString Perl6Language = "perl6";
class SelectionRangeOptions:  public WorkDoneProgressOptions {

public:
	SelectionRangeOptions() = default;
	virtual ~SelectionRangeOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
class DocumentFilter: virtual public GoPlsParams {
	QString *m_language = nullptr;
	QString *m_scheme = nullptr;
	QString *m_pattern = nullptr;

public:
	DocumentFilter() = default;
	virtual ~DocumentFilter() {
		if(m_language) { delete m_language; }
		if(m_scheme) { delete m_scheme; }
		if(m_pattern) { delete m_pattern; }
	}

	void setLanguage(QString *language) { m_language = language; }
	QString *getLanguage() { return m_language; }
	void setScheme(QString *scheme) { m_scheme = scheme; }
	QString *getScheme() { return m_scheme; }
	void setPattern(QString *pattern) { m_pattern = pattern; }
	QString *getPattern() { return m_pattern; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_language) {
			QJsonValue language = QJsonValue::fromVariant(QVariant::fromValue(*m_language));
			obj.insert("language", language);
		}
		if(m_scheme) {
			QJsonValue scheme = QJsonValue::fromVariant(QVariant::fromValue(*m_scheme));
			obj.insert("scheme", scheme);
		}
		if(m_pattern) {
			QJsonValue pattern = QJsonValue::fromVariant(QVariant::fromValue(*m_pattern));
			obj.insert("pattern", pattern);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("language")) {
			auto language = new QString(json.value("language").toString());
			m_language = language;
		}
		if(json.contains("scheme")) {
			auto scheme = new QString(json.value("scheme").toString());
			m_scheme = scheme;
		}
		if(json.contains("pattern")) {
			auto pattern = new QString(json.value("pattern").toString());
			m_pattern = pattern;
		}

	}
};
typedef QList<DocumentFilter*> DocumentSelector;
class TextDocumentRegistrationOptions: virtual public GoPlsParams {
	DocumentSelector *m_documentSelector = nullptr;

public:
	TextDocumentRegistrationOptions() = default;
	virtual ~TextDocumentRegistrationOptions() {
		if(m_documentSelector) { delete m_documentSelector; }
	}

	void setDocumentSelector(DocumentSelector *documentSelector) { m_documentSelector = documentSelector; }
	DocumentSelector *getDocumentSelector() { return m_documentSelector; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_documentSelector) {
				QJsonArray documentSelector;
			for(auto it = m_documentSelector->cbegin(); it != m_documentSelector->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				documentSelector.append(item);
			}

			obj.insert("documentSelector", documentSelector);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("documentSelector")) {
			 QJsonArray arr = json.value("documentSelector").toArray();
			auto documentSelector = new QList<DocumentFilter*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new DocumentFilter();
			item->fromJson((m_item).toObject());
				documentSelector->append(item);
			}
			m_documentSelector = documentSelector;
		}

	}
};
class SelectionRangeRegistrationOptions:  public SelectionRangeOptions, public TextDocumentRegistrationOptions, public StaticRegistrationOptions {

public:
	SelectionRangeRegistrationOptions() = default;
	virtual ~SelectionRangeRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject SelectionRangeOptions_json = SelectionRangeOptions::toJson();
		for(const QString &key: SelectionRangeOptions_json.keys()) {
			obj.insert(key, SelectionRangeOptions_json.value(key));
		}
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject StaticRegistrationOptions_json = StaticRegistrationOptions::toJson();
		for(const QString &key: StaticRegistrationOptions_json.keys()) {
			obj.insert(key, StaticRegistrationOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		SelectionRangeOptions::fromJson(json);
		TextDocumentRegistrationOptions::fromJson(json);
		StaticRegistrationOptions::fromJson(json);

	}
};
class WorkspaceFolder: virtual public GoPlsParams {
	QString *m_uri = nullptr;
	QString *m_name = nullptr;

public:
	WorkspaceFolder() = default;
	virtual ~WorkspaceFolder() {
		if(m_uri) { delete m_uri; }
		if(m_name) { delete m_name; }
	}

	void setUri(QString *uri) { m_uri = uri; }
	QString *getUri() { return m_uri; }
	void setName(QString *name) { m_name = name; }
	QString *getName() { return m_name; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}
		if(m_name) {
			QJsonValue name = QJsonValue::fromVariant(QVariant::fromValue(*m_name));
			obj.insert("name", name);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}
		if(json.contains("name")) {
			auto name = new QString(json.value("name").toString());
			m_name = name;
		}

	}
};
typedef QList<WorkspaceFolder*> WorkspaceFolders;
typedef QJsonValue SelectionRangeProviderOptions;
const QString SemanticTokenInterface = "interface";
const QString ObjectiveCLanguage = "objective-c";
const QString YamlLanguage = "yaml";
class SemanticTokensParams:  public WorkDoneProgressParams, public PartialResultParams {
	TextDocumentIdentifier *m_textDocument = nullptr;

public:
	SemanticTokensParams() = default;
	virtual ~SemanticTokensParams() {
		if(m_textDocument) { delete m_textDocument; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}

	}
};
const QString CsharpLanguage = "csharp";
const QString MonikerKindLocal = "local";
const int CompletionItemKindEnum = 13;
class FoldingRangeParams:  public TextDocumentPositionParams, public PartialResultParams {

public:
	FoldingRangeParams() = default;
	virtual ~FoldingRangeParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		PartialResultParams::fromJson(json);

	}
};
const int SymbolKindField = 8;
typedef double InsertTextFormat;
const int CompletionItemKindFunction = 3;
class CodeActionOptions: virtual public GoPlsParams {
	QList<CodeActionKind*> *m_codeActionKinds = nullptr;
	bool *m_resolveProvider = nullptr;

public:
	CodeActionOptions() = default;
	virtual ~CodeActionOptions() {
		if(m_codeActionKinds) { qDeleteAll(*m_codeActionKinds); delete m_codeActionKinds; }
		if(m_resolveProvider) { delete m_resolveProvider; }
	}

	void setCodeActionKinds(QList<CodeActionKind*> *codeActionKinds) { m_codeActionKinds = codeActionKinds; }
	QList<CodeActionKind*> *getCodeActionKinds() { return m_codeActionKinds; }
	void setResolveProvider(bool *resolveProvider) { m_resolveProvider = resolveProvider; }
	bool *getResolveProvider() { return m_resolveProvider; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_codeActionKinds) {
				QJsonArray codeActionKinds;
			for(auto it = m_codeActionKinds->cbegin(); it != m_codeActionKinds->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				codeActionKinds.append(item);
			}

			obj.insert("codeActionKinds", codeActionKinds);
		}
		if(m_resolveProvider) {
			QJsonValue resolveProvider = QJsonValue::fromVariant(QVariant::fromValue(*m_resolveProvider));
			obj.insert("resolveProvider", resolveProvider);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("codeActionKinds")) {
			 QJsonArray arr = json.value("codeActionKinds").toArray();
			auto codeActionKinds = new QList<CodeActionKind*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				codeActionKinds->append(item);
			}
			m_codeActionKinds = codeActionKinds;
		}
		if(json.contains("resolveProvider")) {
			auto resolveProvider = new bool(json.value("resolveProvider").toVariant().value<bool>());
			m_resolveProvider = resolveProvider;
		}

	}
};
class CodeActionRegistrationOptions:  public TextDocumentRegistrationOptions, public CodeActionOptions {

public:
	CodeActionRegistrationOptions() = default;
	virtual ~CodeActionRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject CodeActionOptions_json = CodeActionOptions::toJson();
		for(const QString &key: CodeActionOptions_json.keys()) {
			obj.insert(key, CodeActionOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		CodeActionOptions::fromJson(json);

	}
};
class CallHierarchyClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	CallHierarchyClientCapabilities() = default;
	virtual ~CallHierarchyClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
typedef QHash<QString,QJsonValue*> StringInterfaceMap;
class MessageActionItem: virtual public GoPlsParams {
	QString *m_title = nullptr;

public:
	MessageActionItem() = default;
	virtual ~MessageActionItem() {
		if(m_title) { delete m_title; }
	}

	void setTitle(QString *title) { m_title = title; }
	QString *getTitle() { return m_title; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_title) {
			QJsonValue title = QJsonValue::fromVariant(QVariant::fromValue(*m_title));
			obj.insert("title", title);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("title")) {
			auto title = new QString(json.value("title").toString());
			m_title = title;
		}

	}
};
typedef double MessageType;
class ShowMessageRequestParams: virtual public GoPlsParams {
	QList<MessageActionItem*> *m_actions = nullptr;
	QString *m_message = nullptr;
	MessageType *m_type = nullptr;

public:
	ShowMessageRequestParams() = default;
	virtual ~ShowMessageRequestParams() {
		if(m_actions) { qDeleteAll(*m_actions); delete m_actions; }
		if(m_message) { delete m_message; }
		if(m_type) { delete m_type; }
	}

	void setActions(QList<MessageActionItem*> *actions) { m_actions = actions; }
	QList<MessageActionItem*> *getActions() { return m_actions; }
	void setMessage(QString *message) { m_message = message; }
	QString *getMessage() { return m_message; }
	void setType(MessageType *type) { m_type = type; }
	MessageType *getType() { return m_type; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_actions) {
				QJsonArray actions;
			for(auto it = m_actions->cbegin(); it != m_actions->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				actions.append(item);
			}

			obj.insert("actions", actions);
		}
		if(m_message) {
			QJsonValue message = QJsonValue::fromVariant(QVariant::fromValue(*m_message));
			obj.insert("message", message);
		}
		if(m_type) {
			QJsonValue type = QJsonValue::fromVariant(QVariant::fromValue(*m_type));
			obj.insert("type", type);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("actions")) {
			 QJsonArray arr = json.value("actions").toArray();
			auto actions = new QList<MessageActionItem*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new MessageActionItem();
			item->fromJson((m_item).toObject());
				actions->append(item);
			}
			m_actions = actions;
		}
		if(json.contains("message")) {
			auto message = new QString(json.value("message").toString());
			m_message = message;
		}
		if(json.contains("type")) {
			auto type = new double(json.value("type").toVariant().value<double>());
			m_type = type;
		}

	}
};
class SignatureHelpOptions: virtual public GoPlsParams {
	QList<QString*> *m_triggerCharacters = nullptr;
	QList<QString*> *m_retriggerCharacters = nullptr;

public:
	SignatureHelpOptions() = default;
	virtual ~SignatureHelpOptions() {
		if(m_triggerCharacters) { qDeleteAll(*m_triggerCharacters); delete m_triggerCharacters; }
		if(m_retriggerCharacters) { qDeleteAll(*m_retriggerCharacters); delete m_retriggerCharacters; }
	}

	void setTriggerCharacters(QList<QString*> *triggerCharacters) { m_triggerCharacters = triggerCharacters; }
	QList<QString*> *getTriggerCharacters() { return m_triggerCharacters; }
	void setRetriggerCharacters(QList<QString*> *retriggerCharacters) { m_retriggerCharacters = retriggerCharacters; }
	QList<QString*> *getRetriggerCharacters() { return m_retriggerCharacters; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_triggerCharacters) {
				QJsonArray triggerCharacters;
			for(auto it = m_triggerCharacters->cbegin(); it != m_triggerCharacters->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				triggerCharacters.append(item);
			}

			obj.insert("triggerCharacters", triggerCharacters);
		}
		if(m_retriggerCharacters) {
				QJsonArray retriggerCharacters;
			for(auto it = m_retriggerCharacters->cbegin(); it != m_retriggerCharacters->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				retriggerCharacters.append(item);
			}

			obj.insert("retriggerCharacters", retriggerCharacters);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("triggerCharacters")) {
			 QJsonArray arr = json.value("triggerCharacters").toArray();
			auto triggerCharacters = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				triggerCharacters->append(item);
			}
			m_triggerCharacters = triggerCharacters;
		}
		if(json.contains("retriggerCharacters")) {
			 QJsonArray arr = json.value("retriggerCharacters").toArray();
			auto retriggerCharacters = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				retriggerCharacters->append(item);
			}
			m_retriggerCharacters = retriggerCharacters;
		}

	}
};
class WorkspaceClientCapabilitiesWorkspaceEditChangeAnnotationSupport: virtual public GoPlsParams {
	bool *m_groupsOnLabel = nullptr;

public:
	WorkspaceClientCapabilitiesWorkspaceEditChangeAnnotationSupport() = default;
	virtual ~WorkspaceClientCapabilitiesWorkspaceEditChangeAnnotationSupport() {
		if(m_groupsOnLabel) { delete m_groupsOnLabel; }
	}

	void setGroupsOnLabel(bool *groupsOnLabel) { m_groupsOnLabel = groupsOnLabel; }
	bool *getGroupsOnLabel() { return m_groupsOnLabel; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_groupsOnLabel) {
			QJsonValue groupsOnLabel = QJsonValue::fromVariant(QVariant::fromValue(*m_groupsOnLabel));
			obj.insert("groupsOnLabel", groupsOnLabel);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("groupsOnLabel")) {
			auto groupsOnLabel = new bool(json.value("groupsOnLabel").toVariant().value<bool>());
			m_groupsOnLabel = groupsOnLabel;
		}

	}
};
const QString RefactorInline = "refactor.inline";
const int CompletionItemKindReference = 18;
const int TextDocumentSaveReasonFocusOut = 3;
const QString LessLanguage = "less";
const QString SemanticTokenProperty = "property";
class CallHierarchyIncomingCall: virtual public GoPlsParams {
	CallHierarchyItem *m_from = nullptr;
	QList<Range*> *m_fromRanges = nullptr;

public:
	CallHierarchyIncomingCall() = default;
	virtual ~CallHierarchyIncomingCall() {
		if(m_from) { delete m_from; }
		if(m_fromRanges) { qDeleteAll(*m_fromRanges); delete m_fromRanges; }
	}

	void setFrom(CallHierarchyItem *from) { m_from = from; }
	CallHierarchyItem *getFrom() { return m_from; }
	void setFromRanges(QList<Range*> *fromRanges) { m_fromRanges = fromRanges; }
	QList<Range*> *getFromRanges() { return m_fromRanges; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_from) {
			QJsonObject from = m_from->toJson();
			obj.insert("from", from);
		}
		if(m_fromRanges) {
				QJsonArray fromRanges;
			for(auto it = m_fromRanges->cbegin(); it != m_fromRanges->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				fromRanges.append(item);
			}

			obj.insert("fromRanges", fromRanges);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("from")) {
			auto from = new CallHierarchyItem();
			from->fromJson(json.value("from").toObject());
			m_from = from;
		}
		if(json.contains("fromRanges")) {
			 QJsonArray arr = json.value("fromRanges").toArray();
			auto fromRanges = new QList<Range*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new Range();
			item->fromJson((m_item).toObject());
				fromRanges->append(item);
			}
			m_fromRanges = fromRanges;
		}

	}
};
typedef double DiagnosticSeverity;
typedef QString URI;
class CodeDescription: virtual public GoPlsParams {
	URI *m_href = nullptr;

public:
	CodeDescription() = default;
	virtual ~CodeDescription() {
		if(m_href) { delete m_href; }
	}

	void setHref(URI *href) { m_href = href; }
	URI *getHref() { return m_href; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_href) {
			QJsonValue href = QJsonValue::fromVariant(QVariant::fromValue(*m_href));
			obj.insert("href", href);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("href")) {
			auto href = new QString(json.value("href").toString());
			m_href = href;
		}

	}
};
class Location: virtual public GoPlsParams {
	DocumentURI *m_uri = nullptr;
	Range *m_range = nullptr;

public:
	Location() = default;
	virtual ~Location() {
		if(m_uri) { delete m_uri; }
		if(m_range) { delete m_range; }
	}

	void setUri(DocumentURI *uri) { m_uri = uri; }
	DocumentURI *getUri() { return m_uri; }
	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}

	}
};
class DiagnosticRelatedInformation: virtual public GoPlsParams {
	Location *m_location = nullptr;
	QString *m_message = nullptr;

public:
	DiagnosticRelatedInformation() = default;
	virtual ~DiagnosticRelatedInformation() {
		if(m_location) { delete m_location; }
		if(m_message) { delete m_message; }
	}

	void setLocation(Location *location) { m_location = location; }
	Location *getLocation() { return m_location; }
	void setMessage(QString *message) { m_message = message; }
	QString *getMessage() { return m_message; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_location) {
			QJsonObject location = m_location->toJson();
			obj.insert("location", location);
		}
		if(m_message) {
			QJsonValue message = QJsonValue::fromVariant(QVariant::fromValue(*m_message));
			obj.insert("message", message);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("location")) {
			auto location = new Location();
			location->fromJson(json.value("location").toObject());
			m_location = location;
		}
		if(json.contains("message")) {
			auto message = new QString(json.value("message").toString());
			m_message = message;
		}

	}
};
class Diagnostic: virtual public GoPlsParams {
	Range *m_range = nullptr;
	DiagnosticSeverity *m_severity = nullptr;
	QJsonValue *m_code = nullptr;
	CodeDescription *m_codeDescription = nullptr;
	QString *m_source = nullptr;
	QString *m_message = nullptr;
	QList<DiagnosticTag*> *m_tags = nullptr;
	QList<DiagnosticRelatedInformation*> *m_relatedInformation = nullptr;
	QJsonValue *m_data = nullptr;

public:
	Diagnostic() = default;
	virtual ~Diagnostic() {
		if(m_range) { delete m_range; }
		if(m_severity) { delete m_severity; }
		if(m_code) { delete m_code; }
		if(m_codeDescription) { delete m_codeDescription; }
		if(m_source) { delete m_source; }
		if(m_message) { delete m_message; }
		if(m_tags) { qDeleteAll(*m_tags); delete m_tags; }
		if(m_relatedInformation) { qDeleteAll(*m_relatedInformation); delete m_relatedInformation; }
		if(m_data) { delete m_data; }
	}

	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setSeverity(DiagnosticSeverity *severity) { m_severity = severity; }
	DiagnosticSeverity *getSeverity() { return m_severity; }
	void setCode(QJsonValue *code) { m_code = code; }
	QJsonValue *getCode() { return m_code; }
	void setCodeDescription(CodeDescription *codeDescription) { m_codeDescription = codeDescription; }
	CodeDescription *getCodeDescription() { return m_codeDescription; }
	void setSource(QString *source) { m_source = source; }
	QString *getSource() { return m_source; }
	void setMessage(QString *message) { m_message = message; }
	QString *getMessage() { return m_message; }
	void setTags(QList<DiagnosticTag*> *tags) { m_tags = tags; }
	QList<DiagnosticTag*> *getTags() { return m_tags; }
	void setRelatedInformation(QList<DiagnosticRelatedInformation*> *relatedInformation) { m_relatedInformation = relatedInformation; }
	QList<DiagnosticRelatedInformation*> *getRelatedInformation() { return m_relatedInformation; }
	void setData(QJsonValue *data) { m_data = data; }
	QJsonValue *getData() { return m_data; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_severity) {
			QJsonValue severity = QJsonValue::fromVariant(QVariant::fromValue(*m_severity));
			obj.insert("severity", severity);
		}
		if(m_code) {
			QJsonValue code = *m_code;
			obj.insert("code", code);
		}
		if(m_codeDescription) {
			QJsonObject codeDescription = m_codeDescription->toJson();
			obj.insert("codeDescription", codeDescription);
		}
		if(m_source) {
			QJsonValue source = QJsonValue::fromVariant(QVariant::fromValue(*m_source));
			obj.insert("source", source);
		}
		if(m_message) {
			QJsonValue message = QJsonValue::fromVariant(QVariant::fromValue(*m_message));
			obj.insert("message", message);
		}
		if(m_tags) {
				QJsonArray tags;
			for(auto it = m_tags->cbegin(); it != m_tags->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				tags.append(item);
			}

			obj.insert("tags", tags);
		}
		if(m_relatedInformation) {
				QJsonArray relatedInformation;
			for(auto it = m_relatedInformation->cbegin(); it != m_relatedInformation->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				relatedInformation.append(item);
			}

			obj.insert("relatedInformation", relatedInformation);
		}
		if(m_data) {
			QJsonValue data = *m_data;
			obj.insert("data", data);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("severity")) {
			auto severity = new double(json.value("severity").toVariant().value<double>());
			m_severity = severity;
		}
		if(json.contains("code")) {
			auto code = new QJsonValue(json.value("code"));
			m_code = code;
		}
		if(json.contains("codeDescription")) {
			auto codeDescription = new CodeDescription();
			codeDescription->fromJson(json.value("codeDescription").toObject());
			m_codeDescription = codeDescription;
		}
		if(json.contains("source")) {
			auto source = new QString(json.value("source").toString());
			m_source = source;
		}
		if(json.contains("message")) {
			auto message = new QString(json.value("message").toString());
			m_message = message;
		}
		if(json.contains("tags")) {
			 QJsonArray arr = json.value("tags").toArray();
			auto tags = new QList<DiagnosticTag*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				tags->append(item);
			}
			m_tags = tags;
		}
		if(json.contains("relatedInformation")) {
			 QJsonArray arr = json.value("relatedInformation").toArray();
			auto relatedInformation = new QList<DiagnosticRelatedInformation*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new DiagnosticRelatedInformation();
			item->fromJson((m_item).toObject());
				relatedInformation->append(item);
			}
			m_relatedInformation = relatedInformation;
		}
		if(json.contains("data")) {
			auto data = new QJsonValue(json.value("data"));
			m_data = data;
		}

	}
};
class CodeActionContext: virtual public GoPlsParams {
	QList<Diagnostic*> *m_diagnostics = nullptr;
	QList<CodeActionKind*> *m_only = nullptr;

public:
	CodeActionContext() = default;
	virtual ~CodeActionContext() {
		if(m_diagnostics) { qDeleteAll(*m_diagnostics); delete m_diagnostics; }
		if(m_only) { qDeleteAll(*m_only); delete m_only; }
	}

	void setDiagnostics(QList<Diagnostic*> *diagnostics) { m_diagnostics = diagnostics; }
	QList<Diagnostic*> *getDiagnostics() { return m_diagnostics; }
	void setOnly(QList<CodeActionKind*> *only) { m_only = only; }
	QList<CodeActionKind*> *getOnly() { return m_only; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_diagnostics) {
				QJsonArray diagnostics;
			for(auto it = m_diagnostics->cbegin(); it != m_diagnostics->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				diagnostics.append(item);
			}

			obj.insert("diagnostics", diagnostics);
		}
		if(m_only) {
				QJsonArray only;
			for(auto it = m_only->cbegin(); it != m_only->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				only.append(item);
			}

			obj.insert("only", only);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("diagnostics")) {
			 QJsonArray arr = json.value("diagnostics").toArray();
			auto diagnostics = new QList<Diagnostic*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new Diagnostic();
			item->fromJson((m_item).toObject());
				diagnostics->append(item);
			}
			m_diagnostics = diagnostics;
		}
		if(json.contains("only")) {
			 QJsonArray arr = json.value("only").toArray();
			auto only = new QList<CodeActionKind*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				only->append(item);
			}
			m_only = only;
		}

	}
};
class CodeActionParams:  public WorkDoneProgressParams, public PartialResultParams {
	TextDocumentIdentifier *m_textDocument = nullptr;
	CodeActionContext *m_context = nullptr;
	Range *m_range = nullptr;

public:
	CodeActionParams() = default;
	virtual ~CodeActionParams() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_context) { delete m_context; }
		if(m_range) { delete m_range; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setContext(CodeActionContext *context) { m_context = context; }
	CodeActionContext *getContext() { return m_context; }
	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_context) {
			QJsonObject context = m_context->toJson();
			obj.insert("context", context);
		}
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("context")) {
			auto context = new CodeActionContext();
			context->fromJson(json.value("context").toObject());
			m_context = context;
		}
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}

	}
};
class MonikerClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	MonikerClientCapabilities() = default;
	virtual ~MonikerClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
typedef MonikerClientCapabilities TextDocumentClientCapabilitiesMoniker;
const QString MethodTextDocumentOnTypeFormatting = "textDocument/onTypeFormatting";
const int FileChangeTypeDeleted = 3;
class CallHierarchyIncomingCallsParams:  public WorkDoneProgressParams, public PartialResultParams {
	CallHierarchyItem *m_item = nullptr;

public:
	CallHierarchyIncomingCallsParams() = default;
	virtual ~CallHierarchyIncomingCallsParams() {
		if(m_item) { delete m_item; }
	}

	void setItem(CallHierarchyItem *item) { m_item = item; }
	CallHierarchyItem *getItem() { return m_item; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_item) {
			QJsonObject item = m_item->toJson();
			obj.insert("item", item);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("item")) {
			auto item = new CallHierarchyItem();
			item->fromJson(json.value("item").toObject());
			m_item = item;
		}

	}
};
const int DocumentHighlightKindRead = 2;
const int SymbolKindFunction = 12;
const int CompletionItemKindModule = 9;
const int FileChangeTypeChanged = 2;
class FoldingRangeClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	unsigned int *m_rangeLimit = nullptr;
	bool *m_lineFoldingOnly = nullptr;

public:
	FoldingRangeClientCapabilities() = default;
	virtual ~FoldingRangeClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_rangeLimit) { delete m_rangeLimit; }
		if(m_lineFoldingOnly) { delete m_lineFoldingOnly; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setRangeLimit(unsigned int *rangeLimit) { m_rangeLimit = rangeLimit; }
	unsigned int *getRangeLimit() { return m_rangeLimit; }
	void setLineFoldingOnly(bool *lineFoldingOnly) { m_lineFoldingOnly = lineFoldingOnly; }
	bool *getLineFoldingOnly() { return m_lineFoldingOnly; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_rangeLimit) {
			QJsonValue rangeLimit = QJsonValue::fromVariant(QVariant::fromValue(*m_rangeLimit));
			obj.insert("rangeLimit", rangeLimit);
		}
		if(m_lineFoldingOnly) {
			QJsonValue lineFoldingOnly = QJsonValue::fromVariant(QVariant::fromValue(*m_lineFoldingOnly));
			obj.insert("lineFoldingOnly", lineFoldingOnly);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("rangeLimit")) {
			auto rangeLimit = new unsigned int(json.value("rangeLimit").toVariant().value<unsigned int>());
			m_rangeLimit = rangeLimit;
		}
		if(json.contains("lineFoldingOnly")) {
			auto lineFoldingOnly = new bool(json.value("lineFoldingOnly").toVariant().value<bool>());
			m_lineFoldingOnly = lineFoldingOnly;
		}

	}
};
const QString MethodTextDocumentWillSaveWaitUntil = "textDocument/willSaveWaitUntil";
const QString GitRebaseLanguage = "git-rebase";
class CallHierarchyPrepareParams:  public TextDocumentPositionParams, public WorkDoneProgressParams {

public:
	CallHierarchyPrepareParams() = default;
	virtual ~CallHierarchyPrepareParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);

	}
};
class ClientInfo: virtual public GoPlsParams {
	QString *m_name = nullptr;
	QString *m_version = nullptr;

public:
	ClientInfo() = default;
	virtual ~ClientInfo() {
		if(m_name) { delete m_name; }
		if(m_version) { delete m_version; }
	}

	void setName(QString *name) { m_name = name; }
	QString *getName() { return m_name; }
	void setVersion(QString *version) { m_version = version; }
	QString *getVersion() { return m_version; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_name) {
			QJsonValue name = QJsonValue::fromVariant(QVariant::fromValue(*m_name));
			obj.insert("name", name);
		}
		if(m_version) {
			QJsonValue version = QJsonValue::fromVariant(QVariant::fromValue(*m_version));
			obj.insert("version", version);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("name")) {
			auto name = new QString(json.value("name").toString());
			m_name = name;
		}
		if(json.contains("version")) {
			auto version = new QString(json.value("version").toString());
			m_version = version;
		}

	}
};
class WorkspaceClientCapabilitiesWorkspaceEdit: virtual public GoPlsParams {
	bool *m_documentChanges = nullptr;
	QString *m_failureHandling = nullptr;
	QList<QString*> *m_resourceOperations = nullptr;
	bool *m_normalizesLineEndings = nullptr;
	WorkspaceClientCapabilitiesWorkspaceEditChangeAnnotationSupport *m_changeAnnotationSupport = nullptr;

public:
	WorkspaceClientCapabilitiesWorkspaceEdit() = default;
	virtual ~WorkspaceClientCapabilitiesWorkspaceEdit() {
		if(m_documentChanges) { delete m_documentChanges; }
		if(m_failureHandling) { delete m_failureHandling; }
		if(m_resourceOperations) { qDeleteAll(*m_resourceOperations); delete m_resourceOperations; }
		if(m_normalizesLineEndings) { delete m_normalizesLineEndings; }
		if(m_changeAnnotationSupport) { delete m_changeAnnotationSupport; }
	}

	void setDocumentChanges(bool *documentChanges) { m_documentChanges = documentChanges; }
	bool *getDocumentChanges() { return m_documentChanges; }
	void setFailureHandling(QString *failureHandling) { m_failureHandling = failureHandling; }
	QString *getFailureHandling() { return m_failureHandling; }
	void setResourceOperations(QList<QString*> *resourceOperations) { m_resourceOperations = resourceOperations; }
	QList<QString*> *getResourceOperations() { return m_resourceOperations; }
	void setNormalizesLineEndings(bool *normalizesLineEndings) { m_normalizesLineEndings = normalizesLineEndings; }
	bool *getNormalizesLineEndings() { return m_normalizesLineEndings; }
	void setChangeAnnotationSupport(WorkspaceClientCapabilitiesWorkspaceEditChangeAnnotationSupport *changeAnnotationSupport) { m_changeAnnotationSupport = changeAnnotationSupport; }
	WorkspaceClientCapabilitiesWorkspaceEditChangeAnnotationSupport *getChangeAnnotationSupport() { return m_changeAnnotationSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_documentChanges) {
			QJsonValue documentChanges = QJsonValue::fromVariant(QVariant::fromValue(*m_documentChanges));
			obj.insert("documentChanges", documentChanges);
		}
		if(m_failureHandling) {
			QJsonValue failureHandling = QJsonValue::fromVariant(QVariant::fromValue(*m_failureHandling));
			obj.insert("failureHandling", failureHandling);
		}
		if(m_resourceOperations) {
				QJsonArray resourceOperations;
			for(auto it = m_resourceOperations->cbegin(); it != m_resourceOperations->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				resourceOperations.append(item);
			}

			obj.insert("resourceOperations", resourceOperations);
		}
		if(m_normalizesLineEndings) {
			QJsonValue normalizesLineEndings = QJsonValue::fromVariant(QVariant::fromValue(*m_normalizesLineEndings));
			obj.insert("normalizesLineEndings", normalizesLineEndings);
		}
		if(m_changeAnnotationSupport) {
			QJsonObject changeAnnotationSupport = m_changeAnnotationSupport->toJson();
			obj.insert("changeAnnotationSupport", changeAnnotationSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("documentChanges")) {
			auto documentChanges = new bool(json.value("documentChanges").toVariant().value<bool>());
			m_documentChanges = documentChanges;
		}
		if(json.contains("failureHandling")) {
			auto failureHandling = new QString(json.value("failureHandling").toString());
			m_failureHandling = failureHandling;
		}
		if(json.contains("resourceOperations")) {
			 QJsonArray arr = json.value("resourceOperations").toArray();
			auto resourceOperations = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				resourceOperations->append(item);
			}
			m_resourceOperations = resourceOperations;
		}
		if(json.contains("normalizesLineEndings")) {
			auto normalizesLineEndings = new bool(json.value("normalizesLineEndings").toVariant().value<bool>());
			m_normalizesLineEndings = normalizesLineEndings;
		}
		if(json.contains("changeAnnotationSupport")) {
			auto changeAnnotationSupport = new WorkspaceClientCapabilitiesWorkspaceEditChangeAnnotationSupport();
			changeAnnotationSupport->fromJson(json.value("changeAnnotationSupport").toObject());
			m_changeAnnotationSupport = changeAnnotationSupport;
		}

	}
};
class DidChangeWatchedFilesWorkspaceClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	DidChangeWatchedFilesWorkspaceClientCapabilities() = default;
	virtual ~DidChangeWatchedFilesWorkspaceClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class TagSupportCapabilities: virtual public GoPlsParams {
	QList<SymbolTag*> *m_valueSet = nullptr;

public:
	TagSupportCapabilities() = default;
	virtual ~TagSupportCapabilities() {
		if(m_valueSet) { qDeleteAll(*m_valueSet); delete m_valueSet; }
	}

	void setValueSet(QList<SymbolTag*> *valueSet) { m_valueSet = valueSet; }
	QList<SymbolTag*> *getValueSet() { return m_valueSet; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_valueSet) {
				QJsonArray valueSet;
			for(auto it = m_valueSet->cbegin(); it != m_valueSet->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				valueSet.append(item);
			}

			obj.insert("valueSet", valueSet);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("valueSet")) {
			 QJsonArray arr = json.value("valueSet").toArray();
			auto valueSet = new QList<SymbolTag*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				valueSet->append(item);
			}
			m_valueSet = valueSet;
		}

	}
};
class WorkspaceSymbolClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	SymbolKindCapabilities *m_symbolKind = nullptr;
	TagSupportCapabilities *m_tagSupport = nullptr;

public:
	WorkspaceSymbolClientCapabilities() = default;
	virtual ~WorkspaceSymbolClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_symbolKind) { delete m_symbolKind; }
		if(m_tagSupport) { delete m_tagSupport; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setSymbolKind(SymbolKindCapabilities *symbolKind) { m_symbolKind = symbolKind; }
	SymbolKindCapabilities *getSymbolKind() { return m_symbolKind; }
	void setTagSupport(TagSupportCapabilities *tagSupport) { m_tagSupport = tagSupport; }
	TagSupportCapabilities *getTagSupport() { return m_tagSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_symbolKind) {
			QJsonObject symbolKind = m_symbolKind->toJson();
			obj.insert("symbolKind", symbolKind);
		}
		if(m_tagSupport) {
			QJsonObject tagSupport = m_tagSupport->toJson();
			obj.insert("tagSupport", tagSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("symbolKind")) {
			auto symbolKind = new SymbolKindCapabilities();
			symbolKind->fromJson(json.value("symbolKind").toObject());
			m_symbolKind = symbolKind;
		}
		if(json.contains("tagSupport")) {
			auto tagSupport = new TagSupportCapabilities();
			tagSupport->fromJson(json.value("tagSupport").toObject());
			m_tagSupport = tagSupport;
		}

	}
};
class ExecuteCommandClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	ExecuteCommandClientCapabilities() = default;
	virtual ~ExecuteCommandClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class SemanticTokensWorkspaceClientCapabilities: virtual public GoPlsParams {
	bool *m_refreshSupport = nullptr;

public:
	SemanticTokensWorkspaceClientCapabilities() = default;
	virtual ~SemanticTokensWorkspaceClientCapabilities() {
		if(m_refreshSupport) { delete m_refreshSupport; }
	}

	void setRefreshSupport(bool *refreshSupport) { m_refreshSupport = refreshSupport; }
	bool *getRefreshSupport() { return m_refreshSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_refreshSupport) {
			QJsonValue refreshSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_refreshSupport));
			obj.insert("refreshSupport", refreshSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("refreshSupport")) {
			auto refreshSupport = new bool(json.value("refreshSupport").toVariant().value<bool>());
			m_refreshSupport = refreshSupport;
		}

	}
};
class CodeLensWorkspaceClientCapabilities: virtual public GoPlsParams {
	bool *m_refreshSupport = nullptr;

public:
	CodeLensWorkspaceClientCapabilities() = default;
	virtual ~CodeLensWorkspaceClientCapabilities() {
		if(m_refreshSupport) { delete m_refreshSupport; }
	}

	void setRefreshSupport(bool *refreshSupport) { m_refreshSupport = refreshSupport; }
	bool *getRefreshSupport() { return m_refreshSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_refreshSupport) {
			QJsonValue refreshSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_refreshSupport));
			obj.insert("refreshSupport", refreshSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("refreshSupport")) {
			auto refreshSupport = new bool(json.value("refreshSupport").toVariant().value<bool>());
			m_refreshSupport = refreshSupport;
		}

	}
};
class WorkspaceClientCapabilitiesFileOperations: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	bool *m_didCreate = nullptr;
	bool *m_willCreate = nullptr;
	bool *m_didRename = nullptr;
	bool *m_willRename = nullptr;
	bool *m_didDelete = nullptr;
	bool *m_willDelete = nullptr;

public:
	WorkspaceClientCapabilitiesFileOperations() = default;
	virtual ~WorkspaceClientCapabilitiesFileOperations() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_didCreate) { delete m_didCreate; }
		if(m_willCreate) { delete m_willCreate; }
		if(m_didRename) { delete m_didRename; }
		if(m_willRename) { delete m_willRename; }
		if(m_didDelete) { delete m_didDelete; }
		if(m_willDelete) { delete m_willDelete; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setDidCreate(bool *didCreate) { m_didCreate = didCreate; }
	bool *getDidCreate() { return m_didCreate; }
	void setWillCreate(bool *willCreate) { m_willCreate = willCreate; }
	bool *getWillCreate() { return m_willCreate; }
	void setDidRename(bool *didRename) { m_didRename = didRename; }
	bool *getDidRename() { return m_didRename; }
	void setWillRename(bool *willRename) { m_willRename = willRename; }
	bool *getWillRename() { return m_willRename; }
	void setDidDelete(bool *didDelete) { m_didDelete = didDelete; }
	bool *getDidDelete() { return m_didDelete; }
	void setWillDelete(bool *willDelete) { m_willDelete = willDelete; }
	bool *getWillDelete() { return m_willDelete; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_didCreate) {
			QJsonValue didCreate = QJsonValue::fromVariant(QVariant::fromValue(*m_didCreate));
			obj.insert("didCreate", didCreate);
		}
		if(m_willCreate) {
			QJsonValue willCreate = QJsonValue::fromVariant(QVariant::fromValue(*m_willCreate));
			obj.insert("willCreate", willCreate);
		}
		if(m_didRename) {
			QJsonValue didRename = QJsonValue::fromVariant(QVariant::fromValue(*m_didRename));
			obj.insert("didRename", didRename);
		}
		if(m_willRename) {
			QJsonValue willRename = QJsonValue::fromVariant(QVariant::fromValue(*m_willRename));
			obj.insert("willRename", willRename);
		}
		if(m_didDelete) {
			QJsonValue didDelete = QJsonValue::fromVariant(QVariant::fromValue(*m_didDelete));
			obj.insert("didDelete", didDelete);
		}
		if(m_willDelete) {
			QJsonValue willDelete = QJsonValue::fromVariant(QVariant::fromValue(*m_willDelete));
			obj.insert("willDelete", willDelete);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("didCreate")) {
			auto didCreate = new bool(json.value("didCreate").toVariant().value<bool>());
			m_didCreate = didCreate;
		}
		if(json.contains("willCreate")) {
			auto willCreate = new bool(json.value("willCreate").toVariant().value<bool>());
			m_willCreate = willCreate;
		}
		if(json.contains("didRename")) {
			auto didRename = new bool(json.value("didRename").toVariant().value<bool>());
			m_didRename = didRename;
		}
		if(json.contains("willRename")) {
			auto willRename = new bool(json.value("willRename").toVariant().value<bool>());
			m_willRename = willRename;
		}
		if(json.contains("didDelete")) {
			auto didDelete = new bool(json.value("didDelete").toVariant().value<bool>());
			m_didDelete = didDelete;
		}
		if(json.contains("willDelete")) {
			auto willDelete = new bool(json.value("willDelete").toVariant().value<bool>());
			m_willDelete = willDelete;
		}

	}
};
class WorkspaceClientCapabilities: virtual public GoPlsParams {
	bool *m_applyEdit = nullptr;
	WorkspaceClientCapabilitiesWorkspaceEdit *m_workspaceEdit = nullptr;
	DidChangeConfigurationWorkspaceClientCapabilities *m_didChangeConfiguration = nullptr;
	DidChangeWatchedFilesWorkspaceClientCapabilities *m_didChangeWatchedFiles = nullptr;
	WorkspaceSymbolClientCapabilities *m_symbol = nullptr;
	ExecuteCommandClientCapabilities *m_executeCommand = nullptr;
	bool *m_workspaceFolders = nullptr;
	bool *m_configuration = nullptr;
	SemanticTokensWorkspaceClientCapabilities *m_semanticTokens = nullptr;
	CodeLensWorkspaceClientCapabilities *m_codeLens = nullptr;
	WorkspaceClientCapabilitiesFileOperations *m_fileOperations = nullptr;

public:
	WorkspaceClientCapabilities() = default;
	virtual ~WorkspaceClientCapabilities() {
		if(m_applyEdit) { delete m_applyEdit; }
		if(m_workspaceEdit) { delete m_workspaceEdit; }
		if(m_didChangeConfiguration) { delete m_didChangeConfiguration; }
		if(m_didChangeWatchedFiles) { delete m_didChangeWatchedFiles; }
		if(m_symbol) { delete m_symbol; }
		if(m_executeCommand) { delete m_executeCommand; }
		if(m_workspaceFolders) { delete m_workspaceFolders; }
		if(m_configuration) { delete m_configuration; }
		if(m_semanticTokens) { delete m_semanticTokens; }
		if(m_codeLens) { delete m_codeLens; }
		if(m_fileOperations) { delete m_fileOperations; }
	}

	void setApplyEdit(bool *applyEdit) { m_applyEdit = applyEdit; }
	bool *getApplyEdit() { return m_applyEdit; }
	void setWorkspaceEdit(WorkspaceClientCapabilitiesWorkspaceEdit *workspaceEdit) { m_workspaceEdit = workspaceEdit; }
	WorkspaceClientCapabilitiesWorkspaceEdit *getWorkspaceEdit() { return m_workspaceEdit; }
	void setDidChangeConfiguration(DidChangeConfigurationWorkspaceClientCapabilities *didChangeConfiguration) { m_didChangeConfiguration = didChangeConfiguration; }
	DidChangeConfigurationWorkspaceClientCapabilities *getDidChangeConfiguration() { return m_didChangeConfiguration; }
	void setDidChangeWatchedFiles(DidChangeWatchedFilesWorkspaceClientCapabilities *didChangeWatchedFiles) { m_didChangeWatchedFiles = didChangeWatchedFiles; }
	DidChangeWatchedFilesWorkspaceClientCapabilities *getDidChangeWatchedFiles() { return m_didChangeWatchedFiles; }
	void setSymbol(WorkspaceSymbolClientCapabilities *symbol) { m_symbol = symbol; }
	WorkspaceSymbolClientCapabilities *getSymbol() { return m_symbol; }
	void setExecuteCommand(ExecuteCommandClientCapabilities *executeCommand) { m_executeCommand = executeCommand; }
	ExecuteCommandClientCapabilities *getExecuteCommand() { return m_executeCommand; }
	void setWorkspaceFolders(bool *workspaceFolders) { m_workspaceFolders = workspaceFolders; }
	bool *getWorkspaceFolders() { return m_workspaceFolders; }
	void setConfiguration(bool *configuration) { m_configuration = configuration; }
	bool *getConfiguration() { return m_configuration; }
	void setSemanticTokens(SemanticTokensWorkspaceClientCapabilities *semanticTokens) { m_semanticTokens = semanticTokens; }
	SemanticTokensWorkspaceClientCapabilities *getSemanticTokens() { return m_semanticTokens; }
	void setCodeLens(CodeLensWorkspaceClientCapabilities *codeLens) { m_codeLens = codeLens; }
	CodeLensWorkspaceClientCapabilities *getCodeLens() { return m_codeLens; }
	void setFileOperations(WorkspaceClientCapabilitiesFileOperations *fileOperations) { m_fileOperations = fileOperations; }
	WorkspaceClientCapabilitiesFileOperations *getFileOperations() { return m_fileOperations; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_applyEdit) {
			QJsonValue applyEdit = QJsonValue::fromVariant(QVariant::fromValue(*m_applyEdit));
			obj.insert("applyEdit", applyEdit);
		}
		if(m_workspaceEdit) {
			QJsonObject workspaceEdit = m_workspaceEdit->toJson();
			obj.insert("workspaceEdit", workspaceEdit);
		}
		if(m_didChangeConfiguration) {
			QJsonObject didChangeConfiguration = m_didChangeConfiguration->toJson();
			obj.insert("didChangeConfiguration", didChangeConfiguration);
		}
		if(m_didChangeWatchedFiles) {
			QJsonObject didChangeWatchedFiles = m_didChangeWatchedFiles->toJson();
			obj.insert("didChangeWatchedFiles", didChangeWatchedFiles);
		}
		if(m_symbol) {
			QJsonObject symbol = m_symbol->toJson();
			obj.insert("symbol", symbol);
		}
		if(m_executeCommand) {
			QJsonObject executeCommand = m_executeCommand->toJson();
			obj.insert("executeCommand", executeCommand);
		}
		if(m_workspaceFolders) {
			QJsonValue workspaceFolders = QJsonValue::fromVariant(QVariant::fromValue(*m_workspaceFolders));
			obj.insert("workspaceFolders", workspaceFolders);
		}
		if(m_configuration) {
			QJsonValue configuration = QJsonValue::fromVariant(QVariant::fromValue(*m_configuration));
			obj.insert("configuration", configuration);
		}
		if(m_semanticTokens) {
			QJsonObject semanticTokens = m_semanticTokens->toJson();
			obj.insert("semanticTokens", semanticTokens);
		}
		if(m_codeLens) {
			QJsonObject codeLens = m_codeLens->toJson();
			obj.insert("codeLens", codeLens);
		}
		if(m_fileOperations) {
			QJsonObject fileOperations = m_fileOperations->toJson();
			obj.insert("fileOperations", fileOperations);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("applyEdit")) {
			auto applyEdit = new bool(json.value("applyEdit").toVariant().value<bool>());
			m_applyEdit = applyEdit;
		}
		if(json.contains("workspaceEdit")) {
			auto workspaceEdit = new WorkspaceClientCapabilitiesWorkspaceEdit();
			workspaceEdit->fromJson(json.value("workspaceEdit").toObject());
			m_workspaceEdit = workspaceEdit;
		}
		if(json.contains("didChangeConfiguration")) {
			auto didChangeConfiguration = new DidChangeConfigurationWorkspaceClientCapabilities();
			didChangeConfiguration->fromJson(json.value("didChangeConfiguration").toObject());
			m_didChangeConfiguration = didChangeConfiguration;
		}
		if(json.contains("didChangeWatchedFiles")) {
			auto didChangeWatchedFiles = new DidChangeWatchedFilesWorkspaceClientCapabilities();
			didChangeWatchedFiles->fromJson(json.value("didChangeWatchedFiles").toObject());
			m_didChangeWatchedFiles = didChangeWatchedFiles;
		}
		if(json.contains("symbol")) {
			auto symbol = new WorkspaceSymbolClientCapabilities();
			symbol->fromJson(json.value("symbol").toObject());
			m_symbol = symbol;
		}
		if(json.contains("executeCommand")) {
			auto executeCommand = new ExecuteCommandClientCapabilities();
			executeCommand->fromJson(json.value("executeCommand").toObject());
			m_executeCommand = executeCommand;
		}
		if(json.contains("workspaceFolders")) {
			auto workspaceFolders = new bool(json.value("workspaceFolders").toVariant().value<bool>());
			m_workspaceFolders = workspaceFolders;
		}
		if(json.contains("configuration")) {
			auto configuration = new bool(json.value("configuration").toVariant().value<bool>());
			m_configuration = configuration;
		}
		if(json.contains("semanticTokens")) {
			auto semanticTokens = new SemanticTokensWorkspaceClientCapabilities();
			semanticTokens->fromJson(json.value("semanticTokens").toObject());
			m_semanticTokens = semanticTokens;
		}
		if(json.contains("codeLens")) {
			auto codeLens = new CodeLensWorkspaceClientCapabilities();
			codeLens->fromJson(json.value("codeLens").toObject());
			m_codeLens = codeLens;
		}
		if(json.contains("fileOperations")) {
			auto fileOperations = new WorkspaceClientCapabilitiesFileOperations();
			fileOperations->fromJson(json.value("fileOperations").toObject());
			m_fileOperations = fileOperations;
		}

	}
};
class TextDocumentSyncClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	bool *m_willSave = nullptr;
	bool *m_willSaveWaitUntil = nullptr;
	bool *m_didSave = nullptr;

public:
	TextDocumentSyncClientCapabilities() = default;
	virtual ~TextDocumentSyncClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_willSave) { delete m_willSave; }
		if(m_willSaveWaitUntil) { delete m_willSaveWaitUntil; }
		if(m_didSave) { delete m_didSave; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setWillSave(bool *willSave) { m_willSave = willSave; }
	bool *getWillSave() { return m_willSave; }
	void setWillSaveWaitUntil(bool *willSaveWaitUntil) { m_willSaveWaitUntil = willSaveWaitUntil; }
	bool *getWillSaveWaitUntil() { return m_willSaveWaitUntil; }
	void setDidSave(bool *didSave) { m_didSave = didSave; }
	bool *getDidSave() { return m_didSave; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_willSave) {
			QJsonValue willSave = QJsonValue::fromVariant(QVariant::fromValue(*m_willSave));
			obj.insert("willSave", willSave);
		}
		if(m_willSaveWaitUntil) {
			QJsonValue willSaveWaitUntil = QJsonValue::fromVariant(QVariant::fromValue(*m_willSaveWaitUntil));
			obj.insert("willSaveWaitUntil", willSaveWaitUntil);
		}
		if(m_didSave) {
			QJsonValue didSave = QJsonValue::fromVariant(QVariant::fromValue(*m_didSave));
			obj.insert("didSave", didSave);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("willSave")) {
			auto willSave = new bool(json.value("willSave").toVariant().value<bool>());
			m_willSave = willSave;
		}
		if(json.contains("willSaveWaitUntil")) {
			auto willSaveWaitUntil = new bool(json.value("willSaveWaitUntil").toVariant().value<bool>());
			m_willSaveWaitUntil = willSaveWaitUntil;
		}
		if(json.contains("didSave")) {
			auto didSave = new bool(json.value("didSave").toVariant().value<bool>());
			m_didSave = didSave;
		}

	}
};
typedef double CompletionItemTag;
class CompletionTextDocumentClientCapabilitiesItemTagSupport: virtual public GoPlsParams {
	QList<CompletionItemTag*> *m_valueSet = nullptr;

public:
	CompletionTextDocumentClientCapabilitiesItemTagSupport() = default;
	virtual ~CompletionTextDocumentClientCapabilitiesItemTagSupport() {
		if(m_valueSet) { qDeleteAll(*m_valueSet); delete m_valueSet; }
	}

	void setValueSet(QList<CompletionItemTag*> *valueSet) { m_valueSet = valueSet; }
	QList<CompletionItemTag*> *getValueSet() { return m_valueSet; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_valueSet) {
				QJsonArray valueSet;
			for(auto it = m_valueSet->cbegin(); it != m_valueSet->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				valueSet.append(item);
			}

			obj.insert("valueSet", valueSet);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("valueSet")) {
			 QJsonArray arr = json.value("valueSet").toArray();
			auto valueSet = new QList<CompletionItemTag*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				valueSet->append(item);
			}
			m_valueSet = valueSet;
		}

	}
};
class CompletionTextDocumentClientCapabilitiesItemResolveSupport: virtual public GoPlsParams {
	QList<QString*> *m_properties = nullptr;

public:
	CompletionTextDocumentClientCapabilitiesItemResolveSupport() = default;
	virtual ~CompletionTextDocumentClientCapabilitiesItemResolveSupport() {
		if(m_properties) { qDeleteAll(*m_properties); delete m_properties; }
	}

	void setProperties(QList<QString*> *properties) { m_properties = properties; }
	QList<QString*> *getProperties() { return m_properties; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_properties) {
				QJsonArray properties;
			for(auto it = m_properties->cbegin(); it != m_properties->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				properties.append(item);
			}

			obj.insert("properties", properties);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("properties")) {
			 QJsonArray arr = json.value("properties").toArray();
			auto properties = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				properties->append(item);
			}
			m_properties = properties;
		}

	}
};
class CompletionTextDocumentClientCapabilitiesItem: virtual public GoPlsParams {
	bool *m_snippetSupport = nullptr;
	bool *m_commitCharactersSupport = nullptr;
	QList<MarkupKind*> *m_documentationFormat = nullptr;
	bool *m_deprecatedSupport = nullptr;
	bool *m_preselectSupport = nullptr;
	CompletionTextDocumentClientCapabilitiesItemTagSupport *m_tagSupport = nullptr;
	bool *m_insertReplaceSupport = nullptr;
	CompletionTextDocumentClientCapabilitiesItemResolveSupport *m_resolveSupport = nullptr;
	CompletionTextDocumentClientCapabilitiesItemInsertTextModeSupport *m_insertTextModeSupport = nullptr;

public:
	CompletionTextDocumentClientCapabilitiesItem() = default;
	virtual ~CompletionTextDocumentClientCapabilitiesItem() {
		if(m_snippetSupport) { delete m_snippetSupport; }
		if(m_commitCharactersSupport) { delete m_commitCharactersSupport; }
		if(m_documentationFormat) { qDeleteAll(*m_documentationFormat); delete m_documentationFormat; }
		if(m_deprecatedSupport) { delete m_deprecatedSupport; }
		if(m_preselectSupport) { delete m_preselectSupport; }
		if(m_tagSupport) { delete m_tagSupport; }
		if(m_insertReplaceSupport) { delete m_insertReplaceSupport; }
		if(m_resolveSupport) { delete m_resolveSupport; }
		if(m_insertTextModeSupport) { delete m_insertTextModeSupport; }
	}

	void setSnippetSupport(bool *snippetSupport) { m_snippetSupport = snippetSupport; }
	bool *getSnippetSupport() { return m_snippetSupport; }
	void setCommitCharactersSupport(bool *commitCharactersSupport) { m_commitCharactersSupport = commitCharactersSupport; }
	bool *getCommitCharactersSupport() { return m_commitCharactersSupport; }
	void setDocumentationFormat(QList<MarkupKind*> *documentationFormat) { m_documentationFormat = documentationFormat; }
	QList<MarkupKind*> *getDocumentationFormat() { return m_documentationFormat; }
	void setDeprecatedSupport(bool *deprecatedSupport) { m_deprecatedSupport = deprecatedSupport; }
	bool *getDeprecatedSupport() { return m_deprecatedSupport; }
	void setPreselectSupport(bool *preselectSupport) { m_preselectSupport = preselectSupport; }
	bool *getPreselectSupport() { return m_preselectSupport; }
	void setTagSupport(CompletionTextDocumentClientCapabilitiesItemTagSupport *tagSupport) { m_tagSupport = tagSupport; }
	CompletionTextDocumentClientCapabilitiesItemTagSupport *getTagSupport() { return m_tagSupport; }
	void setInsertReplaceSupport(bool *insertReplaceSupport) { m_insertReplaceSupport = insertReplaceSupport; }
	bool *getInsertReplaceSupport() { return m_insertReplaceSupport; }
	void setResolveSupport(CompletionTextDocumentClientCapabilitiesItemResolveSupport *resolveSupport) { m_resolveSupport = resolveSupport; }
	CompletionTextDocumentClientCapabilitiesItemResolveSupport *getResolveSupport() { return m_resolveSupport; }
	void setInsertTextModeSupport(CompletionTextDocumentClientCapabilitiesItemInsertTextModeSupport *insertTextModeSupport) { m_insertTextModeSupport = insertTextModeSupport; }
	CompletionTextDocumentClientCapabilitiesItemInsertTextModeSupport *getInsertTextModeSupport() { return m_insertTextModeSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_snippetSupport) {
			QJsonValue snippetSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_snippetSupport));
			obj.insert("snippetSupport", snippetSupport);
		}
		if(m_commitCharactersSupport) {
			QJsonValue commitCharactersSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_commitCharactersSupport));
			obj.insert("commitCharactersSupport", commitCharactersSupport);
		}
		if(m_documentationFormat) {
				QJsonArray documentationFormat;
			for(auto it = m_documentationFormat->cbegin(); it != m_documentationFormat->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				documentationFormat.append(item);
			}

			obj.insert("documentationFormat", documentationFormat);
		}
		if(m_deprecatedSupport) {
			QJsonValue deprecatedSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_deprecatedSupport));
			obj.insert("deprecatedSupport", deprecatedSupport);
		}
		if(m_preselectSupport) {
			QJsonValue preselectSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_preselectSupport));
			obj.insert("preselectSupport", preselectSupport);
		}
		if(m_tagSupport) {
			QJsonObject tagSupport = m_tagSupport->toJson();
			obj.insert("tagSupport", tagSupport);
		}
		if(m_insertReplaceSupport) {
			QJsonValue insertReplaceSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_insertReplaceSupport));
			obj.insert("insertReplaceSupport", insertReplaceSupport);
		}
		if(m_resolveSupport) {
			QJsonObject resolveSupport = m_resolveSupport->toJson();
			obj.insert("resolveSupport", resolveSupport);
		}
		if(m_insertTextModeSupport) {
			QJsonObject insertTextModeSupport = m_insertTextModeSupport->toJson();
			obj.insert("insertTextModeSupport", insertTextModeSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("snippetSupport")) {
			auto snippetSupport = new bool(json.value("snippetSupport").toVariant().value<bool>());
			m_snippetSupport = snippetSupport;
		}
		if(json.contains("commitCharactersSupport")) {
			auto commitCharactersSupport = new bool(json.value("commitCharactersSupport").toVariant().value<bool>());
			m_commitCharactersSupport = commitCharactersSupport;
		}
		if(json.contains("documentationFormat")) {
			 QJsonArray arr = json.value("documentationFormat").toArray();
			auto documentationFormat = new QList<MarkupKind*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				documentationFormat->append(item);
			}
			m_documentationFormat = documentationFormat;
		}
		if(json.contains("deprecatedSupport")) {
			auto deprecatedSupport = new bool(json.value("deprecatedSupport").toVariant().value<bool>());
			m_deprecatedSupport = deprecatedSupport;
		}
		if(json.contains("preselectSupport")) {
			auto preselectSupport = new bool(json.value("preselectSupport").toVariant().value<bool>());
			m_preselectSupport = preselectSupport;
		}
		if(json.contains("tagSupport")) {
			auto tagSupport = new CompletionTextDocumentClientCapabilitiesItemTagSupport();
			tagSupport->fromJson(json.value("tagSupport").toObject());
			m_tagSupport = tagSupport;
		}
		if(json.contains("insertReplaceSupport")) {
			auto insertReplaceSupport = new bool(json.value("insertReplaceSupport").toVariant().value<bool>());
			m_insertReplaceSupport = insertReplaceSupport;
		}
		if(json.contains("resolveSupport")) {
			auto resolveSupport = new CompletionTextDocumentClientCapabilitiesItemResolveSupport();
			resolveSupport->fromJson(json.value("resolveSupport").toObject());
			m_resolveSupport = resolveSupport;
		}
		if(json.contains("insertTextModeSupport")) {
			auto insertTextModeSupport = new CompletionTextDocumentClientCapabilitiesItemInsertTextModeSupport();
			insertTextModeSupport->fromJson(json.value("insertTextModeSupport").toObject());
			m_insertTextModeSupport = insertTextModeSupport;
		}

	}
};
typedef double CompletionItemKind;
class CompletionTextDocumentClientCapabilitiesItemKind: virtual public GoPlsParams {
	QList<CompletionItemKind*> *m_valueSet = nullptr;

public:
	CompletionTextDocumentClientCapabilitiesItemKind() = default;
	virtual ~CompletionTextDocumentClientCapabilitiesItemKind() {
		if(m_valueSet) { qDeleteAll(*m_valueSet); delete m_valueSet; }
	}

	void setValueSet(QList<CompletionItemKind*> *valueSet) { m_valueSet = valueSet; }
	QList<CompletionItemKind*> *getValueSet() { return m_valueSet; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_valueSet) {
				QJsonArray valueSet;
			for(auto it = m_valueSet->cbegin(); it != m_valueSet->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				valueSet.append(item);
			}

			obj.insert("valueSet", valueSet);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("valueSet")) {
			 QJsonArray arr = json.value("valueSet").toArray();
			auto valueSet = new QList<CompletionItemKind*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				valueSet->append(item);
			}
			m_valueSet = valueSet;
		}

	}
};
class CompletionTextDocumentClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	CompletionTextDocumentClientCapabilitiesItem *m_completionItem = nullptr;
	CompletionTextDocumentClientCapabilitiesItemKind *m_completionItemKind = nullptr;
	bool *m_contextSupport = nullptr;

public:
	CompletionTextDocumentClientCapabilities() = default;
	virtual ~CompletionTextDocumentClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_completionItem) { delete m_completionItem; }
		if(m_completionItemKind) { delete m_completionItemKind; }
		if(m_contextSupport) { delete m_contextSupport; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setCompletionItem(CompletionTextDocumentClientCapabilitiesItem *completionItem) { m_completionItem = completionItem; }
	CompletionTextDocumentClientCapabilitiesItem *getCompletionItem() { return m_completionItem; }
	void setCompletionItemKind(CompletionTextDocumentClientCapabilitiesItemKind *completionItemKind) { m_completionItemKind = completionItemKind; }
	CompletionTextDocumentClientCapabilitiesItemKind *getCompletionItemKind() { return m_completionItemKind; }
	void setContextSupport(bool *contextSupport) { m_contextSupport = contextSupport; }
	bool *getContextSupport() { return m_contextSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_completionItem) {
			QJsonObject completionItem = m_completionItem->toJson();
			obj.insert("completionItem", completionItem);
		}
		if(m_completionItemKind) {
			QJsonObject completionItemKind = m_completionItemKind->toJson();
			obj.insert("completionItemKind", completionItemKind);
		}
		if(m_contextSupport) {
			QJsonValue contextSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_contextSupport));
			obj.insert("contextSupport", contextSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("completionItem")) {
			auto completionItem = new CompletionTextDocumentClientCapabilitiesItem();
			completionItem->fromJson(json.value("completionItem").toObject());
			m_completionItem = completionItem;
		}
		if(json.contains("completionItemKind")) {
			auto completionItemKind = new CompletionTextDocumentClientCapabilitiesItemKind();
			completionItemKind->fromJson(json.value("completionItemKind").toObject());
			m_completionItemKind = completionItemKind;
		}
		if(json.contains("contextSupport")) {
			auto contextSupport = new bool(json.value("contextSupport").toVariant().value<bool>());
			m_contextSupport = contextSupport;
		}

	}
};
class HoverTextDocumentClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	QList<MarkupKind*> *m_contentFormat = nullptr;

public:
	HoverTextDocumentClientCapabilities() = default;
	virtual ~HoverTextDocumentClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_contentFormat) { qDeleteAll(*m_contentFormat); delete m_contentFormat; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setContentFormat(QList<MarkupKind*> *contentFormat) { m_contentFormat = contentFormat; }
	QList<MarkupKind*> *getContentFormat() { return m_contentFormat; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_contentFormat) {
				QJsonArray contentFormat;
			for(auto it = m_contentFormat->cbegin(); it != m_contentFormat->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				contentFormat.append(item);
			}

			obj.insert("contentFormat", contentFormat);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("contentFormat")) {
			 QJsonArray arr = json.value("contentFormat").toArray();
			auto contentFormat = new QList<MarkupKind*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				contentFormat->append(item);
			}
			m_contentFormat = contentFormat;
		}

	}
};
class TextDocumentClientCapabilitiesParameterInformation: virtual public GoPlsParams {
	bool *m_labelOffsetSupport = nullptr;

public:
	TextDocumentClientCapabilitiesParameterInformation() = default;
	virtual ~TextDocumentClientCapabilitiesParameterInformation() {
		if(m_labelOffsetSupport) { delete m_labelOffsetSupport; }
	}

	void setLabelOffsetSupport(bool *labelOffsetSupport) { m_labelOffsetSupport = labelOffsetSupport; }
	bool *getLabelOffsetSupport() { return m_labelOffsetSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_labelOffsetSupport) {
			QJsonValue labelOffsetSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_labelOffsetSupport));
			obj.insert("labelOffsetSupport", labelOffsetSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("labelOffsetSupport")) {
			auto labelOffsetSupport = new bool(json.value("labelOffsetSupport").toVariant().value<bool>());
			m_labelOffsetSupport = labelOffsetSupport;
		}

	}
};
class TextDocumentClientCapabilitiesSignatureInformation: virtual public GoPlsParams {
	QList<MarkupKind*> *m_documentationFormat = nullptr;
	TextDocumentClientCapabilitiesParameterInformation *m_parameterInformation = nullptr;
	bool *m_activeParameterSupport = nullptr;

public:
	TextDocumentClientCapabilitiesSignatureInformation() = default;
	virtual ~TextDocumentClientCapabilitiesSignatureInformation() {
		if(m_documentationFormat) { qDeleteAll(*m_documentationFormat); delete m_documentationFormat; }
		if(m_parameterInformation) { delete m_parameterInformation; }
		if(m_activeParameterSupport) { delete m_activeParameterSupport; }
	}

	void setDocumentationFormat(QList<MarkupKind*> *documentationFormat) { m_documentationFormat = documentationFormat; }
	QList<MarkupKind*> *getDocumentationFormat() { return m_documentationFormat; }
	void setParameterInformation(TextDocumentClientCapabilitiesParameterInformation *parameterInformation) { m_parameterInformation = parameterInformation; }
	TextDocumentClientCapabilitiesParameterInformation *getParameterInformation() { return m_parameterInformation; }
	void setActiveParameterSupport(bool *activeParameterSupport) { m_activeParameterSupport = activeParameterSupport; }
	bool *getActiveParameterSupport() { return m_activeParameterSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_documentationFormat) {
				QJsonArray documentationFormat;
			for(auto it = m_documentationFormat->cbegin(); it != m_documentationFormat->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				documentationFormat.append(item);
			}

			obj.insert("documentationFormat", documentationFormat);
		}
		if(m_parameterInformation) {
			QJsonObject parameterInformation = m_parameterInformation->toJson();
			obj.insert("parameterInformation", parameterInformation);
		}
		if(m_activeParameterSupport) {
			QJsonValue activeParameterSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_activeParameterSupport));
			obj.insert("activeParameterSupport", activeParameterSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("documentationFormat")) {
			 QJsonArray arr = json.value("documentationFormat").toArray();
			auto documentationFormat = new QList<MarkupKind*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				documentationFormat->append(item);
			}
			m_documentationFormat = documentationFormat;
		}
		if(json.contains("parameterInformation")) {
			auto parameterInformation = new TextDocumentClientCapabilitiesParameterInformation();
			parameterInformation->fromJson(json.value("parameterInformation").toObject());
			m_parameterInformation = parameterInformation;
		}
		if(json.contains("activeParameterSupport")) {
			auto activeParameterSupport = new bool(json.value("activeParameterSupport").toVariant().value<bool>());
			m_activeParameterSupport = activeParameterSupport;
		}

	}
};
class SignatureHelpTextDocumentClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	TextDocumentClientCapabilitiesSignatureInformation *m_signatureInformation = nullptr;
	bool *m_contextSupport = nullptr;

public:
	SignatureHelpTextDocumentClientCapabilities() = default;
	virtual ~SignatureHelpTextDocumentClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_signatureInformation) { delete m_signatureInformation; }
		if(m_contextSupport) { delete m_contextSupport; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setSignatureInformation(TextDocumentClientCapabilitiesSignatureInformation *signatureInformation) { m_signatureInformation = signatureInformation; }
	TextDocumentClientCapabilitiesSignatureInformation *getSignatureInformation() { return m_signatureInformation; }
	void setContextSupport(bool *contextSupport) { m_contextSupport = contextSupport; }
	bool *getContextSupport() { return m_contextSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_signatureInformation) {
			QJsonObject signatureInformation = m_signatureInformation->toJson();
			obj.insert("signatureInformation", signatureInformation);
		}
		if(m_contextSupport) {
			QJsonValue contextSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_contextSupport));
			obj.insert("contextSupport", contextSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("signatureInformation")) {
			auto signatureInformation = new TextDocumentClientCapabilitiesSignatureInformation();
			signatureInformation->fromJson(json.value("signatureInformation").toObject());
			m_signatureInformation = signatureInformation;
		}
		if(json.contains("contextSupport")) {
			auto contextSupport = new bool(json.value("contextSupport").toVariant().value<bool>());
			m_contextSupport = contextSupport;
		}

	}
};
class DeclarationTextDocumentClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	bool *m_linkSupport = nullptr;

public:
	DeclarationTextDocumentClientCapabilities() = default;
	virtual ~DeclarationTextDocumentClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_linkSupport) { delete m_linkSupport; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setLinkSupport(bool *linkSupport) { m_linkSupport = linkSupport; }
	bool *getLinkSupport() { return m_linkSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_linkSupport) {
			QJsonValue linkSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_linkSupport));
			obj.insert("linkSupport", linkSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("linkSupport")) {
			auto linkSupport = new bool(json.value("linkSupport").toVariant().value<bool>());
			m_linkSupport = linkSupport;
		}

	}
};
class DefinitionTextDocumentClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	bool *m_linkSupport = nullptr;

public:
	DefinitionTextDocumentClientCapabilities() = default;
	virtual ~DefinitionTextDocumentClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_linkSupport) { delete m_linkSupport; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setLinkSupport(bool *linkSupport) { m_linkSupport = linkSupport; }
	bool *getLinkSupport() { return m_linkSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_linkSupport) {
			QJsonValue linkSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_linkSupport));
			obj.insert("linkSupport", linkSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("linkSupport")) {
			auto linkSupport = new bool(json.value("linkSupport").toVariant().value<bool>());
			m_linkSupport = linkSupport;
		}

	}
};
class TypeDefinitionTextDocumentClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	bool *m_linkSupport = nullptr;

public:
	TypeDefinitionTextDocumentClientCapabilities() = default;
	virtual ~TypeDefinitionTextDocumentClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_linkSupport) { delete m_linkSupport; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setLinkSupport(bool *linkSupport) { m_linkSupport = linkSupport; }
	bool *getLinkSupport() { return m_linkSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_linkSupport) {
			QJsonValue linkSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_linkSupport));
			obj.insert("linkSupport", linkSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("linkSupport")) {
			auto linkSupport = new bool(json.value("linkSupport").toVariant().value<bool>());
			m_linkSupport = linkSupport;
		}

	}
};
class ReferencesTextDocumentClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	ReferencesTextDocumentClientCapabilities() = default;
	virtual ~ReferencesTextDocumentClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class DocumentHighlightClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	DocumentHighlightClientCapabilities() = default;
	virtual ~DocumentHighlightClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class DocumentSymbolClientCapabilitiesTagSupport: virtual public GoPlsParams {
	QList<SymbolTag*> *m_valueSet = nullptr;

public:
	DocumentSymbolClientCapabilitiesTagSupport() = default;
	virtual ~DocumentSymbolClientCapabilitiesTagSupport() {
		if(m_valueSet) { qDeleteAll(*m_valueSet); delete m_valueSet; }
	}

	void setValueSet(QList<SymbolTag*> *valueSet) { m_valueSet = valueSet; }
	QList<SymbolTag*> *getValueSet() { return m_valueSet; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_valueSet) {
				QJsonArray valueSet;
			for(auto it = m_valueSet->cbegin(); it != m_valueSet->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				valueSet.append(item);
			}

			obj.insert("valueSet", valueSet);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("valueSet")) {
			 QJsonArray arr = json.value("valueSet").toArray();
			auto valueSet = new QList<SymbolTag*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				valueSet->append(item);
			}
			m_valueSet = valueSet;
		}

	}
};
class DocumentSymbolClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	SymbolKindCapabilities *m_symbolKind = nullptr;
	bool *m_hierarchicalDocumentSymbolSupport = nullptr;
	DocumentSymbolClientCapabilitiesTagSupport *m_tagSupport = nullptr;
	bool *m_labelSupport = nullptr;

public:
	DocumentSymbolClientCapabilities() = default;
	virtual ~DocumentSymbolClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_symbolKind) { delete m_symbolKind; }
		if(m_hierarchicalDocumentSymbolSupport) { delete m_hierarchicalDocumentSymbolSupport; }
		if(m_tagSupport) { delete m_tagSupport; }
		if(m_labelSupport) { delete m_labelSupport; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setSymbolKind(SymbolKindCapabilities *symbolKind) { m_symbolKind = symbolKind; }
	SymbolKindCapabilities *getSymbolKind() { return m_symbolKind; }
	void setHierarchicalDocumentSymbolSupport(bool *hierarchicalDocumentSymbolSupport) { m_hierarchicalDocumentSymbolSupport = hierarchicalDocumentSymbolSupport; }
	bool *getHierarchicalDocumentSymbolSupport() { return m_hierarchicalDocumentSymbolSupport; }
	void setTagSupport(DocumentSymbolClientCapabilitiesTagSupport *tagSupport) { m_tagSupport = tagSupport; }
	DocumentSymbolClientCapabilitiesTagSupport *getTagSupport() { return m_tagSupport; }
	void setLabelSupport(bool *labelSupport) { m_labelSupport = labelSupport; }
	bool *getLabelSupport() { return m_labelSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_symbolKind) {
			QJsonObject symbolKind = m_symbolKind->toJson();
			obj.insert("symbolKind", symbolKind);
		}
		if(m_hierarchicalDocumentSymbolSupport) {
			QJsonValue hierarchicalDocumentSymbolSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_hierarchicalDocumentSymbolSupport));
			obj.insert("hierarchicalDocumentSymbolSupport", hierarchicalDocumentSymbolSupport);
		}
		if(m_tagSupport) {
			QJsonObject tagSupport = m_tagSupport->toJson();
			obj.insert("tagSupport", tagSupport);
		}
		if(m_labelSupport) {
			QJsonValue labelSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_labelSupport));
			obj.insert("labelSupport", labelSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("symbolKind")) {
			auto symbolKind = new SymbolKindCapabilities();
			symbolKind->fromJson(json.value("symbolKind").toObject());
			m_symbolKind = symbolKind;
		}
		if(json.contains("hierarchicalDocumentSymbolSupport")) {
			auto hierarchicalDocumentSymbolSupport = new bool(json.value("hierarchicalDocumentSymbolSupport").toVariant().value<bool>());
			m_hierarchicalDocumentSymbolSupport = hierarchicalDocumentSymbolSupport;
		}
		if(json.contains("tagSupport")) {
			auto tagSupport = new DocumentSymbolClientCapabilitiesTagSupport();
			tagSupport->fromJson(json.value("tagSupport").toObject());
			m_tagSupport = tagSupport;
		}
		if(json.contains("labelSupport")) {
			auto labelSupport = new bool(json.value("labelSupport").toVariant().value<bool>());
			m_labelSupport = labelSupport;
		}

	}
};
class CodeLensClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	CodeLensClientCapabilities() = default;
	virtual ~CodeLensClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class DocumentLinkClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	bool *m_tooltipSupport = nullptr;

public:
	DocumentLinkClientCapabilities() = default;
	virtual ~DocumentLinkClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_tooltipSupport) { delete m_tooltipSupport; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setTooltipSupport(bool *tooltipSupport) { m_tooltipSupport = tooltipSupport; }
	bool *getTooltipSupport() { return m_tooltipSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_tooltipSupport) {
			QJsonValue tooltipSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_tooltipSupport));
			obj.insert("tooltipSupport", tooltipSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("tooltipSupport")) {
			auto tooltipSupport = new bool(json.value("tooltipSupport").toVariant().value<bool>());
			m_tooltipSupport = tooltipSupport;
		}

	}
};
class DocumentColorClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	DocumentColorClientCapabilities() = default;
	virtual ~DocumentColorClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class DocumentFormattingClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	DocumentFormattingClientCapabilities() = default;
	virtual ~DocumentFormattingClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class DocumentOnTypeFormattingClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	DocumentOnTypeFormattingClientCapabilities() = default;
	virtual ~DocumentOnTypeFormattingClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class PublishDiagnosticsClientCapabilities: virtual public GoPlsParams {
	bool *m_relatedInformation = nullptr;
	PublishDiagnosticsClientCapabilitiesTagSupport *m_tagSupport = nullptr;
	bool *m_versionSupport = nullptr;
	bool *m_codeDescriptionSupport = nullptr;
	bool *m_dataSupport = nullptr;

public:
	PublishDiagnosticsClientCapabilities() = default;
	virtual ~PublishDiagnosticsClientCapabilities() {
		if(m_relatedInformation) { delete m_relatedInformation; }
		if(m_tagSupport) { delete m_tagSupport; }
		if(m_versionSupport) { delete m_versionSupport; }
		if(m_codeDescriptionSupport) { delete m_codeDescriptionSupport; }
		if(m_dataSupport) { delete m_dataSupport; }
	}

	void setRelatedInformation(bool *relatedInformation) { m_relatedInformation = relatedInformation; }
	bool *getRelatedInformation() { return m_relatedInformation; }
	void setTagSupport(PublishDiagnosticsClientCapabilitiesTagSupport *tagSupport) { m_tagSupport = tagSupport; }
	PublishDiagnosticsClientCapabilitiesTagSupport *getTagSupport() { return m_tagSupport; }
	void setVersionSupport(bool *versionSupport) { m_versionSupport = versionSupport; }
	bool *getVersionSupport() { return m_versionSupport; }
	void setCodeDescriptionSupport(bool *codeDescriptionSupport) { m_codeDescriptionSupport = codeDescriptionSupport; }
	bool *getCodeDescriptionSupport() { return m_codeDescriptionSupport; }
	void setDataSupport(bool *dataSupport) { m_dataSupport = dataSupport; }
	bool *getDataSupport() { return m_dataSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_relatedInformation) {
			QJsonValue relatedInformation = QJsonValue::fromVariant(QVariant::fromValue(*m_relatedInformation));
			obj.insert("relatedInformation", relatedInformation);
		}
		if(m_tagSupport) {
			QJsonObject tagSupport = m_tagSupport->toJson();
			obj.insert("tagSupport", tagSupport);
		}
		if(m_versionSupport) {
			QJsonValue versionSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_versionSupport));
			obj.insert("versionSupport", versionSupport);
		}
		if(m_codeDescriptionSupport) {
			QJsonValue codeDescriptionSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_codeDescriptionSupport));
			obj.insert("codeDescriptionSupport", codeDescriptionSupport);
		}
		if(m_dataSupport) {
			QJsonValue dataSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_dataSupport));
			obj.insert("dataSupport", dataSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("relatedInformation")) {
			auto relatedInformation = new bool(json.value("relatedInformation").toVariant().value<bool>());
			m_relatedInformation = relatedInformation;
		}
		if(json.contains("tagSupport")) {
			auto tagSupport = new PublishDiagnosticsClientCapabilitiesTagSupport();
			tagSupport->fromJson(json.value("tagSupport").toObject());
			m_tagSupport = tagSupport;
		}
		if(json.contains("versionSupport")) {
			auto versionSupport = new bool(json.value("versionSupport").toVariant().value<bool>());
			m_versionSupport = versionSupport;
		}
		if(json.contains("codeDescriptionSupport")) {
			auto codeDescriptionSupport = new bool(json.value("codeDescriptionSupport").toVariant().value<bool>());
			m_codeDescriptionSupport = codeDescriptionSupport;
		}
		if(json.contains("dataSupport")) {
			auto dataSupport = new bool(json.value("dataSupport").toVariant().value<bool>());
			m_dataSupport = dataSupport;
		}

	}
};
typedef double PrepareSupportDefaultBehavior;
class RenameClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	bool *m_prepareSupport = nullptr;
	PrepareSupportDefaultBehavior *m_prepareSupportDefaultBehavior = nullptr;
	bool *m_honorsChangeAnnotations = nullptr;

public:
	RenameClientCapabilities() = default;
	virtual ~RenameClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_prepareSupport) { delete m_prepareSupport; }
		if(m_prepareSupportDefaultBehavior) { delete m_prepareSupportDefaultBehavior; }
		if(m_honorsChangeAnnotations) { delete m_honorsChangeAnnotations; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setPrepareSupport(bool *prepareSupport) { m_prepareSupport = prepareSupport; }
	bool *getPrepareSupport() { return m_prepareSupport; }
	void setPrepareSupportDefaultBehavior(PrepareSupportDefaultBehavior *prepareSupportDefaultBehavior) { m_prepareSupportDefaultBehavior = prepareSupportDefaultBehavior; }
	PrepareSupportDefaultBehavior *getPrepareSupportDefaultBehavior() { return m_prepareSupportDefaultBehavior; }
	void setHonorsChangeAnnotations(bool *honorsChangeAnnotations) { m_honorsChangeAnnotations = honorsChangeAnnotations; }
	bool *getHonorsChangeAnnotations() { return m_honorsChangeAnnotations; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_prepareSupport) {
			QJsonValue prepareSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_prepareSupport));
			obj.insert("prepareSupport", prepareSupport);
		}
		if(m_prepareSupportDefaultBehavior) {
			QJsonValue prepareSupportDefaultBehavior = QJsonValue::fromVariant(QVariant::fromValue(*m_prepareSupportDefaultBehavior));
			obj.insert("prepareSupportDefaultBehavior", prepareSupportDefaultBehavior);
		}
		if(m_honorsChangeAnnotations) {
			QJsonValue honorsChangeAnnotations = QJsonValue::fromVariant(QVariant::fromValue(*m_honorsChangeAnnotations));
			obj.insert("honorsChangeAnnotations", honorsChangeAnnotations);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("prepareSupport")) {
			auto prepareSupport = new bool(json.value("prepareSupport").toVariant().value<bool>());
			m_prepareSupport = prepareSupport;
		}
		if(json.contains("prepareSupportDefaultBehavior")) {
			auto prepareSupportDefaultBehavior = new double(json.value("prepareSupportDefaultBehavior").toVariant().value<double>());
			m_prepareSupportDefaultBehavior = prepareSupportDefaultBehavior;
		}
		if(json.contains("honorsChangeAnnotations")) {
			auto honorsChangeAnnotations = new bool(json.value("honorsChangeAnnotations").toVariant().value<bool>());
			m_honorsChangeAnnotations = honorsChangeAnnotations;
		}

	}
};
class SelectionRangeClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	SelectionRangeClientCapabilities() = default;
	virtual ~SelectionRangeClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class SemanticTokensWorkspaceClientCapabilitiesRequests: virtual public GoPlsParams {
	bool *m_range = nullptr;
	QJsonValue *m_full = nullptr;

public:
	SemanticTokensWorkspaceClientCapabilitiesRequests() = default;
	virtual ~SemanticTokensWorkspaceClientCapabilitiesRequests() {
		if(m_range) { delete m_range; }
		if(m_full) { delete m_full; }
	}

	void setRange(bool *range) { m_range = range; }
	bool *getRange() { return m_range; }
	void setFull(QJsonValue *full) { m_full = full; }
	QJsonValue *getFull() { return m_full; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_range) {
			QJsonValue range = QJsonValue::fromVariant(QVariant::fromValue(*m_range));
			obj.insert("range", range);
		}
		if(m_full) {
			QJsonValue full = *m_full;
			obj.insert("full", full);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("range")) {
			auto range = new bool(json.value("range").toVariant().value<bool>());
			m_range = range;
		}
		if(json.contains("full")) {
			auto full = new QJsonValue(json.value("full"));
			m_full = full;
		}

	}
};
typedef QString TokenFormat;
class SemanticTokensClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;
	SemanticTokensWorkspaceClientCapabilitiesRequests *m_requests = nullptr;
	QList<QString*> *m_tokenTypes = nullptr;
	QList<QString*> *m_tokenModifiers = nullptr;
	QList<TokenFormat*> *m_formats = nullptr;
	bool *m_overlappingTokenSupport = nullptr;
	bool *m_multilineTokenSupport = nullptr;

public:
	SemanticTokensClientCapabilities() = default;
	virtual ~SemanticTokensClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
		if(m_requests) { delete m_requests; }
		if(m_tokenTypes) { qDeleteAll(*m_tokenTypes); delete m_tokenTypes; }
		if(m_tokenModifiers) { qDeleteAll(*m_tokenModifiers); delete m_tokenModifiers; }
		if(m_formats) { qDeleteAll(*m_formats); delete m_formats; }
		if(m_overlappingTokenSupport) { delete m_overlappingTokenSupport; }
		if(m_multilineTokenSupport) { delete m_multilineTokenSupport; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	void setRequests(SemanticTokensWorkspaceClientCapabilitiesRequests *requests) { m_requests = requests; }
	SemanticTokensWorkspaceClientCapabilitiesRequests *getRequests() { return m_requests; }
	void setTokenTypes(QList<QString*> *tokenTypes) { m_tokenTypes = tokenTypes; }
	QList<QString*> *getTokenTypes() { return m_tokenTypes; }
	void setTokenModifiers(QList<QString*> *tokenModifiers) { m_tokenModifiers = tokenModifiers; }
	QList<QString*> *getTokenModifiers() { return m_tokenModifiers; }
	void setFormats(QList<TokenFormat*> *formats) { m_formats = formats; }
	QList<TokenFormat*> *getFormats() { return m_formats; }
	void setOverlappingTokenSupport(bool *overlappingTokenSupport) { m_overlappingTokenSupport = overlappingTokenSupport; }
	bool *getOverlappingTokenSupport() { return m_overlappingTokenSupport; }
	void setMultilineTokenSupport(bool *multilineTokenSupport) { m_multilineTokenSupport = multilineTokenSupport; }
	bool *getMultilineTokenSupport() { return m_multilineTokenSupport; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}
		if(m_requests) {
			QJsonObject requests = m_requests->toJson();
			obj.insert("requests", requests);
		}
		if(m_tokenTypes) {
				QJsonArray tokenTypes;
			for(auto it = m_tokenTypes->cbegin(); it != m_tokenTypes->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				tokenTypes.append(item);
			}

			obj.insert("tokenTypes", tokenTypes);
		}
		if(m_tokenModifiers) {
				QJsonArray tokenModifiers;
			for(auto it = m_tokenModifiers->cbegin(); it != m_tokenModifiers->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				tokenModifiers.append(item);
			}

			obj.insert("tokenModifiers", tokenModifiers);
		}
		if(m_formats) {
				QJsonArray formats;
			for(auto it = m_formats->cbegin(); it != m_formats->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				formats.append(item);
			}

			obj.insert("formats", formats);
		}
		if(m_overlappingTokenSupport) {
			QJsonValue overlappingTokenSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_overlappingTokenSupport));
			obj.insert("overlappingTokenSupport", overlappingTokenSupport);
		}
		if(m_multilineTokenSupport) {
			QJsonValue multilineTokenSupport = QJsonValue::fromVariant(QVariant::fromValue(*m_multilineTokenSupport));
			obj.insert("multilineTokenSupport", multilineTokenSupport);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}
		if(json.contains("requests")) {
			auto requests = new SemanticTokensWorkspaceClientCapabilitiesRequests();
			requests->fromJson(json.value("requests").toObject());
			m_requests = requests;
		}
		if(json.contains("tokenTypes")) {
			 QJsonArray arr = json.value("tokenTypes").toArray();
			auto tokenTypes = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				tokenTypes->append(item);
			}
			m_tokenTypes = tokenTypes;
		}
		if(json.contains("tokenModifiers")) {
			 QJsonArray arr = json.value("tokenModifiers").toArray();
			auto tokenModifiers = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				tokenModifiers->append(item);
			}
			m_tokenModifiers = tokenModifiers;
		}
		if(json.contains("formats")) {
			 QJsonArray arr = json.value("formats").toArray();
			auto formats = new QList<TokenFormat*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				formats->append(item);
			}
			m_formats = formats;
		}
		if(json.contains("overlappingTokenSupport")) {
			auto overlappingTokenSupport = new bool(json.value("overlappingTokenSupport").toVariant().value<bool>());
			m_overlappingTokenSupport = overlappingTokenSupport;
		}
		if(json.contains("multilineTokenSupport")) {
			auto multilineTokenSupport = new bool(json.value("multilineTokenSupport").toVariant().value<bool>());
			m_multilineTokenSupport = multilineTokenSupport;
		}

	}
};
class LinkedEditingRangeClientCapabilities: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	LinkedEditingRangeClientCapabilities() = default;
	virtual ~LinkedEditingRangeClientCapabilities() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class TextDocumentClientCapabilities: virtual public GoPlsParams {
	TextDocumentSyncClientCapabilities *m_synchronization = nullptr;
	CompletionTextDocumentClientCapabilities *m_completion = nullptr;
	HoverTextDocumentClientCapabilities *m_hover = nullptr;
	SignatureHelpTextDocumentClientCapabilities *m_signatureHelp = nullptr;
	DeclarationTextDocumentClientCapabilities *m_declaration = nullptr;
	DefinitionTextDocumentClientCapabilities *m_definition = nullptr;
	TypeDefinitionTextDocumentClientCapabilities *m_typeDefinition = nullptr;
	ImplementationTextDocumentClientCapabilities *m_implementation = nullptr;
	ReferencesTextDocumentClientCapabilities *m_references = nullptr;
	DocumentHighlightClientCapabilities *m_documentHighlight = nullptr;
	DocumentSymbolClientCapabilities *m_documentSymbol = nullptr;
	CodeActionClientCapabilities *m_codeAction = nullptr;
	CodeLensClientCapabilities *m_codeLens = nullptr;
	DocumentLinkClientCapabilities *m_documentLink = nullptr;
	DocumentColorClientCapabilities *m_colorProvider = nullptr;
	DocumentFormattingClientCapabilities *m_formatting = nullptr;
	DocumentRangeFormattingClientCapabilities *m_rangeFormatting = nullptr;
	DocumentOnTypeFormattingClientCapabilities *m_onTypeFormatting = nullptr;
	PublishDiagnosticsClientCapabilities *m_publishDiagnostics = nullptr;
	RenameClientCapabilities *m_rename = nullptr;
	FoldingRangeClientCapabilities *m_foldingRange = nullptr;
	SelectionRangeClientCapabilities *m_selectionRange = nullptr;
	CallHierarchyClientCapabilities *m_callHierarchy = nullptr;
	SemanticTokensClientCapabilities *m_semanticTokens = nullptr;
	LinkedEditingRangeClientCapabilities *m_linkedEditingRange = nullptr;
	MonikerClientCapabilities *m_moniker = nullptr;

public:
	TextDocumentClientCapabilities() = default;
	virtual ~TextDocumentClientCapabilities() {
		if(m_synchronization) { delete m_synchronization; }
		if(m_completion) { delete m_completion; }
		if(m_hover) { delete m_hover; }
		if(m_signatureHelp) { delete m_signatureHelp; }
		if(m_declaration) { delete m_declaration; }
		if(m_definition) { delete m_definition; }
		if(m_typeDefinition) { delete m_typeDefinition; }
		if(m_implementation) { delete m_implementation; }
		if(m_references) { delete m_references; }
		if(m_documentHighlight) { delete m_documentHighlight; }
		if(m_documentSymbol) { delete m_documentSymbol; }
		if(m_codeAction) { delete m_codeAction; }
		if(m_codeLens) { delete m_codeLens; }
		if(m_documentLink) { delete m_documentLink; }
		if(m_colorProvider) { delete m_colorProvider; }
		if(m_formatting) { delete m_formatting; }
		if(m_rangeFormatting) { delete m_rangeFormatting; }
		if(m_onTypeFormatting) { delete m_onTypeFormatting; }
		if(m_publishDiagnostics) { delete m_publishDiagnostics; }
		if(m_rename) { delete m_rename; }
		if(m_foldingRange) { delete m_foldingRange; }
		if(m_selectionRange) { delete m_selectionRange; }
		if(m_callHierarchy) { delete m_callHierarchy; }
		if(m_semanticTokens) { delete m_semanticTokens; }
		if(m_linkedEditingRange) { delete m_linkedEditingRange; }
		if(m_moniker) { delete m_moniker; }
	}

	void setSynchronization(TextDocumentSyncClientCapabilities *synchronization) { m_synchronization = synchronization; }
	TextDocumentSyncClientCapabilities *getSynchronization() { return m_synchronization; }
	void setCompletion(CompletionTextDocumentClientCapabilities *completion) { m_completion = completion; }
	CompletionTextDocumentClientCapabilities *getCompletion() { return m_completion; }
	void setHover(HoverTextDocumentClientCapabilities *hover) { m_hover = hover; }
	HoverTextDocumentClientCapabilities *getHover() { return m_hover; }
	void setSignatureHelp(SignatureHelpTextDocumentClientCapabilities *signatureHelp) { m_signatureHelp = signatureHelp; }
	SignatureHelpTextDocumentClientCapabilities *getSignatureHelp() { return m_signatureHelp; }
	void setDeclaration(DeclarationTextDocumentClientCapabilities *declaration) { m_declaration = declaration; }
	DeclarationTextDocumentClientCapabilities *getDeclaration() { return m_declaration; }
	void setDefinition(DefinitionTextDocumentClientCapabilities *definition) { m_definition = definition; }
	DefinitionTextDocumentClientCapabilities *getDefinition() { return m_definition; }
	void setTypeDefinition(TypeDefinitionTextDocumentClientCapabilities *typeDefinition) { m_typeDefinition = typeDefinition; }
	TypeDefinitionTextDocumentClientCapabilities *getTypeDefinition() { return m_typeDefinition; }
	void setImplementation(ImplementationTextDocumentClientCapabilities *implementation) { m_implementation = implementation; }
	ImplementationTextDocumentClientCapabilities *getImplementation() { return m_implementation; }
	void setReferences(ReferencesTextDocumentClientCapabilities *references) { m_references = references; }
	ReferencesTextDocumentClientCapabilities *getReferences() { return m_references; }
	void setDocumentHighlight(DocumentHighlightClientCapabilities *documentHighlight) { m_documentHighlight = documentHighlight; }
	DocumentHighlightClientCapabilities *getDocumentHighlight() { return m_documentHighlight; }
	void setDocumentSymbol(DocumentSymbolClientCapabilities *documentSymbol) { m_documentSymbol = documentSymbol; }
	DocumentSymbolClientCapabilities *getDocumentSymbol() { return m_documentSymbol; }
	void setCodeAction(CodeActionClientCapabilities *codeAction) { m_codeAction = codeAction; }
	CodeActionClientCapabilities *getCodeAction() { return m_codeAction; }
	void setCodeLens(CodeLensClientCapabilities *codeLens) { m_codeLens = codeLens; }
	CodeLensClientCapabilities *getCodeLens() { return m_codeLens; }
	void setDocumentLink(DocumentLinkClientCapabilities *documentLink) { m_documentLink = documentLink; }
	DocumentLinkClientCapabilities *getDocumentLink() { return m_documentLink; }
	void setColorProvider(DocumentColorClientCapabilities *colorProvider) { m_colorProvider = colorProvider; }
	DocumentColorClientCapabilities *getColorProvider() { return m_colorProvider; }
	void setFormatting(DocumentFormattingClientCapabilities *formatting) { m_formatting = formatting; }
	DocumentFormattingClientCapabilities *getFormatting() { return m_formatting; }
	void setRangeFormatting(DocumentRangeFormattingClientCapabilities *rangeFormatting) { m_rangeFormatting = rangeFormatting; }
	DocumentRangeFormattingClientCapabilities *getRangeFormatting() { return m_rangeFormatting; }
	void setOnTypeFormatting(DocumentOnTypeFormattingClientCapabilities *onTypeFormatting) { m_onTypeFormatting = onTypeFormatting; }
	DocumentOnTypeFormattingClientCapabilities *getOnTypeFormatting() { return m_onTypeFormatting; }
	void setPublishDiagnostics(PublishDiagnosticsClientCapabilities *publishDiagnostics) { m_publishDiagnostics = publishDiagnostics; }
	PublishDiagnosticsClientCapabilities *getPublishDiagnostics() { return m_publishDiagnostics; }
	void setRename(RenameClientCapabilities *rename) { m_rename = rename; }
	RenameClientCapabilities *getRename() { return m_rename; }
	void setFoldingRange(FoldingRangeClientCapabilities *foldingRange) { m_foldingRange = foldingRange; }
	FoldingRangeClientCapabilities *getFoldingRange() { return m_foldingRange; }
	void setSelectionRange(SelectionRangeClientCapabilities *selectionRange) { m_selectionRange = selectionRange; }
	SelectionRangeClientCapabilities *getSelectionRange() { return m_selectionRange; }
	void setCallHierarchy(CallHierarchyClientCapabilities *callHierarchy) { m_callHierarchy = callHierarchy; }
	CallHierarchyClientCapabilities *getCallHierarchy() { return m_callHierarchy; }
	void setSemanticTokens(SemanticTokensClientCapabilities *semanticTokens) { m_semanticTokens = semanticTokens; }
	SemanticTokensClientCapabilities *getSemanticTokens() { return m_semanticTokens; }
	void setLinkedEditingRange(LinkedEditingRangeClientCapabilities *linkedEditingRange) { m_linkedEditingRange = linkedEditingRange; }
	LinkedEditingRangeClientCapabilities *getLinkedEditingRange() { return m_linkedEditingRange; }
	void setMoniker(MonikerClientCapabilities *moniker) { m_moniker = moniker; }
	MonikerClientCapabilities *getMoniker() { return m_moniker; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_synchronization) {
			QJsonObject synchronization = m_synchronization->toJson();
			obj.insert("synchronization", synchronization);
		}
		if(m_completion) {
			QJsonObject completion = m_completion->toJson();
			obj.insert("completion", completion);
		}
		if(m_hover) {
			QJsonObject hover = m_hover->toJson();
			obj.insert("hover", hover);
		}
		if(m_signatureHelp) {
			QJsonObject signatureHelp = m_signatureHelp->toJson();
			obj.insert("signatureHelp", signatureHelp);
		}
		if(m_declaration) {
			QJsonObject declaration = m_declaration->toJson();
			obj.insert("declaration", declaration);
		}
		if(m_definition) {
			QJsonObject definition = m_definition->toJson();
			obj.insert("definition", definition);
		}
		if(m_typeDefinition) {
			QJsonObject typeDefinition = m_typeDefinition->toJson();
			obj.insert("typeDefinition", typeDefinition);
		}
		if(m_implementation) {
			QJsonObject implementation = m_implementation->toJson();
			obj.insert("implementation", implementation);
		}
		if(m_references) {
			QJsonObject references = m_references->toJson();
			obj.insert("references", references);
		}
		if(m_documentHighlight) {
			QJsonObject documentHighlight = m_documentHighlight->toJson();
			obj.insert("documentHighlight", documentHighlight);
		}
		if(m_documentSymbol) {
			QJsonObject documentSymbol = m_documentSymbol->toJson();
			obj.insert("documentSymbol", documentSymbol);
		}
		if(m_codeAction) {
			QJsonObject codeAction = m_codeAction->toJson();
			obj.insert("codeAction", codeAction);
		}
		if(m_codeLens) {
			QJsonObject codeLens = m_codeLens->toJson();
			obj.insert("codeLens", codeLens);
		}
		if(m_documentLink) {
			QJsonObject documentLink = m_documentLink->toJson();
			obj.insert("documentLink", documentLink);
		}
		if(m_colorProvider) {
			QJsonObject colorProvider = m_colorProvider->toJson();
			obj.insert("colorProvider", colorProvider);
		}
		if(m_formatting) {
			QJsonObject formatting = m_formatting->toJson();
			obj.insert("formatting", formatting);
		}
		if(m_rangeFormatting) {
			QJsonObject rangeFormatting = m_rangeFormatting->toJson();
			obj.insert("rangeFormatting", rangeFormatting);
		}
		if(m_onTypeFormatting) {
			QJsonObject onTypeFormatting = m_onTypeFormatting->toJson();
			obj.insert("onTypeFormatting", onTypeFormatting);
		}
		if(m_publishDiagnostics) {
			QJsonObject publishDiagnostics = m_publishDiagnostics->toJson();
			obj.insert("publishDiagnostics", publishDiagnostics);
		}
		if(m_rename) {
			QJsonObject rename = m_rename->toJson();
			obj.insert("rename", rename);
		}
		if(m_foldingRange) {
			QJsonObject foldingRange = m_foldingRange->toJson();
			obj.insert("foldingRange", foldingRange);
		}
		if(m_selectionRange) {
			QJsonObject selectionRange = m_selectionRange->toJson();
			obj.insert("selectionRange", selectionRange);
		}
		if(m_callHierarchy) {
			QJsonObject callHierarchy = m_callHierarchy->toJson();
			obj.insert("callHierarchy", callHierarchy);
		}
		if(m_semanticTokens) {
			QJsonObject semanticTokens = m_semanticTokens->toJson();
			obj.insert("semanticTokens", semanticTokens);
		}
		if(m_linkedEditingRange) {
			QJsonObject linkedEditingRange = m_linkedEditingRange->toJson();
			obj.insert("linkedEditingRange", linkedEditingRange);
		}
		if(m_moniker) {
			QJsonObject moniker = m_moniker->toJson();
			obj.insert("moniker", moniker);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("synchronization")) {
			auto synchronization = new TextDocumentSyncClientCapabilities();
			synchronization->fromJson(json.value("synchronization").toObject());
			m_synchronization = synchronization;
		}
		if(json.contains("completion")) {
			auto completion = new CompletionTextDocumentClientCapabilities();
			completion->fromJson(json.value("completion").toObject());
			m_completion = completion;
		}
		if(json.contains("hover")) {
			auto hover = new HoverTextDocumentClientCapabilities();
			hover->fromJson(json.value("hover").toObject());
			m_hover = hover;
		}
		if(json.contains("signatureHelp")) {
			auto signatureHelp = new SignatureHelpTextDocumentClientCapabilities();
			signatureHelp->fromJson(json.value("signatureHelp").toObject());
			m_signatureHelp = signatureHelp;
		}
		if(json.contains("declaration")) {
			auto declaration = new DeclarationTextDocumentClientCapabilities();
			declaration->fromJson(json.value("declaration").toObject());
			m_declaration = declaration;
		}
		if(json.contains("definition")) {
			auto definition = new DefinitionTextDocumentClientCapabilities();
			definition->fromJson(json.value("definition").toObject());
			m_definition = definition;
		}
		if(json.contains("typeDefinition")) {
			auto typeDefinition = new TypeDefinitionTextDocumentClientCapabilities();
			typeDefinition->fromJson(json.value("typeDefinition").toObject());
			m_typeDefinition = typeDefinition;
		}
		if(json.contains("implementation")) {
			auto implementation = new ImplementationTextDocumentClientCapabilities();
			implementation->fromJson(json.value("implementation").toObject());
			m_implementation = implementation;
		}
		if(json.contains("references")) {
			auto references = new ReferencesTextDocumentClientCapabilities();
			references->fromJson(json.value("references").toObject());
			m_references = references;
		}
		if(json.contains("documentHighlight")) {
			auto documentHighlight = new DocumentHighlightClientCapabilities();
			documentHighlight->fromJson(json.value("documentHighlight").toObject());
			m_documentHighlight = documentHighlight;
		}
		if(json.contains("documentSymbol")) {
			auto documentSymbol = new DocumentSymbolClientCapabilities();
			documentSymbol->fromJson(json.value("documentSymbol").toObject());
			m_documentSymbol = documentSymbol;
		}
		if(json.contains("codeAction")) {
			auto codeAction = new CodeActionClientCapabilities();
			codeAction->fromJson(json.value("codeAction").toObject());
			m_codeAction = codeAction;
		}
		if(json.contains("codeLens")) {
			auto codeLens = new CodeLensClientCapabilities();
			codeLens->fromJson(json.value("codeLens").toObject());
			m_codeLens = codeLens;
		}
		if(json.contains("documentLink")) {
			auto documentLink = new DocumentLinkClientCapabilities();
			documentLink->fromJson(json.value("documentLink").toObject());
			m_documentLink = documentLink;
		}
		if(json.contains("colorProvider")) {
			auto colorProvider = new DocumentColorClientCapabilities();
			colorProvider->fromJson(json.value("colorProvider").toObject());
			m_colorProvider = colorProvider;
		}
		if(json.contains("formatting")) {
			auto formatting = new DocumentFormattingClientCapabilities();
			formatting->fromJson(json.value("formatting").toObject());
			m_formatting = formatting;
		}
		if(json.contains("rangeFormatting")) {
			auto rangeFormatting = new DocumentRangeFormattingClientCapabilities();
			rangeFormatting->fromJson(json.value("rangeFormatting").toObject());
			m_rangeFormatting = rangeFormatting;
		}
		if(json.contains("onTypeFormatting")) {
			auto onTypeFormatting = new DocumentOnTypeFormattingClientCapabilities();
			onTypeFormatting->fromJson(json.value("onTypeFormatting").toObject());
			m_onTypeFormatting = onTypeFormatting;
		}
		if(json.contains("publishDiagnostics")) {
			auto publishDiagnostics = new PublishDiagnosticsClientCapabilities();
			publishDiagnostics->fromJson(json.value("publishDiagnostics").toObject());
			m_publishDiagnostics = publishDiagnostics;
		}
		if(json.contains("rename")) {
			auto rename = new RenameClientCapabilities();
			rename->fromJson(json.value("rename").toObject());
			m_rename = rename;
		}
		if(json.contains("foldingRange")) {
			auto foldingRange = new FoldingRangeClientCapabilities();
			foldingRange->fromJson(json.value("foldingRange").toObject());
			m_foldingRange = foldingRange;
		}
		if(json.contains("selectionRange")) {
			auto selectionRange = new SelectionRangeClientCapabilities();
			selectionRange->fromJson(json.value("selectionRange").toObject());
			m_selectionRange = selectionRange;
		}
		if(json.contains("callHierarchy")) {
			auto callHierarchy = new CallHierarchyClientCapabilities();
			callHierarchy->fromJson(json.value("callHierarchy").toObject());
			m_callHierarchy = callHierarchy;
		}
		if(json.contains("semanticTokens")) {
			auto semanticTokens = new SemanticTokensClientCapabilities();
			semanticTokens->fromJson(json.value("semanticTokens").toObject());
			m_semanticTokens = semanticTokens;
		}
		if(json.contains("linkedEditingRange")) {
			auto linkedEditingRange = new LinkedEditingRangeClientCapabilities();
			linkedEditingRange->fromJson(json.value("linkedEditingRange").toObject());
			m_linkedEditingRange = linkedEditingRange;
		}
		if(json.contains("moniker")) {
			auto moniker = new MonikerClientCapabilities();
			moniker->fromJson(json.value("moniker").toObject());
			m_moniker = moniker;
		}

	}
};
class ShowMessageRequestClientCapabilities: virtual public GoPlsParams {
	ShowMessageRequestClientCapabilitiesMessageActionItem *m_messageActionItem = nullptr;

public:
	ShowMessageRequestClientCapabilities() = default;
	virtual ~ShowMessageRequestClientCapabilities() {
		if(m_messageActionItem) { delete m_messageActionItem; }
	}

	void setMessageActionItem(ShowMessageRequestClientCapabilitiesMessageActionItem *messageActionItem) { m_messageActionItem = messageActionItem; }
	ShowMessageRequestClientCapabilitiesMessageActionItem *getMessageActionItem() { return m_messageActionItem; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_messageActionItem) {
			QJsonObject messageActionItem = m_messageActionItem->toJson();
			obj.insert("messageActionItem", messageActionItem);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("messageActionItem")) {
			auto messageActionItem = new ShowMessageRequestClientCapabilitiesMessageActionItem();
			messageActionItem->fromJson(json.value("messageActionItem").toObject());
			m_messageActionItem = messageActionItem;
		}

	}
};
class WindowClientCapabilities: virtual public GoPlsParams {
	bool *m_workDoneProgress = nullptr;
	ShowMessageRequestClientCapabilities *m_showMessage = nullptr;
	ShowDocumentClientCapabilities *m_showDocument = nullptr;

public:
	WindowClientCapabilities() = default;
	virtual ~WindowClientCapabilities() {
		if(m_workDoneProgress) { delete m_workDoneProgress; }
		if(m_showMessage) { delete m_showMessage; }
		if(m_showDocument) { delete m_showDocument; }
	}

	void setWorkDoneProgress(bool *workDoneProgress) { m_workDoneProgress = workDoneProgress; }
	bool *getWorkDoneProgress() { return m_workDoneProgress; }
	void setShowMessage(ShowMessageRequestClientCapabilities *showMessage) { m_showMessage = showMessage; }
	ShowMessageRequestClientCapabilities *getShowMessage() { return m_showMessage; }
	void setShowDocument(ShowDocumentClientCapabilities *showDocument) { m_showDocument = showDocument; }
	ShowDocumentClientCapabilities *getShowDocument() { return m_showDocument; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_workDoneProgress) {
			QJsonValue workDoneProgress = QJsonValue::fromVariant(QVariant::fromValue(*m_workDoneProgress));
			obj.insert("workDoneProgress", workDoneProgress);
		}
		if(m_showMessage) {
			QJsonObject showMessage = m_showMessage->toJson();
			obj.insert("showMessage", showMessage);
		}
		if(m_showDocument) {
			QJsonObject showDocument = m_showDocument->toJson();
			obj.insert("showDocument", showDocument);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("workDoneProgress")) {
			auto workDoneProgress = new bool(json.value("workDoneProgress").toVariant().value<bool>());
			m_workDoneProgress = workDoneProgress;
		}
		if(json.contains("showMessage")) {
			auto showMessage = new ShowMessageRequestClientCapabilities();
			showMessage->fromJson(json.value("showMessage").toObject());
			m_showMessage = showMessage;
		}
		if(json.contains("showDocument")) {
			auto showDocument = new ShowDocumentClientCapabilities();
			showDocument->fromJson(json.value("showDocument").toObject());
			m_showDocument = showDocument;
		}

	}
};
class ClientCapabilities: virtual public GoPlsParams {
	WorkspaceClientCapabilities *m_workspace = nullptr;
	TextDocumentClientCapabilities *m_textDocument = nullptr;
	WindowClientCapabilities *m_window = nullptr;
	GeneralClientCapabilities *m_general = nullptr;
	QJsonValue *m_experimental = nullptr;

public:
	ClientCapabilities() = default;
	virtual ~ClientCapabilities() {
		if(m_workspace) { delete m_workspace; }
		if(m_textDocument) { delete m_textDocument; }
		if(m_window) { delete m_window; }
		if(m_general) { delete m_general; }
		if(m_experimental) { delete m_experimental; }
	}

	void setWorkspace(WorkspaceClientCapabilities *workspace) { m_workspace = workspace; }
	WorkspaceClientCapabilities *getWorkspace() { return m_workspace; }
	void setTextDocument(TextDocumentClientCapabilities *textDocument) { m_textDocument = textDocument; }
	TextDocumentClientCapabilities *getTextDocument() { return m_textDocument; }
	void setWindow(WindowClientCapabilities *window) { m_window = window; }
	WindowClientCapabilities *getWindow() { return m_window; }
	void setGeneral(GeneralClientCapabilities *general) { m_general = general; }
	GeneralClientCapabilities *getGeneral() { return m_general; }
	void setExperimental(QJsonValue *experimental) { m_experimental = experimental; }
	QJsonValue *getExperimental() { return m_experimental; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_workspace) {
			QJsonObject workspace = m_workspace->toJson();
			obj.insert("workspace", workspace);
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_window) {
			QJsonObject window = m_window->toJson();
			obj.insert("window", window);
		}
		if(m_general) {
			QJsonObject general = m_general->toJson();
			obj.insert("general", general);
		}
		if(m_experimental) {
			QJsonValue experimental = *m_experimental;
			obj.insert("experimental", experimental);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("workspace")) {
			auto workspace = new WorkspaceClientCapabilities();
			workspace->fromJson(json.value("workspace").toObject());
			m_workspace = workspace;
		}
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentClientCapabilities();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("window")) {
			auto window = new WindowClientCapabilities();
			window->fromJson(json.value("window").toObject());
			m_window = window;
		}
		if(json.contains("general")) {
			auto general = new GeneralClientCapabilities();
			general->fromJson(json.value("general").toObject());
			m_general = general;
		}
		if(json.contains("experimental")) {
			auto experimental = new QJsonValue(json.value("experimental"));
			m_experimental = experimental;
		}

	}
};
typedef QString TraceValue;
class InitializeParams:  public WorkDoneProgressParams {
	int *m_processId = nullptr;
	ClientInfo *m_clientInfo = nullptr;
	QString *m_locale = nullptr;
	QString *m_rootPath = nullptr;
	DocumentURI *m_rootUri = nullptr;
	QJsonValue *m_initializationOptions = nullptr;
	ClientCapabilities *m_capabilities = nullptr;
	TraceValue *m_trace = nullptr;
	QList<WorkspaceFolder*> *m_workspaceFolders = nullptr;

public:
	InitializeParams() = default;
	virtual ~InitializeParams() {
		if(m_processId) { delete m_processId; }
		if(m_clientInfo) { delete m_clientInfo; }
		if(m_locale) { delete m_locale; }
		if(m_rootPath) { delete m_rootPath; }
		if(m_rootUri) { delete m_rootUri; }
		if(m_initializationOptions) { delete m_initializationOptions; }
		if(m_capabilities) { delete m_capabilities; }
		if(m_trace) { delete m_trace; }
		if(m_workspaceFolders) { qDeleteAll(*m_workspaceFolders); delete m_workspaceFolders; }
	}

	void setProcessId(int *processId) { m_processId = processId; }
	int *getProcessId() { return m_processId; }
	void setClientInfo(ClientInfo *clientInfo) { m_clientInfo = clientInfo; }
	ClientInfo *getClientInfo() { return m_clientInfo; }
	void setLocale(QString *locale) { m_locale = locale; }
	QString *getLocale() { return m_locale; }
	void setRootPath(QString *rootPath) { m_rootPath = rootPath; }
	QString *getRootPath() { return m_rootPath; }
	void setRootUri(DocumentURI *rootUri) { m_rootUri = rootUri; }
	DocumentURI *getRootUri() { return m_rootUri; }
	void setInitializationOptions(QJsonValue *initializationOptions) { m_initializationOptions = initializationOptions; }
	QJsonValue *getInitializationOptions() { return m_initializationOptions; }
	void setCapabilities(ClientCapabilities *capabilities) { m_capabilities = capabilities; }
	ClientCapabilities *getCapabilities() { return m_capabilities; }
	void setTrace(TraceValue *trace) { m_trace = trace; }
	TraceValue *getTrace() { return m_trace; }
	void setWorkspaceFolders(QList<WorkspaceFolder*> *workspaceFolders) { m_workspaceFolders = workspaceFolders; }
	QList<WorkspaceFolder*> *getWorkspaceFolders() { return m_workspaceFolders; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		if(m_processId) {
			QJsonValue processId = QJsonValue::fromVariant(QVariant::fromValue(*m_processId));
			obj.insert("processId", processId);
		}
		if(m_clientInfo) {
			QJsonObject clientInfo = m_clientInfo->toJson();
			obj.insert("clientInfo", clientInfo);
		}
		if(m_locale) {
			QJsonValue locale = QJsonValue::fromVariant(QVariant::fromValue(*m_locale));
			obj.insert("locale", locale);
		}
		if(m_rootPath) {
			QJsonValue rootPath = QJsonValue::fromVariant(QVariant::fromValue(*m_rootPath));
			obj.insert("rootPath", rootPath);
		}
		if(m_rootUri) {
			QJsonValue rootUri = QJsonValue::fromVariant(QVariant::fromValue(*m_rootUri));
			obj.insert("rootUri", rootUri);
		}
		if(m_initializationOptions) {
			QJsonValue initializationOptions = *m_initializationOptions;
			obj.insert("initializationOptions", initializationOptions);
		}
		if(m_capabilities) {
			QJsonObject capabilities = m_capabilities->toJson();
			obj.insert("capabilities", capabilities);
		}
		if(m_trace) {
			QJsonValue trace = QJsonValue::fromVariant(QVariant::fromValue(*m_trace));
			obj.insert("trace", trace);
		}
		if(m_workspaceFolders) {
				QJsonArray workspaceFolders;
			for(auto it = m_workspaceFolders->cbegin(); it != m_workspaceFolders->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				workspaceFolders.append(item);
			}

			obj.insert("workspaceFolders", workspaceFolders);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		if(json.contains("processId")) {
			auto processId = new int(json.value("processId").toVariant().value<int>());
			m_processId = processId;
		}
		if(json.contains("clientInfo")) {
			auto clientInfo = new ClientInfo();
			clientInfo->fromJson(json.value("clientInfo").toObject());
			m_clientInfo = clientInfo;
		}
		if(json.contains("locale")) {
			auto locale = new QString(json.value("locale").toString());
			m_locale = locale;
		}
		if(json.contains("rootPath")) {
			auto rootPath = new QString(json.value("rootPath").toString());
			m_rootPath = rootPath;
		}
		if(json.contains("rootUri")) {
			auto rootUri = new QString(json.value("rootUri").toString());
			m_rootUri = rootUri;
		}
		if(json.contains("initializationOptions")) {
			auto initializationOptions = new QJsonValue(json.value("initializationOptions"));
			m_initializationOptions = initializationOptions;
		}
		if(json.contains("capabilities")) {
			auto capabilities = new ClientCapabilities();
			capabilities->fromJson(json.value("capabilities").toObject());
			m_capabilities = capabilities;
		}
		if(json.contains("trace")) {
			auto trace = new QString(json.value("trace").toString());
			m_trace = trace;
		}
		if(json.contains("workspaceFolders")) {
			 QJsonArray arr = json.value("workspaceFolders").toArray();
			auto workspaceFolders = new QList<WorkspaceFolder*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new WorkspaceFolder();
			item->fromJson((m_item).toObject());
				workspaceFolders->append(item);
			}
			m_workspaceFolders = workspaceFolders;
		}

	}
};
const int CompletionItemKindTypeParameter = 25;
class DeclarationParams:  public TextDocumentPositionParams, public WorkDoneProgressParams, public PartialResultParams {

public:
	DeclarationParams() = default;
	virtual ~DeclarationParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);

	}
};
class Unregistration: virtual public GoPlsParams {
	QString *m_id = nullptr;
	QString *m_method = nullptr;

public:
	Unregistration() = default;
	virtual ~Unregistration() {
		if(m_id) { delete m_id; }
		if(m_method) { delete m_method; }
	}

	void setId(QString *id) { m_id = id; }
	QString *getId() { return m_id; }
	void setMethod(QString *method) { m_method = method; }
	QString *getMethod() { return m_method; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_id) {
			QJsonValue id = QJsonValue::fromVariant(QVariant::fromValue(*m_id));
			obj.insert("id", id);
		}
		if(m_method) {
			QJsonValue method = QJsonValue::fromVariant(QVariant::fromValue(*m_method));
			obj.insert("method", method);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("id")) {
			auto id = new QString(json.value("id").toString());
			m_id = id;
		}
		if(json.contains("method")) {
			auto method = new QString(json.value("method").toString());
			m_method = method;
		}

	}
};
typedef DocumentSymbolClientCapabilities TextDocumentClientCapabilitiesDocumentSymbol;
const QString MethodLogTrace = "$/logTrace";
const int CompletionItemKindFolder = 19;
typedef SymbolKindCapabilities WorkspaceClientCapabilitiesSymbolKind;
class HoverParams:  public TextDocumentPositionParams, public WorkDoneProgressParams {

public:
	HoverParams() = default;
	virtual ~HoverParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);

	}
};
const QString MethodTextDocumentDidOpen = "textDocument/didOpen";
class ServerCapabilitiesWorkspaceFolders: virtual public GoPlsParams {
	bool *m_supported = nullptr;
	QJsonValue *m_changeNotifications = nullptr;

public:
	ServerCapabilitiesWorkspaceFolders() = default;
	virtual ~ServerCapabilitiesWorkspaceFolders() {
		if(m_supported) { delete m_supported; }
		if(m_changeNotifications) { delete m_changeNotifications; }
	}

	void setSupported(bool *supported) { m_supported = supported; }
	bool *getSupported() { return m_supported; }
	void setChangeNotifications(QJsonValue *changeNotifications) { m_changeNotifications = changeNotifications; }
	QJsonValue *getChangeNotifications() { return m_changeNotifications; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_supported) {
			QJsonValue supported = QJsonValue::fromVariant(QVariant::fromValue(*m_supported));
			obj.insert("supported", supported);
		}
		if(m_changeNotifications) {
			QJsonValue changeNotifications = *m_changeNotifications;
			obj.insert("changeNotifications", changeNotifications);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("supported")) {
			auto supported = new bool(json.value("supported").toVariant().value<bool>());
			m_supported = supported;
		}
		if(json.contains("changeNotifications")) {
			auto changeNotifications = new QJsonValue(json.value("changeNotifications"));
			m_changeNotifications = changeNotifications;
		}

	}
};
typedef QString FileOperationPatternKind;
class FileOperationPatternOptions: virtual public GoPlsParams {
	bool *m_ignoreCase = nullptr;

public:
	FileOperationPatternOptions() = default;
	virtual ~FileOperationPatternOptions() {
		if(m_ignoreCase) { delete m_ignoreCase; }
	}

	void setIgnoreCase(bool *ignoreCase) { m_ignoreCase = ignoreCase; }
	bool *getIgnoreCase() { return m_ignoreCase; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_ignoreCase) {
			QJsonValue ignoreCase = QJsonValue::fromVariant(QVariant::fromValue(*m_ignoreCase));
			obj.insert("ignoreCase", ignoreCase);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("ignoreCase")) {
			auto ignoreCase = new bool(json.value("ignoreCase").toVariant().value<bool>());
			m_ignoreCase = ignoreCase;
		}

	}
};
class FileOperationPattern: virtual public GoPlsParams {
	QString *m_glob = nullptr;
	FileOperationPatternKind *m_matches = nullptr;
	FileOperationPatternOptions *m_options = nullptr;

public:
	FileOperationPattern() = default;
	virtual ~FileOperationPattern() {
		if(m_glob) { delete m_glob; }
		if(m_matches) { delete m_matches; }
		if(m_options) { delete m_options; }
	}

	void setGlob(QString *glob) { m_glob = glob; }
	QString *getGlob() { return m_glob; }
	void setMatches(FileOperationPatternKind *matches) { m_matches = matches; }
	FileOperationPatternKind *getMatches() { return m_matches; }
	void setOptions(FileOperationPatternOptions *options) { m_options = options; }
	FileOperationPatternOptions *getOptions() { return m_options; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_glob) {
			QJsonValue glob = QJsonValue::fromVariant(QVariant::fromValue(*m_glob));
			obj.insert("glob", glob);
		}
		if(m_matches) {
			QJsonValue matches = QJsonValue::fromVariant(QVariant::fromValue(*m_matches));
			obj.insert("matches", matches);
		}
		if(m_options) {
			QJsonObject options = m_options->toJson();
			obj.insert("options", options);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("glob")) {
			auto glob = new QString(json.value("glob").toString());
			m_glob = glob;
		}
		if(json.contains("matches")) {
			auto matches = new QString(json.value("matches").toString());
			m_matches = matches;
		}
		if(json.contains("options")) {
			auto options = new FileOperationPatternOptions();
			options->fromJson(json.value("options").toObject());
			m_options = options;
		}

	}
};
class FileOperationFilter: virtual public GoPlsParams {
	QString *m_scheme = nullptr;
	FileOperationPattern *m_pattern = nullptr;

public:
	FileOperationFilter() = default;
	virtual ~FileOperationFilter() {
		if(m_scheme) { delete m_scheme; }
		if(m_pattern) { delete m_pattern; }
	}

	void setScheme(QString *scheme) { m_scheme = scheme; }
	QString *getScheme() { return m_scheme; }
	void setPattern(FileOperationPattern *pattern) { m_pattern = pattern; }
	FileOperationPattern *getPattern() { return m_pattern; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_scheme) {
			QJsonValue scheme = QJsonValue::fromVariant(QVariant::fromValue(*m_scheme));
			obj.insert("scheme", scheme);
		}
		if(m_pattern) {
			QJsonObject pattern = m_pattern->toJson();
			obj.insert("pattern", pattern);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("scheme")) {
			auto scheme = new QString(json.value("scheme").toString());
			m_scheme = scheme;
		}
		if(json.contains("pattern")) {
			auto pattern = new FileOperationPattern();
			pattern->fromJson(json.value("pattern").toObject());
			m_pattern = pattern;
		}

	}
};
class FileOperationRegistrationOptions: virtual public GoPlsParams {
	QList<FileOperationFilter*> *m_filters = nullptr;

public:
	FileOperationRegistrationOptions() = default;
	virtual ~FileOperationRegistrationOptions() {
		if(m_filters) { qDeleteAll(*m_filters); delete m_filters; }
	}

	void setFilters(QList<FileOperationFilter*> *filters) { m_filters = filters; }
	QList<FileOperationFilter*> *getFilters() { return m_filters; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_filters) {
				QJsonArray filters;
			for(auto it = m_filters->cbegin(); it != m_filters->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				filters.append(item);
			}

			obj.insert("filters", filters);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("filters")) {
			 QJsonArray arr = json.value("filters").toArray();
			auto filters = new QList<FileOperationFilter*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new FileOperationFilter();
			item->fromJson((m_item).toObject());
				filters->append(item);
			}
			m_filters = filters;
		}

	}
};
class ServerCapabilitiesWorkspaceFileOperations: virtual public GoPlsParams {
	FileOperationRegistrationOptions *m_didCreate = nullptr;
	FileOperationRegistrationOptions *m_willCreate = nullptr;
	FileOperationRegistrationOptions *m_didRename = nullptr;
	FileOperationRegistrationOptions *m_willRename = nullptr;
	FileOperationRegistrationOptions *m_didDelete = nullptr;
	FileOperationRegistrationOptions *m_willDelete = nullptr;

public:
	ServerCapabilitiesWorkspaceFileOperations() = default;
	virtual ~ServerCapabilitiesWorkspaceFileOperations() {
		if(m_didCreate) { delete m_didCreate; }
		if(m_willCreate) { delete m_willCreate; }
		if(m_didRename) { delete m_didRename; }
		if(m_willRename) { delete m_willRename; }
		if(m_didDelete) { delete m_didDelete; }
		if(m_willDelete) { delete m_willDelete; }
	}

	void setDidCreate(FileOperationRegistrationOptions *didCreate) { m_didCreate = didCreate; }
	FileOperationRegistrationOptions *getDidCreate() { return m_didCreate; }
	void setWillCreate(FileOperationRegistrationOptions *willCreate) { m_willCreate = willCreate; }
	FileOperationRegistrationOptions *getWillCreate() { return m_willCreate; }
	void setDidRename(FileOperationRegistrationOptions *didRename) { m_didRename = didRename; }
	FileOperationRegistrationOptions *getDidRename() { return m_didRename; }
	void setWillRename(FileOperationRegistrationOptions *willRename) { m_willRename = willRename; }
	FileOperationRegistrationOptions *getWillRename() { return m_willRename; }
	void setDidDelete(FileOperationRegistrationOptions *didDelete) { m_didDelete = didDelete; }
	FileOperationRegistrationOptions *getDidDelete() { return m_didDelete; }
	void setWillDelete(FileOperationRegistrationOptions *willDelete) { m_willDelete = willDelete; }
	FileOperationRegistrationOptions *getWillDelete() { return m_willDelete; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_didCreate) {
			QJsonObject didCreate = m_didCreate->toJson();
			obj.insert("didCreate", didCreate);
		}
		if(m_willCreate) {
			QJsonObject willCreate = m_willCreate->toJson();
			obj.insert("willCreate", willCreate);
		}
		if(m_didRename) {
			QJsonObject didRename = m_didRename->toJson();
			obj.insert("didRename", didRename);
		}
		if(m_willRename) {
			QJsonObject willRename = m_willRename->toJson();
			obj.insert("willRename", willRename);
		}
		if(m_didDelete) {
			QJsonObject didDelete = m_didDelete->toJson();
			obj.insert("didDelete", didDelete);
		}
		if(m_willDelete) {
			QJsonObject willDelete = m_willDelete->toJson();
			obj.insert("willDelete", willDelete);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("didCreate")) {
			auto didCreate = new FileOperationRegistrationOptions();
			didCreate->fromJson(json.value("didCreate").toObject());
			m_didCreate = didCreate;
		}
		if(json.contains("willCreate")) {
			auto willCreate = new FileOperationRegistrationOptions();
			willCreate->fromJson(json.value("willCreate").toObject());
			m_willCreate = willCreate;
		}
		if(json.contains("didRename")) {
			auto didRename = new FileOperationRegistrationOptions();
			didRename->fromJson(json.value("didRename").toObject());
			m_didRename = didRename;
		}
		if(json.contains("willRename")) {
			auto willRename = new FileOperationRegistrationOptions();
			willRename->fromJson(json.value("willRename").toObject());
			m_willRename = willRename;
		}
		if(json.contains("didDelete")) {
			auto didDelete = new FileOperationRegistrationOptions();
			didDelete->fromJson(json.value("didDelete").toObject());
			m_didDelete = didDelete;
		}
		if(json.contains("willDelete")) {
			auto willDelete = new FileOperationRegistrationOptions();
			willDelete->fromJson(json.value("willDelete").toObject());
			m_willDelete = willDelete;
		}

	}
};
class ServerCapabilitiesWorkspace: virtual public GoPlsParams {
	ServerCapabilitiesWorkspaceFolders *m_workspaceFolders = nullptr;
	ServerCapabilitiesWorkspaceFileOperations *m_fileOperations = nullptr;

public:
	ServerCapabilitiesWorkspace() = default;
	virtual ~ServerCapabilitiesWorkspace() {
		if(m_workspaceFolders) { delete m_workspaceFolders; }
		if(m_fileOperations) { delete m_fileOperations; }
	}

	void setWorkspaceFolders(ServerCapabilitiesWorkspaceFolders *workspaceFolders) { m_workspaceFolders = workspaceFolders; }
	ServerCapabilitiesWorkspaceFolders *getWorkspaceFolders() { return m_workspaceFolders; }
	void setFileOperations(ServerCapabilitiesWorkspaceFileOperations *fileOperations) { m_fileOperations = fileOperations; }
	ServerCapabilitiesWorkspaceFileOperations *getFileOperations() { return m_fileOperations; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_workspaceFolders) {
			QJsonObject workspaceFolders = m_workspaceFolders->toJson();
			obj.insert("workspaceFolders", workspaceFolders);
		}
		if(m_fileOperations) {
			QJsonObject fileOperations = m_fileOperations->toJson();
			obj.insert("fileOperations", fileOperations);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("workspaceFolders")) {
			auto workspaceFolders = new ServerCapabilitiesWorkspaceFolders();
			workspaceFolders->fromJson(json.value("workspaceFolders").toObject());
			m_workspaceFolders = workspaceFolders;
		}
		if(json.contains("fileOperations")) {
			auto fileOperations = new ServerCapabilitiesWorkspaceFileOperations();
			fileOperations->fromJson(json.value("fileOperations").toObject());
			m_fileOperations = fileOperations;
		}

	}
};
const int DiagnosticSeverityError = 1;
const int SymbolKindFile = 1;
const int DocumentHighlightKindText = 1;
typedef QList<Position*> Positions;
const QString MethodTextDocumentDidChange = "textDocument/didChange";
class UnregistrationParams: virtual public GoPlsParams {
	QList<Unregistration*> *m_unregisterations = nullptr;

public:
	UnregistrationParams() = default;
	virtual ~UnregistrationParams() {
		if(m_unregisterations) { qDeleteAll(*m_unregisterations); delete m_unregisterations; }
	}

	void setUnregisterations(QList<Unregistration*> *unregisterations) { m_unregisterations = unregisterations; }
	QList<Unregistration*> *getUnregisterations() { return m_unregisterations; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_unregisterations) {
				QJsonArray unregisterations;
			for(auto it = m_unregisterations->cbegin(); it != m_unregisterations->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				unregisterations.append(item);
			}

			obj.insert("unregisterations", unregisterations);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("unregisterations")) {
			 QJsonArray arr = json.value("unregisterations").toArray();
			auto unregisterations = new QList<Unregistration*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new Unregistration();
			item->fromJson((m_item).toObject());
				unregisterations->append(item);
			}
			m_unregisterations = unregisterations;
		}

	}
};
class DeleteFileOptions: virtual public GoPlsParams {
	bool *m_recursive = nullptr;
	bool *m_ignoreIfNotExists = nullptr;

public:
	DeleteFileOptions() = default;
	virtual ~DeleteFileOptions() {
		if(m_recursive) { delete m_recursive; }
		if(m_ignoreIfNotExists) { delete m_ignoreIfNotExists; }
	}

	void setRecursive(bool *recursive) { m_recursive = recursive; }
	bool *getRecursive() { return m_recursive; }
	void setIgnoreIfNotExists(bool *ignoreIfNotExists) { m_ignoreIfNotExists = ignoreIfNotExists; }
	bool *getIgnoreIfNotExists() { return m_ignoreIfNotExists; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_recursive) {
			QJsonValue recursive = QJsonValue::fromVariant(QVariant::fromValue(*m_recursive));
			obj.insert("recursive", recursive);
		}
		if(m_ignoreIfNotExists) {
			QJsonValue ignoreIfNotExists = QJsonValue::fromVariant(QVariant::fromValue(*m_ignoreIfNotExists));
			obj.insert("ignoreIfNotExists", ignoreIfNotExists);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("recursive")) {
			auto recursive = new bool(json.value("recursive").toVariant().value<bool>());
			m_recursive = recursive;
		}
		if(json.contains("ignoreIfNotExists")) {
			auto ignoreIfNotExists = new bool(json.value("ignoreIfNotExists").toVariant().value<bool>());
			m_ignoreIfNotExists = ignoreIfNotExists;
		}

	}
};
class TypeDefinitionOptions:  public WorkDoneProgressOptions {

public:
	TypeDefinitionOptions() = default;
	virtual ~TypeDefinitionOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
const int CompletionItemKindEvent = 23;
const int SymbolKindNull = 21;
typedef ReferenceParams ReferencesParams;
const QString MethodTextDocumentDefinition = "textDocument/definition";
class DeleteFile: virtual public GoPlsParams {
	ResourceOperationKind *m_kind = nullptr;
	DocumentURI *m_uri = nullptr;
	DeleteFileOptions *m_options = nullptr;
	ChangeAnnotationIdentifier *m_annotationId = nullptr;

public:
	DeleteFile() = default;
	virtual ~DeleteFile() {
		if(m_kind) { delete m_kind; }
		if(m_uri) { delete m_uri; }
		if(m_options) { delete m_options; }
		if(m_annotationId) { delete m_annotationId; }
	}

	void setKind(ResourceOperationKind *kind) { m_kind = kind; }
	ResourceOperationKind *getKind() { return m_kind; }
	void setUri(DocumentURI *uri) { m_uri = uri; }
	DocumentURI *getUri() { return m_uri; }
	void setOptions(DeleteFileOptions *options) { m_options = options; }
	DeleteFileOptions *getOptions() { return m_options; }
	void setAnnotationId(ChangeAnnotationIdentifier *annotationId) { m_annotationId = annotationId; }
	ChangeAnnotationIdentifier *getAnnotationId() { return m_annotationId; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}
		if(m_options) {
			QJsonObject options = m_options->toJson();
			obj.insert("options", options);
		}
		if(m_annotationId) {
			QJsonValue annotationId = QJsonValue::fromVariant(QVariant::fromValue(*m_annotationId));
			obj.insert("annotationId", annotationId);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("kind")) {
			auto kind = new QString(json.value("kind").toString());
			m_kind = kind;
		}
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}
		if(json.contains("options")) {
			auto options = new DeleteFileOptions();
			options->fromJson(json.value("options").toObject());
			m_options = options;
		}
		if(json.contains("annotationId")) {
			auto annotationId = new QString(json.value("annotationId").toString());
			m_annotationId = annotationId;
		}

	}
};
typedef QHash<QString,QString*> StringStringMap;
class ServerInfo: virtual public GoPlsParams {
	QString *m_name = nullptr;
	QString *m_version = nullptr;

public:
	ServerInfo() = default;
	virtual ~ServerInfo() {
		if(m_name) { delete m_name; }
		if(m_version) { delete m_version; }
	}

	void setName(QString *name) { m_name = name; }
	QString *getName() { return m_name; }
	void setVersion(QString *version) { m_version = version; }
	QString *getVersion() { return m_version; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_name) {
			QJsonValue name = QJsonValue::fromVariant(QVariant::fromValue(*m_name));
			obj.insert("name", name);
		}
		if(m_version) {
			QJsonValue version = QJsonValue::fromVariant(QVariant::fromValue(*m_version));
			obj.insert("version", version);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("name")) {
			auto name = new QString(json.value("name").toString());
			m_name = name;
		}
		if(json.contains("version")) {
			auto version = new QString(json.value("version").toString());
			m_version = version;
		}

	}
};
const QString MethodWillCreateFiles = "workspace/willCreateFiles";
typedef DefinitionTextDocumentClientCapabilities TextDocumentClientCapabilitiesDefinition;
typedef QList<Range*> Ranges;
const int CompletionItemKindUnit = 11;
const int SymbolKindObject = 19;
typedef QJsonValue Client;
const int WatchKindCreate = 1;
typedef QString SemanticTokenModifiers;
class DeclarationOptions:  public WorkDoneProgressOptions {

public:
	DeclarationOptions() = default;
	virtual ~DeclarationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
class MonikerOptions:  public WorkDoneProgressOptions {

public:
	MonikerOptions() = default;
	virtual ~MonikerOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
class MonikerRegistrationOptions:  public TextDocumentRegistrationOptions, public MonikerOptions {

public:
	MonikerRegistrationOptions() = default;
	virtual ~MonikerRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject MonikerOptions_json = MonikerOptions::toJson();
		for(const QString &key: MonikerOptions_json.keys()) {
			obj.insert(key, MonikerOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		MonikerOptions::fromJson(json);

	}
};
typedef SelectionRangeClientCapabilities TextDocumentClientCapabilitiesSelectionRange;
const QString MethodCodeLensResolve = "codeLens/resolve";
typedef QString LanguageIdentifier;
class TextDocumentItem: virtual public GoPlsParams {
	DocumentURI *m_uri = nullptr;
	LanguageIdentifier *m_languageId = nullptr;
	int *m_version = nullptr;
	QString *m_text = nullptr;

public:
	TextDocumentItem() = default;
	virtual ~TextDocumentItem() {
		if(m_uri) { delete m_uri; }
		if(m_languageId) { delete m_languageId; }
		if(m_version) { delete m_version; }
		if(m_text) { delete m_text; }
	}

	void setUri(DocumentURI *uri) { m_uri = uri; }
	DocumentURI *getUri() { return m_uri; }
	void setLanguageId(LanguageIdentifier *languageId) { m_languageId = languageId; }
	LanguageIdentifier *getLanguageId() { return m_languageId; }
	void setVersion(int *version) { m_version = version; }
	int *getVersion() { return m_version; }
	void setText(QString *text) { m_text = text; }
	QString *getText() { return m_text; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}
		if(m_languageId) {
			QJsonValue languageId = QJsonValue::fromVariant(QVariant::fromValue(*m_languageId));
			obj.insert("languageId", languageId);
		}
		if(m_version) {
			QJsonValue version = QJsonValue::fromVariant(QVariant::fromValue(*m_version));
			obj.insert("version", version);
		}
		if(m_text) {
			QJsonValue text = QJsonValue::fromVariant(QVariant::fromValue(*m_text));
			obj.insert("text", text);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}
		if(json.contains("languageId")) {
			auto languageId = new QString(json.value("languageId").toString());
			m_languageId = languageId;
		}
		if(json.contains("version")) {
			auto version = new int(json.value("version").toVariant().value<int>());
			m_version = version;
		}
		if(json.contains("text")) {
			auto text = new QString(json.value("text").toString());
			m_text = text;
		}

	}
};
class DidOpenTextDocumentParams: virtual public GoPlsParams {
	TextDocumentItem *m_textDocument = nullptr;

public:
	DidOpenTextDocumentParams() = default;
	virtual ~DidOpenTextDocumentParams() {
		if(m_textDocument) { delete m_textDocument; }
	}

	void setTextDocument(TextDocumentItem *textDocument) { m_textDocument = textDocument; }
	TextDocumentItem *getTextDocument() { return m_textDocument; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentItem();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}

	}
};
class DocumentHighlightParams:  public TextDocumentPositionParams, public WorkDoneProgressParams, public PartialResultParams {

public:
	DocumentHighlightParams() = default;
	virtual ~DocumentHighlightParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);

	}
};
class Color: virtual public GoPlsParams {
	double *m_alpha = nullptr;
	double *m_blue = nullptr;
	double *m_green = nullptr;
	double *m_red = nullptr;

public:
	Color() = default;
	virtual ~Color() {
		if(m_alpha) { delete m_alpha; }
		if(m_blue) { delete m_blue; }
		if(m_green) { delete m_green; }
		if(m_red) { delete m_red; }
	}

	void setAlpha(double *alpha) { m_alpha = alpha; }
	double *getAlpha() { return m_alpha; }
	void setBlue(double *blue) { m_blue = blue; }
	double *getBlue() { return m_blue; }
	void setGreen(double *green) { m_green = green; }
	double *getGreen() { return m_green; }
	void setRed(double *red) { m_red = red; }
	double *getRed() { return m_red; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_alpha) {
			QJsonValue alpha = QJsonValue::fromVariant(QVariant::fromValue(*m_alpha));
			obj.insert("alpha", alpha);
		}
		if(m_blue) {
			QJsonValue blue = QJsonValue::fromVariant(QVariant::fromValue(*m_blue));
			obj.insert("blue", blue);
		}
		if(m_green) {
			QJsonValue green = QJsonValue::fromVariant(QVariant::fromValue(*m_green));
			obj.insert("green", green);
		}
		if(m_red) {
			QJsonValue red = QJsonValue::fromVariant(QVariant::fromValue(*m_red));
			obj.insert("red", red);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("alpha")) {
			auto alpha = new double(json.value("alpha").toVariant().value<double>());
			m_alpha = alpha;
		}
		if(json.contains("blue")) {
			auto blue = new double(json.value("blue").toVariant().value<double>());
			m_blue = blue;
		}
		if(json.contains("green")) {
			auto green = new double(json.value("green").toVariant().value<double>());
			m_green = green;
		}
		if(json.contains("red")) {
			auto red = new double(json.value("red").toVariant().value<double>());
			m_red = red;
		}

	}
};
class TextDocumentChangeRegistrationOptions:  public TextDocumentRegistrationOptions {
	TextDocumentSyncKind *m_syncKind = nullptr;

public:
	TextDocumentChangeRegistrationOptions() = default;
	virtual ~TextDocumentChangeRegistrationOptions() {
		if(m_syncKind) { delete m_syncKind; }
	}

	void setSyncKind(TextDocumentSyncKind *syncKind) { m_syncKind = syncKind; }
	TextDocumentSyncKind *getSyncKind() { return m_syncKind; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		if(m_syncKind) {
			QJsonValue syncKind = QJsonValue::fromVariant(QVariant::fromValue(*m_syncKind));
			obj.insert("syncKind", syncKind);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		if(json.contains("syncKind")) {
			auto syncKind = new double(json.value("syncKind").toVariant().value<double>());
			m_syncKind = syncKind;
		}

	}
};
const QString ObjectiveCppLanguage = "objective-cpp";
const QString TypeScriptLanguage = "typescript";
class CompletionOptions: virtual public GoPlsParams {
	bool *m_resolveProvider = nullptr;
	QList<QString*> *m_triggerCharacters = nullptr;

public:
	CompletionOptions() = default;
	virtual ~CompletionOptions() {
		if(m_resolveProvider) { delete m_resolveProvider; }
		if(m_triggerCharacters) { qDeleteAll(*m_triggerCharacters); delete m_triggerCharacters; }
	}

	void setResolveProvider(bool *resolveProvider) { m_resolveProvider = resolveProvider; }
	bool *getResolveProvider() { return m_resolveProvider; }
	void setTriggerCharacters(QList<QString*> *triggerCharacters) { m_triggerCharacters = triggerCharacters; }
	QList<QString*> *getTriggerCharacters() { return m_triggerCharacters; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_resolveProvider) {
			QJsonValue resolveProvider = QJsonValue::fromVariant(QVariant::fromValue(*m_resolveProvider));
			obj.insert("resolveProvider", resolveProvider);
		}
		if(m_triggerCharacters) {
				QJsonArray triggerCharacters;
			for(auto it = m_triggerCharacters->cbegin(); it != m_triggerCharacters->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				triggerCharacters.append(item);
			}

			obj.insert("triggerCharacters", triggerCharacters);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("resolveProvider")) {
			auto resolveProvider = new bool(json.value("resolveProvider").toVariant().value<bool>());
			m_resolveProvider = resolveProvider;
		}
		if(json.contains("triggerCharacters")) {
			 QJsonArray arr = json.value("triggerCharacters").toArray();
			auto triggerCharacters = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				triggerCharacters->append(item);
			}
			m_triggerCharacters = triggerCharacters;
		}

	}
};
class CodeLensOptions: virtual public GoPlsParams {
	bool *m_resolveProvider = nullptr;

public:
	CodeLensOptions() = default;
	virtual ~CodeLensOptions() {
		if(m_resolveProvider) { delete m_resolveProvider; }
	}

	void setResolveProvider(bool *resolveProvider) { m_resolveProvider = resolveProvider; }
	bool *getResolveProvider() { return m_resolveProvider; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_resolveProvider) {
			QJsonValue resolveProvider = QJsonValue::fromVariant(QVariant::fromValue(*m_resolveProvider));
			obj.insert("resolveProvider", resolveProvider);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("resolveProvider")) {
			auto resolveProvider = new bool(json.value("resolveProvider").toVariant().value<bool>());
			m_resolveProvider = resolveProvider;
		}

	}
};
class DocumentLinkOptions: virtual public GoPlsParams {
	bool *m_resolveProvider = nullptr;

public:
	DocumentLinkOptions() = default;
	virtual ~DocumentLinkOptions() {
		if(m_resolveProvider) { delete m_resolveProvider; }
	}

	void setResolveProvider(bool *resolveProvider) { m_resolveProvider = resolveProvider; }
	bool *getResolveProvider() { return m_resolveProvider; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_resolveProvider) {
			QJsonValue resolveProvider = QJsonValue::fromVariant(QVariant::fromValue(*m_resolveProvider));
			obj.insert("resolveProvider", resolveProvider);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("resolveProvider")) {
			auto resolveProvider = new bool(json.value("resolveProvider").toVariant().value<bool>());
			m_resolveProvider = resolveProvider;
		}

	}
};
class DocumentOnTypeFormattingOptions: virtual public GoPlsParams {
	QString *m_firstTriggerCharacter = nullptr;
	QList<QString*> *m_moreTriggerCharacter = nullptr;

public:
	DocumentOnTypeFormattingOptions() = default;
	virtual ~DocumentOnTypeFormattingOptions() {
		if(m_firstTriggerCharacter) { delete m_firstTriggerCharacter; }
		if(m_moreTriggerCharacter) { qDeleteAll(*m_moreTriggerCharacter); delete m_moreTriggerCharacter; }
	}

	void setFirstTriggerCharacter(QString *firstTriggerCharacter) { m_firstTriggerCharacter = firstTriggerCharacter; }
	QString *getFirstTriggerCharacter() { return m_firstTriggerCharacter; }
	void setMoreTriggerCharacter(QList<QString*> *moreTriggerCharacter) { m_moreTriggerCharacter = moreTriggerCharacter; }
	QList<QString*> *getMoreTriggerCharacter() { return m_moreTriggerCharacter; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_firstTriggerCharacter) {
			QJsonValue firstTriggerCharacter = QJsonValue::fromVariant(QVariant::fromValue(*m_firstTriggerCharacter));
			obj.insert("firstTriggerCharacter", firstTriggerCharacter);
		}
		if(m_moreTriggerCharacter) {
				QJsonArray moreTriggerCharacter;
			for(auto it = m_moreTriggerCharacter->cbegin(); it != m_moreTriggerCharacter->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				moreTriggerCharacter.append(item);
			}

			obj.insert("moreTriggerCharacter", moreTriggerCharacter);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("firstTriggerCharacter")) {
			auto firstTriggerCharacter = new QString(json.value("firstTriggerCharacter").toString());
			m_firstTriggerCharacter = firstTriggerCharacter;
		}
		if(json.contains("moreTriggerCharacter")) {
			 QJsonArray arr = json.value("moreTriggerCharacter").toArray();
			auto moreTriggerCharacter = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				moreTriggerCharacter->append(item);
			}
			m_moreTriggerCharacter = moreTriggerCharacter;
		}

	}
};
class ExecuteCommandOptions: virtual public GoPlsParams {
	QList<QString*> *m_commands = nullptr;

public:
	ExecuteCommandOptions() = default;
	virtual ~ExecuteCommandOptions() {
		if(m_commands) { qDeleteAll(*m_commands); delete m_commands; }
	}

	void setCommands(QList<QString*> *commands) { m_commands = commands; }
	QList<QString*> *getCommands() { return m_commands; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_commands) {
				QJsonArray commands;
			for(auto it = m_commands->cbegin(); it != m_commands->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				commands.append(item);
			}

			obj.insert("commands", commands);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("commands")) {
			 QJsonArray arr = json.value("commands").toArray();
			auto commands = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				commands->append(item);
			}
			m_commands = commands;
		}

	}
};
class ServerCapabilities: virtual public GoPlsParams {
	QJsonValue *m_textDocumentSync = nullptr;
	CompletionOptions *m_completionProvider = nullptr;
	QJsonValue *m_hoverProvider = nullptr;
	SignatureHelpOptions *m_signatureHelpProvider = nullptr;
	QJsonValue *m_declarationProvider = nullptr;
	QJsonValue *m_definitionProvider = nullptr;
	QJsonValue *m_typeDefinitionProvider = nullptr;
	QJsonValue *m_implementationProvider = nullptr;
	QJsonValue *m_referencesProvider = nullptr;
	QJsonValue *m_documentHighlightProvider = nullptr;
	QJsonValue *m_documentSymbolProvider = nullptr;
	QJsonValue *m_codeActionProvider = nullptr;
	CodeLensOptions *m_codeLensProvider = nullptr;
	DocumentLinkOptions *m_documentLinkProvider = nullptr;
	QJsonValue *m_colorProvider = nullptr;
	QJsonValue *m_workspaceSymbolProvider = nullptr;
	QJsonValue *m_documentFormattingProvider = nullptr;
	QJsonValue *m_documentRangeFormattingProvider = nullptr;
	DocumentOnTypeFormattingOptions *m_documentOnTypeFormattingProvider = nullptr;
	QJsonValue *m_renameProvider = nullptr;
	QJsonValue *m_foldingRangeProvider = nullptr;
	QJsonValue *m_selectionRangeProvider = nullptr;
	ExecuteCommandOptions *m_executeCommandProvider = nullptr;
	QJsonValue *m_callHierarchyProvider = nullptr;
	QJsonValue *m_linkedEditingRangeProvider = nullptr;
	QJsonValue *m_semanticTokensProvider = nullptr;
	ServerCapabilitiesWorkspace *m_workspace = nullptr;
	QJsonValue *m_monikerProvider = nullptr;
	QJsonValue *m_experimental = nullptr;

public:
	ServerCapabilities() = default;
	virtual ~ServerCapabilities() {
		if(m_textDocumentSync) { delete m_textDocumentSync; }
		if(m_completionProvider) { delete m_completionProvider; }
		if(m_hoverProvider) { delete m_hoverProvider; }
		if(m_signatureHelpProvider) { delete m_signatureHelpProvider; }
		if(m_declarationProvider) { delete m_declarationProvider; }
		if(m_definitionProvider) { delete m_definitionProvider; }
		if(m_typeDefinitionProvider) { delete m_typeDefinitionProvider; }
		if(m_implementationProvider) { delete m_implementationProvider; }
		if(m_referencesProvider) { delete m_referencesProvider; }
		if(m_documentHighlightProvider) { delete m_documentHighlightProvider; }
		if(m_documentSymbolProvider) { delete m_documentSymbolProvider; }
		if(m_codeActionProvider) { delete m_codeActionProvider; }
		if(m_codeLensProvider) { delete m_codeLensProvider; }
		if(m_documentLinkProvider) { delete m_documentLinkProvider; }
		if(m_colorProvider) { delete m_colorProvider; }
		if(m_workspaceSymbolProvider) { delete m_workspaceSymbolProvider; }
		if(m_documentFormattingProvider) { delete m_documentFormattingProvider; }
		if(m_documentRangeFormattingProvider) { delete m_documentRangeFormattingProvider; }
		if(m_documentOnTypeFormattingProvider) { delete m_documentOnTypeFormattingProvider; }
		if(m_renameProvider) { delete m_renameProvider; }
		if(m_foldingRangeProvider) { delete m_foldingRangeProvider; }
		if(m_selectionRangeProvider) { delete m_selectionRangeProvider; }
		if(m_executeCommandProvider) { delete m_executeCommandProvider; }
		if(m_callHierarchyProvider) { delete m_callHierarchyProvider; }
		if(m_linkedEditingRangeProvider) { delete m_linkedEditingRangeProvider; }
		if(m_semanticTokensProvider) { delete m_semanticTokensProvider; }
		if(m_workspace) { delete m_workspace; }
		if(m_monikerProvider) { delete m_monikerProvider; }
		if(m_experimental) { delete m_experimental; }
	}

	void setTextDocumentSync(QJsonValue *textDocumentSync) { m_textDocumentSync = textDocumentSync; }
	QJsonValue *getTextDocumentSync() { return m_textDocumentSync; }
	void setCompletionProvider(CompletionOptions *completionProvider) { m_completionProvider = completionProvider; }
	CompletionOptions *getCompletionProvider() { return m_completionProvider; }
	void setHoverProvider(QJsonValue *hoverProvider) { m_hoverProvider = hoverProvider; }
	QJsonValue *getHoverProvider() { return m_hoverProvider; }
	void setSignatureHelpProvider(SignatureHelpOptions *signatureHelpProvider) { m_signatureHelpProvider = signatureHelpProvider; }
	SignatureHelpOptions *getSignatureHelpProvider() { return m_signatureHelpProvider; }
	void setDeclarationProvider(QJsonValue *declarationProvider) { m_declarationProvider = declarationProvider; }
	QJsonValue *getDeclarationProvider() { return m_declarationProvider; }
	void setDefinitionProvider(QJsonValue *definitionProvider) { m_definitionProvider = definitionProvider; }
	QJsonValue *getDefinitionProvider() { return m_definitionProvider; }
	void setTypeDefinitionProvider(QJsonValue *typeDefinitionProvider) { m_typeDefinitionProvider = typeDefinitionProvider; }
	QJsonValue *getTypeDefinitionProvider() { return m_typeDefinitionProvider; }
	void setImplementationProvider(QJsonValue *implementationProvider) { m_implementationProvider = implementationProvider; }
	QJsonValue *getImplementationProvider() { return m_implementationProvider; }
	void setReferencesProvider(QJsonValue *referencesProvider) { m_referencesProvider = referencesProvider; }
	QJsonValue *getReferencesProvider() { return m_referencesProvider; }
	void setDocumentHighlightProvider(QJsonValue *documentHighlightProvider) { m_documentHighlightProvider = documentHighlightProvider; }
	QJsonValue *getDocumentHighlightProvider() { return m_documentHighlightProvider; }
	void setDocumentSymbolProvider(QJsonValue *documentSymbolProvider) { m_documentSymbolProvider = documentSymbolProvider; }
	QJsonValue *getDocumentSymbolProvider() { return m_documentSymbolProvider; }
	void setCodeActionProvider(QJsonValue *codeActionProvider) { m_codeActionProvider = codeActionProvider; }
	QJsonValue *getCodeActionProvider() { return m_codeActionProvider; }
	void setCodeLensProvider(CodeLensOptions *codeLensProvider) { m_codeLensProvider = codeLensProvider; }
	CodeLensOptions *getCodeLensProvider() { return m_codeLensProvider; }
	void setDocumentLinkProvider(DocumentLinkOptions *documentLinkProvider) { m_documentLinkProvider = documentLinkProvider; }
	DocumentLinkOptions *getDocumentLinkProvider() { return m_documentLinkProvider; }
	void setColorProvider(QJsonValue *colorProvider) { m_colorProvider = colorProvider; }
	QJsonValue *getColorProvider() { return m_colorProvider; }
	void setWorkspaceSymbolProvider(QJsonValue *workspaceSymbolProvider) { m_workspaceSymbolProvider = workspaceSymbolProvider; }
	QJsonValue *getWorkspaceSymbolProvider() { return m_workspaceSymbolProvider; }
	void setDocumentFormattingProvider(QJsonValue *documentFormattingProvider) { m_documentFormattingProvider = documentFormattingProvider; }
	QJsonValue *getDocumentFormattingProvider() { return m_documentFormattingProvider; }
	void setDocumentRangeFormattingProvider(QJsonValue *documentRangeFormattingProvider) { m_documentRangeFormattingProvider = documentRangeFormattingProvider; }
	QJsonValue *getDocumentRangeFormattingProvider() { return m_documentRangeFormattingProvider; }
	void setDocumentOnTypeFormattingProvider(DocumentOnTypeFormattingOptions *documentOnTypeFormattingProvider) { m_documentOnTypeFormattingProvider = documentOnTypeFormattingProvider; }
	DocumentOnTypeFormattingOptions *getDocumentOnTypeFormattingProvider() { return m_documentOnTypeFormattingProvider; }
	void setRenameProvider(QJsonValue *renameProvider) { m_renameProvider = renameProvider; }
	QJsonValue *getRenameProvider() { return m_renameProvider; }
	void setFoldingRangeProvider(QJsonValue *foldingRangeProvider) { m_foldingRangeProvider = foldingRangeProvider; }
	QJsonValue *getFoldingRangeProvider() { return m_foldingRangeProvider; }
	void setSelectionRangeProvider(QJsonValue *selectionRangeProvider) { m_selectionRangeProvider = selectionRangeProvider; }
	QJsonValue *getSelectionRangeProvider() { return m_selectionRangeProvider; }
	void setExecuteCommandProvider(ExecuteCommandOptions *executeCommandProvider) { m_executeCommandProvider = executeCommandProvider; }
	ExecuteCommandOptions *getExecuteCommandProvider() { return m_executeCommandProvider; }
	void setCallHierarchyProvider(QJsonValue *callHierarchyProvider) { m_callHierarchyProvider = callHierarchyProvider; }
	QJsonValue *getCallHierarchyProvider() { return m_callHierarchyProvider; }
	void setLinkedEditingRangeProvider(QJsonValue *linkedEditingRangeProvider) { m_linkedEditingRangeProvider = linkedEditingRangeProvider; }
	QJsonValue *getLinkedEditingRangeProvider() { return m_linkedEditingRangeProvider; }
	void setSemanticTokensProvider(QJsonValue *semanticTokensProvider) { m_semanticTokensProvider = semanticTokensProvider; }
	QJsonValue *getSemanticTokensProvider() { return m_semanticTokensProvider; }
	void setWorkspace(ServerCapabilitiesWorkspace *workspace) { m_workspace = workspace; }
	ServerCapabilitiesWorkspace *getWorkspace() { return m_workspace; }
	void setMonikerProvider(QJsonValue *monikerProvider) { m_monikerProvider = monikerProvider; }
	QJsonValue *getMonikerProvider() { return m_monikerProvider; }
	void setExperimental(QJsonValue *experimental) { m_experimental = experimental; }
	QJsonValue *getExperimental() { return m_experimental; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_textDocumentSync) {
			QJsonValue textDocumentSync = *m_textDocumentSync;
			obj.insert("textDocumentSync", textDocumentSync);
		}
		if(m_completionProvider) {
			QJsonObject completionProvider = m_completionProvider->toJson();
			obj.insert("completionProvider", completionProvider);
		}
		if(m_hoverProvider) {
			QJsonValue hoverProvider = *m_hoverProvider;
			obj.insert("hoverProvider", hoverProvider);
		}
		if(m_signatureHelpProvider) {
			QJsonObject signatureHelpProvider = m_signatureHelpProvider->toJson();
			obj.insert("signatureHelpProvider", signatureHelpProvider);
		}
		if(m_declarationProvider) {
			QJsonValue declarationProvider = *m_declarationProvider;
			obj.insert("declarationProvider", declarationProvider);
		}
		if(m_definitionProvider) {
			QJsonValue definitionProvider = *m_definitionProvider;
			obj.insert("definitionProvider", definitionProvider);
		}
		if(m_typeDefinitionProvider) {
			QJsonValue typeDefinitionProvider = *m_typeDefinitionProvider;
			obj.insert("typeDefinitionProvider", typeDefinitionProvider);
		}
		if(m_implementationProvider) {
			QJsonValue implementationProvider = *m_implementationProvider;
			obj.insert("implementationProvider", implementationProvider);
		}
		if(m_referencesProvider) {
			QJsonValue referencesProvider = *m_referencesProvider;
			obj.insert("referencesProvider", referencesProvider);
		}
		if(m_documentHighlightProvider) {
			QJsonValue documentHighlightProvider = *m_documentHighlightProvider;
			obj.insert("documentHighlightProvider", documentHighlightProvider);
		}
		if(m_documentSymbolProvider) {
			QJsonValue documentSymbolProvider = *m_documentSymbolProvider;
			obj.insert("documentSymbolProvider", documentSymbolProvider);
		}
		if(m_codeActionProvider) {
			QJsonValue codeActionProvider = *m_codeActionProvider;
			obj.insert("codeActionProvider", codeActionProvider);
		}
		if(m_codeLensProvider) {
			QJsonObject codeLensProvider = m_codeLensProvider->toJson();
			obj.insert("codeLensProvider", codeLensProvider);
		}
		if(m_documentLinkProvider) {
			QJsonObject documentLinkProvider = m_documentLinkProvider->toJson();
			obj.insert("documentLinkProvider", documentLinkProvider);
		}
		if(m_colorProvider) {
			QJsonValue colorProvider = *m_colorProvider;
			obj.insert("colorProvider", colorProvider);
		}
		if(m_workspaceSymbolProvider) {
			QJsonValue workspaceSymbolProvider = *m_workspaceSymbolProvider;
			obj.insert("workspaceSymbolProvider", workspaceSymbolProvider);
		}
		if(m_documentFormattingProvider) {
			QJsonValue documentFormattingProvider = *m_documentFormattingProvider;
			obj.insert("documentFormattingProvider", documentFormattingProvider);
		}
		if(m_documentRangeFormattingProvider) {
			QJsonValue documentRangeFormattingProvider = *m_documentRangeFormattingProvider;
			obj.insert("documentRangeFormattingProvider", documentRangeFormattingProvider);
		}
		if(m_documentOnTypeFormattingProvider) {
			QJsonObject documentOnTypeFormattingProvider = m_documentOnTypeFormattingProvider->toJson();
			obj.insert("documentOnTypeFormattingProvider", documentOnTypeFormattingProvider);
		}
		if(m_renameProvider) {
			QJsonValue renameProvider = *m_renameProvider;
			obj.insert("renameProvider", renameProvider);
		}
		if(m_foldingRangeProvider) {
			QJsonValue foldingRangeProvider = *m_foldingRangeProvider;
			obj.insert("foldingRangeProvider", foldingRangeProvider);
		}
		if(m_selectionRangeProvider) {
			QJsonValue selectionRangeProvider = *m_selectionRangeProvider;
			obj.insert("selectionRangeProvider", selectionRangeProvider);
		}
		if(m_executeCommandProvider) {
			QJsonObject executeCommandProvider = m_executeCommandProvider->toJson();
			obj.insert("executeCommandProvider", executeCommandProvider);
		}
		if(m_callHierarchyProvider) {
			QJsonValue callHierarchyProvider = *m_callHierarchyProvider;
			obj.insert("callHierarchyProvider", callHierarchyProvider);
		}
		if(m_linkedEditingRangeProvider) {
			QJsonValue linkedEditingRangeProvider = *m_linkedEditingRangeProvider;
			obj.insert("linkedEditingRangeProvider", linkedEditingRangeProvider);
		}
		if(m_semanticTokensProvider) {
			QJsonValue semanticTokensProvider = *m_semanticTokensProvider;
			obj.insert("semanticTokensProvider", semanticTokensProvider);
		}
		if(m_workspace) {
			QJsonObject workspace = m_workspace->toJson();
			obj.insert("workspace", workspace);
		}
		if(m_monikerProvider) {
			QJsonValue monikerProvider = *m_monikerProvider;
			obj.insert("monikerProvider", monikerProvider);
		}
		if(m_experimental) {
			QJsonValue experimental = *m_experimental;
			obj.insert("experimental", experimental);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("textDocumentSync")) {
			auto textDocumentSync = new QJsonValue(json.value("textDocumentSync"));
			m_textDocumentSync = textDocumentSync;
		}
		if(json.contains("completionProvider")) {
			auto completionProvider = new CompletionOptions();
			completionProvider->fromJson(json.value("completionProvider").toObject());
			m_completionProvider = completionProvider;
		}
		if(json.contains("hoverProvider")) {
			auto hoverProvider = new QJsonValue(json.value("hoverProvider"));
			m_hoverProvider = hoverProvider;
		}
		if(json.contains("signatureHelpProvider")) {
			auto signatureHelpProvider = new SignatureHelpOptions();
			signatureHelpProvider->fromJson(json.value("signatureHelpProvider").toObject());
			m_signatureHelpProvider = signatureHelpProvider;
		}
		if(json.contains("declarationProvider")) {
			auto declarationProvider = new QJsonValue(json.value("declarationProvider"));
			m_declarationProvider = declarationProvider;
		}
		if(json.contains("definitionProvider")) {
			auto definitionProvider = new QJsonValue(json.value("definitionProvider"));
			m_definitionProvider = definitionProvider;
		}
		if(json.contains("typeDefinitionProvider")) {
			auto typeDefinitionProvider = new QJsonValue(json.value("typeDefinitionProvider"));
			m_typeDefinitionProvider = typeDefinitionProvider;
		}
		if(json.contains("implementationProvider")) {
			auto implementationProvider = new QJsonValue(json.value("implementationProvider"));
			m_implementationProvider = implementationProvider;
		}
		if(json.contains("referencesProvider")) {
			auto referencesProvider = new QJsonValue(json.value("referencesProvider"));
			m_referencesProvider = referencesProvider;
		}
		if(json.contains("documentHighlightProvider")) {
			auto documentHighlightProvider = new QJsonValue(json.value("documentHighlightProvider"));
			m_documentHighlightProvider = documentHighlightProvider;
		}
		if(json.contains("documentSymbolProvider")) {
			auto documentSymbolProvider = new QJsonValue(json.value("documentSymbolProvider"));
			m_documentSymbolProvider = documentSymbolProvider;
		}
		if(json.contains("codeActionProvider")) {
			auto codeActionProvider = new QJsonValue(json.value("codeActionProvider"));
			m_codeActionProvider = codeActionProvider;
		}
		if(json.contains("codeLensProvider")) {
			auto codeLensProvider = new CodeLensOptions();
			codeLensProvider->fromJson(json.value("codeLensProvider").toObject());
			m_codeLensProvider = codeLensProvider;
		}
		if(json.contains("documentLinkProvider")) {
			auto documentLinkProvider = new DocumentLinkOptions();
			documentLinkProvider->fromJson(json.value("documentLinkProvider").toObject());
			m_documentLinkProvider = documentLinkProvider;
		}
		if(json.contains("colorProvider")) {
			auto colorProvider = new QJsonValue(json.value("colorProvider"));
			m_colorProvider = colorProvider;
		}
		if(json.contains("workspaceSymbolProvider")) {
			auto workspaceSymbolProvider = new QJsonValue(json.value("workspaceSymbolProvider"));
			m_workspaceSymbolProvider = workspaceSymbolProvider;
		}
		if(json.contains("documentFormattingProvider")) {
			auto documentFormattingProvider = new QJsonValue(json.value("documentFormattingProvider"));
			m_documentFormattingProvider = documentFormattingProvider;
		}
		if(json.contains("documentRangeFormattingProvider")) {
			auto documentRangeFormattingProvider = new QJsonValue(json.value("documentRangeFormattingProvider"));
			m_documentRangeFormattingProvider = documentRangeFormattingProvider;
		}
		if(json.contains("documentOnTypeFormattingProvider")) {
			auto documentOnTypeFormattingProvider = new DocumentOnTypeFormattingOptions();
			documentOnTypeFormattingProvider->fromJson(json.value("documentOnTypeFormattingProvider").toObject());
			m_documentOnTypeFormattingProvider = documentOnTypeFormattingProvider;
		}
		if(json.contains("renameProvider")) {
			auto renameProvider = new QJsonValue(json.value("renameProvider"));
			m_renameProvider = renameProvider;
		}
		if(json.contains("foldingRangeProvider")) {
			auto foldingRangeProvider = new QJsonValue(json.value("foldingRangeProvider"));
			m_foldingRangeProvider = foldingRangeProvider;
		}
		if(json.contains("selectionRangeProvider")) {
			auto selectionRangeProvider = new QJsonValue(json.value("selectionRangeProvider"));
			m_selectionRangeProvider = selectionRangeProvider;
		}
		if(json.contains("executeCommandProvider")) {
			auto executeCommandProvider = new ExecuteCommandOptions();
			executeCommandProvider->fromJson(json.value("executeCommandProvider").toObject());
			m_executeCommandProvider = executeCommandProvider;
		}
		if(json.contains("callHierarchyProvider")) {
			auto callHierarchyProvider = new QJsonValue(json.value("callHierarchyProvider"));
			m_callHierarchyProvider = callHierarchyProvider;
		}
		if(json.contains("linkedEditingRangeProvider")) {
			auto linkedEditingRangeProvider = new QJsonValue(json.value("linkedEditingRangeProvider"));
			m_linkedEditingRangeProvider = linkedEditingRangeProvider;
		}
		if(json.contains("semanticTokensProvider")) {
			auto semanticTokensProvider = new QJsonValue(json.value("semanticTokensProvider"));
			m_semanticTokensProvider = semanticTokensProvider;
		}
		if(json.contains("workspace")) {
			auto workspace = new ServerCapabilitiesWorkspace();
			workspace->fromJson(json.value("workspace").toObject());
			m_workspace = workspace;
		}
		if(json.contains("monikerProvider")) {
			auto monikerProvider = new QJsonValue(json.value("monikerProvider"));
			m_monikerProvider = monikerProvider;
		}
		if(json.contains("experimental")) {
			auto experimental = new QJsonValue(json.value("experimental"));
			m_experimental = experimental;
		}

	}
};
const int SymbolKindString = 15;
typedef CompletionTextDocumentClientCapabilitiesItemTagSupport TextDocumentClientCapabilitiesCompletionItemTagSupport;
const int SymbolKindConstant = 14;
class Command: virtual public GoPlsParams {
	QString *m_title = nullptr;
	QString *m_command = nullptr;
	QList<QJsonValue*> *m_arguments = nullptr;

public:
	Command() = default;
	virtual ~Command() {
		if(m_title) { delete m_title; }
		if(m_command) { delete m_command; }
		if(m_arguments) { qDeleteAll(*m_arguments); delete m_arguments; }
	}

	void setTitle(QString *title) { m_title = title; }
	QString *getTitle() { return m_title; }
	void setCommand(QString *command) { m_command = command; }
	QString *getCommand() { return m_command; }
	void setArguments(QList<QJsonValue*> *arguments) { m_arguments = arguments; }
	QList<QJsonValue*> *getArguments() { return m_arguments; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_title) {
			QJsonValue title = QJsonValue::fromVariant(QVariant::fromValue(*m_title));
			obj.insert("title", title);
		}
		if(m_command) {
			QJsonValue command = QJsonValue::fromVariant(QVariant::fromValue(*m_command));
			obj.insert("command", command);
		}
		if(m_arguments) {
				QJsonArray arguments;
			for(auto it = m_arguments->cbegin(); it != m_arguments->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = *m_item;;
				arguments.append(item);
			}

			obj.insert("arguments", arguments);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("title")) {
			auto title = new QString(json.value("title").toString());
			m_title = title;
		}
		if(json.contains("command")) {
			auto command = new QString(json.value("command").toString());
			m_command = command;
		}
		if(json.contains("arguments")) {
			 QJsonArray arr = json.value("arguments").toArray();
			auto arguments = new QList<QJsonValue*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QJsonValue((m_item));
				arguments->append(item);
			}
			m_arguments = arguments;
		}

	}
};
class CompletionItem: virtual public GoPlsParams {
	QList<TextEdit*> *m_additionalTextEdits = nullptr;
	Command *m_command = nullptr;
	QList<QString*> *m_commitCharacters = nullptr;
	QList<CompletionItemTag*> *m_tags = nullptr;
	QJsonValue *m_data = nullptr;
	bool *m_deprecated = nullptr;
	QString *m_detail = nullptr;
	QJsonValue *m_documentation = nullptr;
	QString *m_filterText = nullptr;
	QString *m_insertText = nullptr;
	InsertTextFormat *m_insertTextFormat = nullptr;
	InsertTextMode *m_insertTextMode = nullptr;
	CompletionItemKind *m_kind = nullptr;
	QString *m_label = nullptr;
	bool *m_preselect = nullptr;
	QString *m_sortText = nullptr;
	TextEdit *m_textEdit = nullptr;

public:
	CompletionItem() = default;
	virtual ~CompletionItem() {
		if(m_additionalTextEdits) { qDeleteAll(*m_additionalTextEdits); delete m_additionalTextEdits; }
		if(m_command) { delete m_command; }
		if(m_commitCharacters) { qDeleteAll(*m_commitCharacters); delete m_commitCharacters; }
		if(m_tags) { qDeleteAll(*m_tags); delete m_tags; }
		if(m_data) { delete m_data; }
		if(m_deprecated) { delete m_deprecated; }
		if(m_detail) { delete m_detail; }
		if(m_documentation) { delete m_documentation; }
		if(m_filterText) { delete m_filterText; }
		if(m_insertText) { delete m_insertText; }
		if(m_insertTextFormat) { delete m_insertTextFormat; }
		if(m_insertTextMode) { delete m_insertTextMode; }
		if(m_kind) { delete m_kind; }
		if(m_label) { delete m_label; }
		if(m_preselect) { delete m_preselect; }
		if(m_sortText) { delete m_sortText; }
		if(m_textEdit) { delete m_textEdit; }
	}

	void setAdditionalTextEdits(QList<TextEdit*> *additionalTextEdits) { m_additionalTextEdits = additionalTextEdits; }
	QList<TextEdit*> *getAdditionalTextEdits() { return m_additionalTextEdits; }
	void setCommand(Command *command) { m_command = command; }
	Command *getCommand() { return m_command; }
	void setCommitCharacters(QList<QString*> *commitCharacters) { m_commitCharacters = commitCharacters; }
	QList<QString*> *getCommitCharacters() { return m_commitCharacters; }
	void setTags(QList<CompletionItemTag*> *tags) { m_tags = tags; }
	QList<CompletionItemTag*> *getTags() { return m_tags; }
	void setData(QJsonValue *data) { m_data = data; }
	QJsonValue *getData() { return m_data; }
	void setDeprecated(bool *deprecated) { m_deprecated = deprecated; }
	bool *getDeprecated() { return m_deprecated; }
	void setDetail(QString *detail) { m_detail = detail; }
	QString *getDetail() { return m_detail; }
	void setDocumentation(QJsonValue *documentation) { m_documentation = documentation; }
	QJsonValue *getDocumentation() { return m_documentation; }
	void setFilterText(QString *filterText) { m_filterText = filterText; }
	QString *getFilterText() { return m_filterText; }
	void setInsertText(QString *insertText) { m_insertText = insertText; }
	QString *getInsertText() { return m_insertText; }
	void setInsertTextFormat(InsertTextFormat *insertTextFormat) { m_insertTextFormat = insertTextFormat; }
	InsertTextFormat *getInsertTextFormat() { return m_insertTextFormat; }
	void setInsertTextMode(InsertTextMode *insertTextMode) { m_insertTextMode = insertTextMode; }
	InsertTextMode *getInsertTextMode() { return m_insertTextMode; }
	void setKind(CompletionItemKind *kind) { m_kind = kind; }
	CompletionItemKind *getKind() { return m_kind; }
	void setLabel(QString *label) { m_label = label; }
	QString *getLabel() { return m_label; }
	void setPreselect(bool *preselect) { m_preselect = preselect; }
	bool *getPreselect() { return m_preselect; }
	void setSortText(QString *sortText) { m_sortText = sortText; }
	QString *getSortText() { return m_sortText; }
	void setTextEdit(TextEdit *textEdit) { m_textEdit = textEdit; }
	TextEdit *getTextEdit() { return m_textEdit; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_additionalTextEdits) {
				QJsonArray additionalTextEdits;
			for(auto it = m_additionalTextEdits->cbegin(); it != m_additionalTextEdits->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				additionalTextEdits.append(item);
			}

			obj.insert("additionalTextEdits", additionalTextEdits);
		}
		if(m_command) {
			QJsonObject command = m_command->toJson();
			obj.insert("command", command);
		}
		if(m_commitCharacters) {
				QJsonArray commitCharacters;
			for(auto it = m_commitCharacters->cbegin(); it != m_commitCharacters->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				commitCharacters.append(item);
			}

			obj.insert("commitCharacters", commitCharacters);
		}
		if(m_tags) {
				QJsonArray tags;
			for(auto it = m_tags->cbegin(); it != m_tags->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				tags.append(item);
			}

			obj.insert("tags", tags);
		}
		if(m_data) {
			QJsonValue data = *m_data;
			obj.insert("data", data);
		}
		if(m_deprecated) {
			QJsonValue deprecated = QJsonValue::fromVariant(QVariant::fromValue(*m_deprecated));
			obj.insert("deprecated", deprecated);
		}
		if(m_detail) {
			QJsonValue detail = QJsonValue::fromVariant(QVariant::fromValue(*m_detail));
			obj.insert("detail", detail);
		}
		if(m_documentation) {
			QJsonValue documentation = *m_documentation;
			obj.insert("documentation", documentation);
		}
		if(m_filterText) {
			QJsonValue filterText = QJsonValue::fromVariant(QVariant::fromValue(*m_filterText));
			obj.insert("filterText", filterText);
		}
		if(m_insertText) {
			QJsonValue insertText = QJsonValue::fromVariant(QVariant::fromValue(*m_insertText));
			obj.insert("insertText", insertText);
		}
		if(m_insertTextFormat) {
			QJsonValue insertTextFormat = QJsonValue::fromVariant(QVariant::fromValue(*m_insertTextFormat));
			obj.insert("insertTextFormat", insertTextFormat);
		}
		if(m_insertTextMode) {
			QJsonValue insertTextMode = QJsonValue::fromVariant(QVariant::fromValue(*m_insertTextMode));
			obj.insert("insertTextMode", insertTextMode);
		}
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_label) {
			QJsonValue label = QJsonValue::fromVariant(QVariant::fromValue(*m_label));
			obj.insert("label", label);
		}
		if(m_preselect) {
			QJsonValue preselect = QJsonValue::fromVariant(QVariant::fromValue(*m_preselect));
			obj.insert("preselect", preselect);
		}
		if(m_sortText) {
			QJsonValue sortText = QJsonValue::fromVariant(QVariant::fromValue(*m_sortText));
			obj.insert("sortText", sortText);
		}
		if(m_textEdit) {
			QJsonObject textEdit = m_textEdit->toJson();
			obj.insert("textEdit", textEdit);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("additionalTextEdits")) {
			 QJsonArray arr = json.value("additionalTextEdits").toArray();
			auto additionalTextEdits = new QList<TextEdit*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new TextEdit();
			item->fromJson((m_item).toObject());
				additionalTextEdits->append(item);
			}
			m_additionalTextEdits = additionalTextEdits;
		}
		if(json.contains("command")) {
			auto command = new Command();
			command->fromJson(json.value("command").toObject());
			m_command = command;
		}
		if(json.contains("commitCharacters")) {
			 QJsonArray arr = json.value("commitCharacters").toArray();
			auto commitCharacters = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				commitCharacters->append(item);
			}
			m_commitCharacters = commitCharacters;
		}
		if(json.contains("tags")) {
			 QJsonArray arr = json.value("tags").toArray();
			auto tags = new QList<CompletionItemTag*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				tags->append(item);
			}
			m_tags = tags;
		}
		if(json.contains("data")) {
			auto data = new QJsonValue(json.value("data"));
			m_data = data;
		}
		if(json.contains("deprecated")) {
			auto deprecated = new bool(json.value("deprecated").toVariant().value<bool>());
			m_deprecated = deprecated;
		}
		if(json.contains("detail")) {
			auto detail = new QString(json.value("detail").toString());
			m_detail = detail;
		}
		if(json.contains("documentation")) {
			auto documentation = new QJsonValue(json.value("documentation"));
			m_documentation = documentation;
		}
		if(json.contains("filterText")) {
			auto filterText = new QString(json.value("filterText").toString());
			m_filterText = filterText;
		}
		if(json.contains("insertText")) {
			auto insertText = new QString(json.value("insertText").toString());
			m_insertText = insertText;
		}
		if(json.contains("insertTextFormat")) {
			auto insertTextFormat = new double(json.value("insertTextFormat").toVariant().value<double>());
			m_insertTextFormat = insertTextFormat;
		}
		if(json.contains("insertTextMode")) {
			auto insertTextMode = new double(json.value("insertTextMode").toVariant().value<double>());
			m_insertTextMode = insertTextMode;
		}
		if(json.contains("kind")) {
			auto kind = new double(json.value("kind").toVariant().value<double>());
			m_kind = kind;
		}
		if(json.contains("label")) {
			auto label = new QString(json.value("label").toString());
			m_label = label;
		}
		if(json.contains("preselect")) {
			auto preselect = new bool(json.value("preselect").toVariant().value<bool>());
			m_preselect = preselect;
		}
		if(json.contains("sortText")) {
			auto sortText = new QString(json.value("sortText").toString());
			m_sortText = sortText;
		}
		if(json.contains("textEdit")) {
			auto textEdit = new TextEdit();
			textEdit->fromJson(json.value("textEdit").toObject());
			m_textEdit = textEdit;
		}

	}
};
class CompletionList: virtual public GoPlsParams {
	bool *m_isIncomplete = nullptr;
	QList<CompletionItem*> *m_items = nullptr;

public:
	CompletionList() = default;
	virtual ~CompletionList() {
		if(m_isIncomplete) { delete m_isIncomplete; }
		if(m_items) { qDeleteAll(*m_items); delete m_items; }
	}

	void setIsIncomplete(bool *isIncomplete) { m_isIncomplete = isIncomplete; }
	bool *getIsIncomplete() { return m_isIncomplete; }
	void setItems(QList<CompletionItem*> *items) { m_items = items; }
	QList<CompletionItem*> *getItems() { return m_items; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_isIncomplete) {
			QJsonValue isIncomplete = QJsonValue::fromVariant(QVariant::fromValue(*m_isIncomplete));
			obj.insert("isIncomplete", isIncomplete);
		}
		if(m_items) {
				QJsonArray items;
			for(auto it = m_items->cbegin(); it != m_items->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				items.append(item);
			}

			obj.insert("items", items);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("isIncomplete")) {
			auto isIncomplete = new bool(json.value("isIncomplete").toVariant().value<bool>());
			m_isIncomplete = isIncomplete;
		}
		if(json.contains("items")) {
			 QJsonArray arr = json.value("items").toArray();
			auto items = new QList<CompletionItem*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new CompletionItem();
			item->fromJson((m_item).toObject());
				items->append(item);
			}
			m_items = items;
		}

	}
};
const QString MethodClientRegisterCapability = "client/registerCapability";
const QString MethodWorkspaceDidChangeWatchedFiles = "workspace/didChangeWatchedFiles";
typedef QList<Diagnostic*> Diagnostics;
class LinkedEditingRangeParams:  public TextDocumentPositionParams, public WorkDoneProgressParams {

public:
	LinkedEditingRangeParams() = default;
	virtual ~LinkedEditingRangeParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);

	}
};
class MonikerParams:  public TextDocumentPositionParams, public WorkDoneProgressParams, public PartialResultParams {

public:
	MonikerParams() = default;
	virtual ~MonikerParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);

	}
};
typedef QList<CodeActionKind*> CodeActionKinds;
const QString MethodTextDocumentDidSave = "textDocument/didSave";
const QString MethodWorkspaceDidChangeConfiguration = "workspace/didChangeConfiguration";
const int DiagnosticTagUnnecessary = 1;
const QString SemanticTokenStruct = "struct";
typedef QString MonikerKind;
const QString FailureHandlingKindTransactional = "transactional";
const QString MethodProgress = "$/progress";
const int CompletionItemTagDeprecated = 1;
const QString PlainText = "plaintext";
class FileDelete: virtual public GoPlsParams {
	QString *m_uri = nullptr;

public:
	FileDelete() = default;
	virtual ~FileDelete() {
		if(m_uri) { delete m_uri; }
	}

	void setUri(QString *uri) { m_uri = uri; }
	QString *getUri() { return m_uri; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}

	}
};
const QString SemanticTokenModifierReadonly = "readonly";
const int CompletionTriggerKindTriggerCharacter = 2;
const QString RegionFoldingRange = "region";
typedef DocumentFormattingClientCapabilities TextDocumentClientCapabilitiesFormatting;
const QString MethodWorkspaceExecuteCommand = "workspace/executeCommand";
const QString DockerfileLanguage = "dockerfile";
const QString SemanticTokenParameter = "parameter";
class CodeLens: virtual public GoPlsParams {
	Range *m_range = nullptr;
	Command *m_command = nullptr;
	QJsonValue *m_data = nullptr;

public:
	CodeLens() = default;
	virtual ~CodeLens() {
		if(m_range) { delete m_range; }
		if(m_command) { delete m_command; }
		if(m_data) { delete m_data; }
	}

	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setCommand(Command *command) { m_command = command; }
	Command *getCommand() { return m_command; }
	void setData(QJsonValue *data) { m_data = data; }
	QJsonValue *getData() { return m_data; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_command) {
			QJsonObject command = m_command->toJson();
			obj.insert("command", command);
		}
		if(m_data) {
			QJsonValue data = *m_data;
			obj.insert("data", data);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("command")) {
			auto command = new Command();
			command->fromJson(json.value("command").toObject());
			m_command = command;
		}
		if(json.contains("data")) {
			auto data = new QJsonValue(json.value("data"));
			m_data = data;
		}

	}
};
typedef QList<MessageActionItem*> MessageActionItems;
const int SymbolKindInterface = 11;
const int SymbolKindModule = 2;
const QString MethodTextDocumentColorPresentation = "textDocument/colorPresentation";
typedef QList<CompletionItemTag*> CompletionItemTags;
const QString GoLanguage = "go";
const int CompletionItemKindMethod = 2;
const QString MethodClientUnregisterCapability = "client/unregisterCapability";
const QString MethodTextDocumentImplementation = "textDocument/implementation";
class VersionedTextDocumentIdentifier:  public TextDocumentIdentifier {
	int *m_version = nullptr;

public:
	VersionedTextDocumentIdentifier() = default;
	virtual ~VersionedTextDocumentIdentifier() {
		if(m_version) { delete m_version; }
	}

	void setVersion(int *version) { m_version = version; }
	int *getVersion() { return m_version; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentIdentifier_json = TextDocumentIdentifier::toJson();
		for(const QString &key: TextDocumentIdentifier_json.keys()) {
			obj.insert(key, TextDocumentIdentifier_json.value(key));
		}
		if(m_version) {
			QJsonValue version = QJsonValue::fromVariant(QVariant::fromValue(*m_version));
			obj.insert("version", version);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentIdentifier::fromJson(json);
		if(json.contains("version")) {
			auto version = new int(json.value("version").toVariant().value<int>());
			m_version = version;
		}

	}
};
const int SymbolKindOperator = 25;
const int PrepareSupportDefaultBehaviorIdentifier = 1;
const QString TeXLanguage = "tex";
const QString SemanticTokenModifierDeprecated = "deprecated";
class SemanticTokensDeltaParams:  public WorkDoneProgressParams, public PartialResultParams {
	TextDocumentIdentifier *m_textDocument = nullptr;
	QString *m_previousResultId = nullptr;

public:
	SemanticTokensDeltaParams() = default;
	virtual ~SemanticTokensDeltaParams() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_previousResultId) { delete m_previousResultId; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setPreviousResultId(QString *previousResultId) { m_previousResultId = previousResultId; }
	QString *getPreviousResultId() { return m_previousResultId; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_previousResultId) {
			QJsonValue previousResultId = QJsonValue::fromVariant(QVariant::fromValue(*m_previousResultId));
			obj.insert("previousResultId", previousResultId);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("previousResultId")) {
			auto previousResultId = new QString(json.value("previousResultId").toString());
			m_previousResultId = previousResultId;
		}

	}
};
class InitializeResult: virtual public GoPlsParams {
	ServerCapabilities *m_capabilities = nullptr;
	ServerInfo *m_serverInfo = nullptr;

public:
	InitializeResult() = default;
	virtual ~InitializeResult() {
		if(m_capabilities) { delete m_capabilities; }
		if(m_serverInfo) { delete m_serverInfo; }
	}

	void setCapabilities(ServerCapabilities *capabilities) { m_capabilities = capabilities; }
	ServerCapabilities *getCapabilities() { return m_capabilities; }
	void setServerInfo(ServerInfo *serverInfo) { m_serverInfo = serverInfo; }
	ServerInfo *getServerInfo() { return m_serverInfo; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_capabilities) {
			QJsonObject capabilities = m_capabilities->toJson();
			obj.insert("capabilities", capabilities);
		}
		if(m_serverInfo) {
			QJsonObject serverInfo = m_serverInfo->toJson();
			obj.insert("serverInfo", serverInfo);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("capabilities")) {
			auto capabilities = new ServerCapabilities();
			capabilities->fromJson(json.value("capabilities").toObject());
			m_capabilities = capabilities;
		}
		if(json.contains("serverInfo")) {
			auto serverInfo = new ServerInfo();
			serverInfo->fromJson(json.value("serverInfo").toObject());
			m_serverInfo = serverInfo;
		}

	}
};
const QString WorkDoneProgressKindReport = "report";
const QString PHPLanguage = "php";
class CompletionRegistrationOptions:  public TextDocumentRegistrationOptions {
	QList<QString*> *m_triggerCharacters = nullptr;
	bool *m_resolveProvider = nullptr;

public:
	CompletionRegistrationOptions() = default;
	virtual ~CompletionRegistrationOptions() {
		if(m_triggerCharacters) { qDeleteAll(*m_triggerCharacters); delete m_triggerCharacters; }
		if(m_resolveProvider) { delete m_resolveProvider; }
	}

	void setTriggerCharacters(QList<QString*> *triggerCharacters) { m_triggerCharacters = triggerCharacters; }
	QList<QString*> *getTriggerCharacters() { return m_triggerCharacters; }
	void setResolveProvider(bool *resolveProvider) { m_resolveProvider = resolveProvider; }
	bool *getResolveProvider() { return m_resolveProvider; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		if(m_triggerCharacters) {
				QJsonArray triggerCharacters;
			for(auto it = m_triggerCharacters->cbegin(); it != m_triggerCharacters->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				triggerCharacters.append(item);
			}

			obj.insert("triggerCharacters", triggerCharacters);
		}
		if(m_resolveProvider) {
			QJsonValue resolveProvider = QJsonValue::fromVariant(QVariant::fromValue(*m_resolveProvider));
			obj.insert("resolveProvider", resolveProvider);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		if(json.contains("triggerCharacters")) {
			 QJsonArray arr = json.value("triggerCharacters").toArray();
			auto triggerCharacters = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				triggerCharacters->append(item);
			}
			m_triggerCharacters = triggerCharacters;
		}
		if(json.contains("resolveProvider")) {
			auto resolveProvider = new bool(json.value("resolveProvider").toVariant().value<bool>());
			m_resolveProvider = resolveProvider;
		}

	}
};
const QString MethodWindowShowMessageRequest = "window/showMessageRequest";
const QString MethodTextDocumentSignatureHelp = "textDocument/signatureHelp";
class ExecuteCommandParams:  public WorkDoneProgressParams {
	QString *m_command = nullptr;
	QList<QJsonValue*> *m_arguments = nullptr;

public:
	ExecuteCommandParams() = default;
	virtual ~ExecuteCommandParams() {
		if(m_command) { delete m_command; }
		if(m_arguments) { qDeleteAll(*m_arguments); delete m_arguments; }
	}

	void setCommand(QString *command) { m_command = command; }
	QString *getCommand() { return m_command; }
	void setArguments(QList<QJsonValue*> *arguments) { m_arguments = arguments; }
	QList<QJsonValue*> *getArguments() { return m_arguments; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		if(m_command) {
			QJsonValue command = QJsonValue::fromVariant(QVariant::fromValue(*m_command));
			obj.insert("command", command);
		}
		if(m_arguments) {
				QJsonArray arguments;
			for(auto it = m_arguments->cbegin(); it != m_arguments->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = *m_item;;
				arguments.append(item);
			}

			obj.insert("arguments", arguments);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		if(json.contains("command")) {
			auto command = new QString(json.value("command").toString());
			m_command = command;
		}
		if(json.contains("arguments")) {
			 QJsonArray arr = json.value("arguments").toArray();
			auto arguments = new QList<QJsonValue*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QJsonValue((m_item));
				arguments->append(item);
			}
			m_arguments = arguments;
		}

	}
};
class WorkDoneProgressCreateParams: virtual public GoPlsParams {
	ProgressToken *m_token = nullptr;

public:
	WorkDoneProgressCreateParams() = default;
	virtual ~WorkDoneProgressCreateParams() {
		if(m_token) { delete m_token; }
	}

	void setToken(ProgressToken *token) { m_token = token; }
	ProgressToken *getToken() { return m_token; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_token) {
			QJsonValue token = *m_token;
			obj.insert("token", token);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("token")) {
			auto token = new QJsonValue(json.value("token"));
			m_token = token;
		}

	}
};
class DefinitionParams:  public TextDocumentPositionParams, public WorkDoneProgressParams, public PartialResultParams {

public:
	DefinitionParams() = default;
	virtual ~DefinitionParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);

	}
};
const QString HandlebarsLanguage = "handlebars";
const QString SemanticTokenMacro = "macro";
class DocumentLinkRegistrationOptions:  public TextDocumentRegistrationOptions {
	bool *m_resolveProvider = nullptr;

public:
	DocumentLinkRegistrationOptions() = default;
	virtual ~DocumentLinkRegistrationOptions() {
		if(m_resolveProvider) { delete m_resolveProvider; }
	}

	void setResolveProvider(bool *resolveProvider) { m_resolveProvider = resolveProvider; }
	bool *getResolveProvider() { return m_resolveProvider; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		if(m_resolveProvider) {
			QJsonValue resolveProvider = QJsonValue::fromVariant(QVariant::fromValue(*m_resolveProvider));
			obj.insert("resolveProvider", resolveProvider);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		if(json.contains("resolveProvider")) {
			auto resolveProvider = new bool(json.value("resolveProvider").toVariant().value<bool>());
			m_resolveProvider = resolveProvider;
		}

	}
};
class InitializedParams: virtual public GoPlsParams {

public:
	InitializedParams() = default;
	virtual ~InitializedParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;

		return obj;
	}

	void fromJson(const QJsonObject &json) {

	}
};
class MarkupContent: virtual public GoPlsParams {
	MarkupKind *m_kind = nullptr;
	QString *m_value = nullptr;

public:
	MarkupContent() = default;
	virtual ~MarkupContent() {
		if(m_kind) { delete m_kind; }
		if(m_value) { delete m_value; }
	}

	void setKind(MarkupKind *kind) { m_kind = kind; }
	MarkupKind *getKind() { return m_kind; }
	void setValue(QString *value) { m_value = value; }
	QString *getValue() { return m_value; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_value) {
			QJsonValue value = QJsonValue::fromVariant(QVariant::fromValue(*m_value));
			obj.insert("value", value);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("kind")) {
			auto kind = new QString(json.value("kind").toString());
			m_kind = kind;
		}
		if(json.contains("value")) {
			auto value = new QString(json.value("value").toString());
			m_value = value;
		}

	}
};
class Hover: virtual public GoPlsParams {
	MarkupContent *m_contents = nullptr;
	Range *m_range = nullptr;

public:
	Hover() = default;
	virtual ~Hover() {
		if(m_contents) { delete m_contents; }
		if(m_range) { delete m_range; }
	}

	void setContents(MarkupContent *contents) { m_contents = contents; }
	MarkupContent *getContents() { return m_contents; }
	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_contents) {
			QJsonObject contents = m_contents->toJson();
			obj.insert("contents", contents);
		}
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("contents")) {
			auto contents = new MarkupContent();
			contents->fromJson(json.value("contents").toObject());
			m_contents = contents;
		}
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}

	}
};
typedef CompletionTextDocumentClientCapabilitiesItem TextDocumentClientCapabilitiesCompletionItem;
typedef double TextDocumentSaveReason;
class WillSaveTextDocumentParams: virtual public GoPlsParams {
	TextDocumentIdentifier *m_textDocument = nullptr;
	TextDocumentSaveReason *m_reason = nullptr;

public:
	WillSaveTextDocumentParams() = default;
	virtual ~WillSaveTextDocumentParams() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_reason) { delete m_reason; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setReason(TextDocumentSaveReason *reason) { m_reason = reason; }
	TextDocumentSaveReason *getReason() { return m_reason; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_reason) {
			QJsonValue reason = QJsonValue::fromVariant(QVariant::fromValue(*m_reason));
			obj.insert("reason", reason);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("reason")) {
			auto reason = new double(json.value("reason").toVariant().value<double>());
			m_reason = reason;
		}

	}
};
typedef QList<CompletionItemKind*> CompletionItemKinds;
const int InsertTextFormatPlainText = 1;
const int CompletionItemKindSnippet = 15;
class WorkspaceFoldersChangeEvent: virtual public GoPlsParams {
	QList<WorkspaceFolder*> *m_added = nullptr;
	QList<WorkspaceFolder*> *m_removed = nullptr;

public:
	WorkspaceFoldersChangeEvent() = default;
	virtual ~WorkspaceFoldersChangeEvent() {
		if(m_added) { qDeleteAll(*m_added); delete m_added; }
		if(m_removed) { qDeleteAll(*m_removed); delete m_removed; }
	}

	void setAdded(QList<WorkspaceFolder*> *added) { m_added = added; }
	QList<WorkspaceFolder*> *getAdded() { return m_added; }
	void setRemoved(QList<WorkspaceFolder*> *removed) { m_removed = removed; }
	QList<WorkspaceFolder*> *getRemoved() { return m_removed; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_added) {
				QJsonArray added;
			for(auto it = m_added->cbegin(); it != m_added->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				added.append(item);
			}

			obj.insert("added", added);
		}
		if(m_removed) {
				QJsonArray removed;
			for(auto it = m_removed->cbegin(); it != m_removed->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				removed.append(item);
			}

			obj.insert("removed", removed);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("added")) {
			 QJsonArray arr = json.value("added").toArray();
			auto added = new QList<WorkspaceFolder*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new WorkspaceFolder();
			item->fromJson((m_item).toObject());
				added->append(item);
			}
			m_added = added;
		}
		if(json.contains("removed")) {
			 QJsonArray arr = json.value("removed").toArray();
			auto removed = new QList<WorkspaceFolder*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new WorkspaceFolder();
			item->fromJson((m_item).toObject());
				removed->append(item);
			}
			m_removed = removed;
		}

	}
};
class DidChangeWorkspaceFoldersParams: virtual public GoPlsParams {
	WorkspaceFoldersChangeEvent *m_event = nullptr;

public:
	DidChangeWorkspaceFoldersParams() = default;
	virtual ~DidChangeWorkspaceFoldersParams() {
		if(m_event) { delete m_event; }
	}

	void setEvent(WorkspaceFoldersChangeEvent *event) { m_event = event; }
	WorkspaceFoldersChangeEvent *getEvent() { return m_event; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_event) {
			QJsonObject event = m_event->toJson();
			obj.insert("event", event);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("event")) {
			auto event = new WorkspaceFoldersChangeEvent();
			event->fromJson(json.value("event").toObject());
			m_event = event;
		}

	}
};
const QString DiffLanguage = "diff";
typedef QList<Registration*> Registrations;
class SemanticTokensEdit: virtual public GoPlsParams {
	unsigned int *m_start = nullptr;
	unsigned int *m_deleteCount = nullptr;
	QList<unsigned int*> *m_data = nullptr;

public:
	SemanticTokensEdit() = default;
	virtual ~SemanticTokensEdit() {
		if(m_start) { delete m_start; }
		if(m_deleteCount) { delete m_deleteCount; }
		if(m_data) { qDeleteAll(*m_data); delete m_data; }
	}

	void setStart(unsigned int *start) { m_start = start; }
	unsigned int *getStart() { return m_start; }
	void setDeleteCount(unsigned int *deleteCount) { m_deleteCount = deleteCount; }
	unsigned int *getDeleteCount() { return m_deleteCount; }
	void setData(QList<unsigned int*> *data) { m_data = data; }
	QList<unsigned int*> *getData() { return m_data; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_start) {
			QJsonValue start = QJsonValue::fromVariant(QVariant::fromValue(*m_start));
			obj.insert("start", start);
		}
		if(m_deleteCount) {
			QJsonValue deleteCount = QJsonValue::fromVariant(QVariant::fromValue(*m_deleteCount));
			obj.insert("deleteCount", deleteCount);
		}
		if(m_data) {
				QJsonArray data;
			for(auto it = m_data->cbegin(); it != m_data->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				data.append(item);
			}

			obj.insert("data", data);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("start")) {
			auto start = new unsigned int(json.value("start").toVariant().value<unsigned int>());
			m_start = start;
		}
		if(json.contains("deleteCount")) {
			auto deleteCount = new unsigned int(json.value("deleteCount").toVariant().value<unsigned int>());
			m_deleteCount = deleteCount;
		}
		if(json.contains("data")) {
			 QJsonArray arr = json.value("data").toArray();
			auto data = new QList<unsigned int*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new unsigned int((m_item).toVariant().value<unsigned int>());
				data->append(item);
			}
			m_data = data;
		}

	}
};
class SemanticTokensDelta: virtual public GoPlsParams {
	QString *m_resultId = nullptr;
	QList<SemanticTokensEdit*> *m_edits = nullptr;

public:
	SemanticTokensDelta() = default;
	virtual ~SemanticTokensDelta() {
		if(m_resultId) { delete m_resultId; }
		if(m_edits) { qDeleteAll(*m_edits); delete m_edits; }
	}

	void setResultId(QString *resultId) { m_resultId = resultId; }
	QString *getResultId() { return m_resultId; }
	void setEdits(QList<SemanticTokensEdit*> *edits) { m_edits = edits; }
	QList<SemanticTokensEdit*> *getEdits() { return m_edits; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_resultId) {
			QJsonValue resultId = QJsonValue::fromVariant(QVariant::fromValue(*m_resultId));
			obj.insert("resultId", resultId);
		}
		if(m_edits) {
				QJsonArray edits;
			for(auto it = m_edits->cbegin(); it != m_edits->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				edits.append(item);
			}

			obj.insert("edits", edits);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("resultId")) {
			auto resultId = new QString(json.value("resultId").toString());
			m_resultId = resultId;
		}
		if(json.contains("edits")) {
			 QJsonArray arr = json.value("edits").toArray();
			auto edits = new QList<SemanticTokensEdit*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new SemanticTokensEdit();
			item->fromJson((m_item).toObject());
				edits->append(item);
			}
			m_edits = edits;
		}

	}
};
const QString SemanticTokenModifier = "modifier";
const int SymbolKindNamespace = 3;
typedef DocumentOnTypeFormattingClientCapabilities TextDocumentClientCapabilitiesOnTypeFormatting;
const QString MethodTextDocumentDocumentLink = "textDocument/documentLink";
const QString RustLanguage = "rust";
const QString FileOperationPatternKindFile = "file";
const QString MonikerKindExport = "export";
typedef SignatureHelpTextDocumentClientCapabilities TextDocumentClientCapabilitiesSignatureHelp;
typedef QList<DiagnosticTag*> DiagnosticTags;
class ColorPresentation: virtual public GoPlsParams {
	QString *m_label = nullptr;
	TextEdit *m_textEdit = nullptr;
	QList<TextEdit*> *m_additionalTextEdits = nullptr;

public:
	ColorPresentation() = default;
	virtual ~ColorPresentation() {
		if(m_label) { delete m_label; }
		if(m_textEdit) { delete m_textEdit; }
		if(m_additionalTextEdits) { qDeleteAll(*m_additionalTextEdits); delete m_additionalTextEdits; }
	}

	void setLabel(QString *label) { m_label = label; }
	QString *getLabel() { return m_label; }
	void setTextEdit(TextEdit *textEdit) { m_textEdit = textEdit; }
	TextEdit *getTextEdit() { return m_textEdit; }
	void setAdditionalTextEdits(QList<TextEdit*> *additionalTextEdits) { m_additionalTextEdits = additionalTextEdits; }
	QList<TextEdit*> *getAdditionalTextEdits() { return m_additionalTextEdits; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_label) {
			QJsonValue label = QJsonValue::fromVariant(QVariant::fromValue(*m_label));
			obj.insert("label", label);
		}
		if(m_textEdit) {
			QJsonObject textEdit = m_textEdit->toJson();
			obj.insert("textEdit", textEdit);
		}
		if(m_additionalTextEdits) {
				QJsonArray additionalTextEdits;
			for(auto it = m_additionalTextEdits->cbegin(); it != m_additionalTextEdits->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				additionalTextEdits.append(item);
			}

			obj.insert("additionalTextEdits", additionalTextEdits);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("label")) {
			auto label = new QString(json.value("label").toString());
			m_label = label;
		}
		if(json.contains("textEdit")) {
			auto textEdit = new TextEdit();
			textEdit->fromJson(json.value("textEdit").toObject());
			m_textEdit = textEdit;
		}
		if(json.contains("additionalTextEdits")) {
			 QJsonArray arr = json.value("additionalTextEdits").toArray();
			auto additionalTextEdits = new QList<TextEdit*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new TextEdit();
			item->fromJson((m_item).toObject());
				additionalTextEdits->append(item);
			}
			m_additionalTextEdits = additionalTextEdits;
		}

	}
};
typedef FoldingRangeClientCapabilities TextDocumentClientCapabilitiesFoldingRange;
typedef SemanticTokensWorkspaceClientCapabilitiesRequests WorkspaceClientCapabilitiesSemanticTokensRequests;
const int CompletionItemKindClass = 7;
const QString Refactor = "refactor";
typedef ReferencesTextDocumentClientCapabilities TextDocumentClientCapabilitiesReferences;
const QString JadeLanguage = "jade";
class HoverOptions:  public WorkDoneProgressOptions {

public:
	HoverOptions() = default;
	virtual ~HoverOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
class SignatureInformation: virtual public GoPlsParams {
	QString *m_label = nullptr;
	QJsonValue *m_documentation = nullptr;
	QList<ParameterInformation*> *m_parameters = nullptr;
	unsigned int *m_activeParameter = nullptr;

public:
	SignatureInformation() = default;
	virtual ~SignatureInformation() {
		if(m_label) { delete m_label; }
		if(m_documentation) { delete m_documentation; }
		if(m_parameters) { qDeleteAll(*m_parameters); delete m_parameters; }
		if(m_activeParameter) { delete m_activeParameter; }
	}

	void setLabel(QString *label) { m_label = label; }
	QString *getLabel() { return m_label; }
	void setDocumentation(QJsonValue *documentation) { m_documentation = documentation; }
	QJsonValue *getDocumentation() { return m_documentation; }
	void setParameters(QList<ParameterInformation*> *parameters) { m_parameters = parameters; }
	QList<ParameterInformation*> *getParameters() { return m_parameters; }
	void setActiveParameter(unsigned int *activeParameter) { m_activeParameter = activeParameter; }
	unsigned int *getActiveParameter() { return m_activeParameter; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_label) {
			QJsonValue label = QJsonValue::fromVariant(QVariant::fromValue(*m_label));
			obj.insert("label", label);
		}
		if(m_documentation) {
			QJsonValue documentation = *m_documentation;
			obj.insert("documentation", documentation);
		}
		if(m_parameters) {
				QJsonArray parameters;
			for(auto it = m_parameters->cbegin(); it != m_parameters->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				parameters.append(item);
			}

			obj.insert("parameters", parameters);
		}
		if(m_activeParameter) {
			QJsonValue activeParameter = QJsonValue::fromVariant(QVariant::fromValue(*m_activeParameter));
			obj.insert("activeParameter", activeParameter);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("label")) {
			auto label = new QString(json.value("label").toString());
			m_label = label;
		}
		if(json.contains("documentation")) {
			auto documentation = new QJsonValue(json.value("documentation"));
			m_documentation = documentation;
		}
		if(json.contains("parameters")) {
			 QJsonArray arr = json.value("parameters").toArray();
			auto parameters = new QList<ParameterInformation*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new ParameterInformation();
			item->fromJson((m_item).toObject());
				parameters->append(item);
			}
			m_parameters = parameters;
		}
		if(json.contains("activeParameter")) {
			auto activeParameter = new unsigned int(json.value("activeParameter").toVariant().value<unsigned int>());
			m_activeParameter = activeParameter;
		}

	}
};
class SignatureHelp: virtual public GoPlsParams {
	QList<SignatureInformation*> *m_signatures = nullptr;
	unsigned int *m_activeParameter = nullptr;
	unsigned int *m_activeSignature = nullptr;

public:
	SignatureHelp() = default;
	virtual ~SignatureHelp() {
		if(m_signatures) { qDeleteAll(*m_signatures); delete m_signatures; }
		if(m_activeParameter) { delete m_activeParameter; }
		if(m_activeSignature) { delete m_activeSignature; }
	}

	void setSignatures(QList<SignatureInformation*> *signatures) { m_signatures = signatures; }
	QList<SignatureInformation*> *getSignatures() { return m_signatures; }
	void setActiveParameter(unsigned int *activeParameter) { m_activeParameter = activeParameter; }
	unsigned int *getActiveParameter() { return m_activeParameter; }
	void setActiveSignature(unsigned int *activeSignature) { m_activeSignature = activeSignature; }
	unsigned int *getActiveSignature() { return m_activeSignature; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_signatures) {
				QJsonArray signatures;
			for(auto it = m_signatures->cbegin(); it != m_signatures->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				signatures.append(item);
			}

			obj.insert("signatures", signatures);
		}
		if(m_activeParameter) {
			QJsonValue activeParameter = QJsonValue::fromVariant(QVariant::fromValue(*m_activeParameter));
			obj.insert("activeParameter", activeParameter);
		}
		if(m_activeSignature) {
			QJsonValue activeSignature = QJsonValue::fromVariant(QVariant::fromValue(*m_activeSignature));
			obj.insert("activeSignature", activeSignature);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("signatures")) {
			 QJsonArray arr = json.value("signatures").toArray();
			auto signatures = new QList<SignatureInformation*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new SignatureInformation();
			item->fromJson((m_item).toObject());
				signatures->append(item);
			}
			m_signatures = signatures;
		}
		if(json.contains("activeParameter")) {
			auto activeParameter = new unsigned int(json.value("activeParameter").toVariant().value<unsigned int>());
			m_activeParameter = activeParameter;
		}
		if(json.contains("activeSignature")) {
			auto activeSignature = new unsigned int(json.value("activeSignature").toVariant().value<unsigned int>());
			m_activeSignature = activeSignature;
		}

	}
};
const QString CommentFoldingRange = "comment";
typedef CompletionTextDocumentClientCapabilities TextDocumentClientCapabilitiesCompletion;
const QString MethodShowDocument = "window/showDocument";
const QString MethodTextDocumentPrepareCallHierarchy = "textDocument/prepareCallHierarchy";
const QString SemanticTokenModifierStatic = "static";
class DocumentSymbolParams:  public WorkDoneProgressParams, public PartialResultParams {
	TextDocumentIdentifier *m_textDocument = nullptr;

public:
	DocumentSymbolParams() = default;
	virtual ~DocumentSymbolParams() {
		if(m_textDocument) { delete m_textDocument; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}

	}
};
class PrepareRenameParams:  public TextDocumentPositionParams {

public:
	PrepareRenameParams() = default;
	virtual ~PrepareRenameParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);

	}
};
const QString MethodShutdown = "shutdown";
const QString SASSLanguage = "sass";
typedef double WatchKind;
class FileSystemWatcher: virtual public GoPlsParams {
	QString *m_globPattern = nullptr;
	WatchKind *m_kind = nullptr;

public:
	FileSystemWatcher() = default;
	virtual ~FileSystemWatcher() {
		if(m_globPattern) { delete m_globPattern; }
		if(m_kind) { delete m_kind; }
	}

	void setGlobPattern(QString *globPattern) { m_globPattern = globPattern; }
	QString *getGlobPattern() { return m_globPattern; }
	void setKind(WatchKind *kind) { m_kind = kind; }
	WatchKind *getKind() { return m_kind; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_globPattern) {
			QJsonValue globPattern = QJsonValue::fromVariant(QVariant::fromValue(*m_globPattern));
			obj.insert("globPattern", globPattern);
		}
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("globPattern")) {
			auto globPattern = new QString(json.value("globPattern").toString());
			m_globPattern = globPattern;
		}
		if(json.contains("kind")) {
			auto kind = new double(json.value("kind").toVariant().value<double>());
			m_kind = kind;
		}

	}
};
class DidChangeWatchedFilesRegistrationOptions: virtual public GoPlsParams {
	QList<FileSystemWatcher*> *m_watchers = nullptr;

public:
	DidChangeWatchedFilesRegistrationOptions() = default;
	virtual ~DidChangeWatchedFilesRegistrationOptions() {
		if(m_watchers) { qDeleteAll(*m_watchers); delete m_watchers; }
	}

	void setWatchers(QList<FileSystemWatcher*> *watchers) { m_watchers = watchers; }
	QList<FileSystemWatcher*> *getWatchers() { return m_watchers; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_watchers) {
				QJsonArray watchers;
			for(auto it = m_watchers->cbegin(); it != m_watchers->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				watchers.append(item);
			}

			obj.insert("watchers", watchers);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("watchers")) {
			 QJsonArray arr = json.value("watchers").toArray();
			auto watchers = new QList<FileSystemWatcher*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new FileSystemWatcher();
			item->fromJson((m_item).toObject());
				watchers->append(item);
			}
			m_watchers = watchers;
		}

	}
};
class FoldingRangeOptions:  public WorkDoneProgressOptions {

public:
	FoldingRangeOptions() = default;
	virtual ~FoldingRangeOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
const int CompletionItemKindConstructor = 4;
const QString SemanticTokenString = "string";
class WorkDoneProgressReport: virtual public GoPlsParams {
	WorkDoneProgressKind *m_kind = nullptr;
	bool *m_cancellable = nullptr;
	QString *m_message = nullptr;
	unsigned int *m_percentage = nullptr;

public:
	WorkDoneProgressReport() = default;
	virtual ~WorkDoneProgressReport() {
		if(m_kind) { delete m_kind; }
		if(m_cancellable) { delete m_cancellable; }
		if(m_message) { delete m_message; }
		if(m_percentage) { delete m_percentage; }
	}

	void setKind(WorkDoneProgressKind *kind) { m_kind = kind; }
	WorkDoneProgressKind *getKind() { return m_kind; }
	void setCancellable(bool *cancellable) { m_cancellable = cancellable; }
	bool *getCancellable() { return m_cancellable; }
	void setMessage(QString *message) { m_message = message; }
	QString *getMessage() { return m_message; }
	void setPercentage(unsigned int *percentage) { m_percentage = percentage; }
	unsigned int *getPercentage() { return m_percentage; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_cancellable) {
			QJsonValue cancellable = QJsonValue::fromVariant(QVariant::fromValue(*m_cancellable));
			obj.insert("cancellable", cancellable);
		}
		if(m_message) {
			QJsonValue message = QJsonValue::fromVariant(QVariant::fromValue(*m_message));
			obj.insert("message", message);
		}
		if(m_percentage) {
			QJsonValue percentage = QJsonValue::fromVariant(QVariant::fromValue(*m_percentage));
			obj.insert("percentage", percentage);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("kind")) {
			auto kind = new QString(json.value("kind").toString());
			m_kind = kind;
		}
		if(json.contains("cancellable")) {
			auto cancellable = new bool(json.value("cancellable").toVariant().value<bool>());
			m_cancellable = cancellable;
		}
		if(json.contains("message")) {
			auto message = new QString(json.value("message").toString());
			m_message = message;
		}
		if(json.contains("percentage")) {
			auto percentage = new unsigned int(json.value("percentage").toVariant().value<unsigned int>());
			m_percentage = percentage;
		}

	}
};
const QString MethodTextDocumentReferences = "textDocument/references";
const QString MakefileLanguage = "makefile";
typedef TextDocumentSyncClientCapabilities TextDocumentClientCapabilitiesSynchronization;
const QString SemanticTokenRegexp = "regexp";
class ShowMessageParams: virtual public GoPlsParams {
	QString *m_message = nullptr;
	MessageType *m_type = nullptr;

public:
	ShowMessageParams() = default;
	virtual ~ShowMessageParams() {
		if(m_message) { delete m_message; }
		if(m_type) { delete m_type; }
	}

	void setMessage(QString *message) { m_message = message; }
	QString *getMessage() { return m_message; }
	void setType(MessageType *type) { m_type = type; }
	MessageType *getType() { return m_type; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_message) {
			QJsonValue message = QJsonValue::fromVariant(QVariant::fromValue(*m_message));
			obj.insert("message", message);
		}
		if(m_type) {
			QJsonValue type = QJsonValue::fromVariant(QVariant::fromValue(*m_type));
			obj.insert("type", type);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("message")) {
			auto message = new QString(json.value("message").toString());
			m_message = message;
		}
		if(json.contains("type")) {
			auto type = new double(json.value("type").toVariant().value<double>());
			m_type = type;
		}

	}
};
const QString XMLLanguage = "xml";
const QString RenameResourceOperation = "rename";
typedef CodeActionClientCapabilitiesResolveSupport TextDocumentClientCapabilitiesCodeActionResolveSupport;
const QString WorkDoneProgressKindEnd = "end";
const QString MethodTextDocumentDocumentHighlight = "textDocument/documentHighlight";
const QString MethodCompletionItemResolve = "completionItem/resolve";
const QString MethodDocumentLinkResolve = "documentLink/resolve";
class DocumentFormattingOptions:  public WorkDoneProgressOptions {

public:
	DocumentFormattingOptions() = default;
	virtual ~DocumentFormattingOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
typedef double DocumentHighlightKind;
class DocumentHighlight: virtual public GoPlsParams {
	Range *m_range = nullptr;
	DocumentHighlightKind *m_kind = nullptr;

public:
	DocumentHighlight() = default;
	virtual ~DocumentHighlight() {
		if(m_range) { delete m_range; }
		if(m_kind) { delete m_kind; }
	}

	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setKind(DocumentHighlightKind *kind) { m_kind = kind; }
	DocumentHighlightKind *getKind() { return m_kind; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("kind")) {
			auto kind = new double(json.value("kind").toVariant().value<double>());
			m_kind = kind;
		}

	}
};
const QString SemanticTokenMethod = "method";
const QString SemanticTokenFunction = "function";
const QString MethodDidDeleteFiles = "workspace/didDeleteFiles";
class ExecuteCommandRegistrationOptions: virtual public GoPlsParams {
	QList<QString*> *m_commands = nullptr;

public:
	ExecuteCommandRegistrationOptions() = default;
	virtual ~ExecuteCommandRegistrationOptions() {
		if(m_commands) { qDeleteAll(*m_commands); delete m_commands; }
	}

	void setCommands(QList<QString*> *commands) { m_commands = commands; }
	QList<QString*> *getCommands() { return m_commands; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_commands) {
				QJsonArray commands;
			for(auto it = m_commands->cbegin(); it != m_commands->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				commands.append(item);
			}

			obj.insert("commands", commands);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("commands")) {
			 QJsonArray arr = json.value("commands").toArray();
			auto commands = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				commands->append(item);
			}
			m_commands = commands;
		}

	}
};
typedef QList<TokenFormat*> TokenFormats;
const QString UniquenessLevelScheme = "scheme";
const QString MethodWindowLogMessage = "window/logMessage";
typedef CallHierarchyClientCapabilities TextDocumentClientCapabilitiesCallHierarchy;
typedef LinkedEditingRangeClientCapabilities TextDocumentClientCapabilitiesLinkedEditingRange;
typedef HoverTextDocumentClientCapabilities TextDocumentClientCapabilitiesHover;
const int CompletionItemKindField = 5;
const QString ElixirLanguage = "elixir";
class ConfigurationItem: virtual public GoPlsParams {
	QString *m_scopeUri = nullptr;
	QString *m_section = nullptr;

public:
	ConfigurationItem() = default;
	virtual ~ConfigurationItem() {
		if(m_scopeUri) { delete m_scopeUri; }
		if(m_section) { delete m_section; }
	}

	void setScopeUri(QString *scopeUri) { m_scopeUri = scopeUri; }
	QString *getScopeUri() { return m_scopeUri; }
	void setSection(QString *section) { m_section = section; }
	QString *getSection() { return m_section; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_scopeUri) {
			QJsonValue scopeUri = QJsonValue::fromVariant(QVariant::fromValue(*m_scopeUri));
			obj.insert("scopeUri", scopeUri);
		}
		if(m_section) {
			QJsonValue section = QJsonValue::fromVariant(QVariant::fromValue(*m_section));
			obj.insert("section", section);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("scopeUri")) {
			auto scopeUri = new QString(json.value("scopeUri").toString());
			m_scopeUri = scopeUri;
		}
		if(json.contains("section")) {
			auto section = new QString(json.value("section").toString());
			m_section = section;
		}

	}
};
typedef QList<ConfigurationItem*> ConfigurationItems;
typedef QList<SemanticTokensEdit*> SemanticTokensEdits;
class LinkedEditingRangeOptions:  public WorkDoneProgressOptions {

public:
	LinkedEditingRangeOptions() = default;
	virtual ~LinkedEditingRangeOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
class ShowDocumentParams: virtual public GoPlsParams {
	URI *m_uri = nullptr;
	bool *m_external = nullptr;
	bool *m_takeFocus = nullptr;
	Range *m_selection = nullptr;

public:
	ShowDocumentParams() = default;
	virtual ~ShowDocumentParams() {
		if(m_uri) { delete m_uri; }
		if(m_external) { delete m_external; }
		if(m_takeFocus) { delete m_takeFocus; }
		if(m_selection) { delete m_selection; }
	}

	void setUri(URI *uri) { m_uri = uri; }
	URI *getUri() { return m_uri; }
	void setExternal(bool *external) { m_external = external; }
	bool *getExternal() { return m_external; }
	void setTakeFocus(bool *takeFocus) { m_takeFocus = takeFocus; }
	bool *getTakeFocus() { return m_takeFocus; }
	void setSelection(Range *selection) { m_selection = selection; }
	Range *getSelection() { return m_selection; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}
		if(m_external) {
			QJsonValue external = QJsonValue::fromVariant(QVariant::fromValue(*m_external));
			obj.insert("external", external);
		}
		if(m_takeFocus) {
			QJsonValue takeFocus = QJsonValue::fromVariant(QVariant::fromValue(*m_takeFocus));
			obj.insert("takeFocus", takeFocus);
		}
		if(m_selection) {
			QJsonObject selection = m_selection->toJson();
			obj.insert("selection", selection);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}
		if(json.contains("external")) {
			auto external = new bool(json.value("external").toVariant().value<bool>());
			m_external = external;
		}
		if(json.contains("takeFocus")) {
			auto takeFocus = new bool(json.value("takeFocus").toVariant().value<bool>());
			m_takeFocus = takeFocus;
		}
		if(json.contains("selection")) {
			auto selection = new Range();
			selection->fromJson(json.value("selection").toObject());
			m_selection = selection;
		}

	}
};
const int SymbolKindConstructor = 9;
const int CompletionItemKindColor = 16;
const QString FailureHandlingKindAbort = "abort";
typedef double CompletionTriggerKind;
class ColorPresentationParams:  public WorkDoneProgressParams, public PartialResultParams {
	TextDocumentIdentifier *m_textDocument = nullptr;
	Color *m_color = nullptr;
	Range *m_range = nullptr;

public:
	ColorPresentationParams() = default;
	virtual ~ColorPresentationParams() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_color) { delete m_color; }
		if(m_range) { delete m_range; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setColor(Color *color) { m_color = color; }
	Color *getColor() { return m_color; }
	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_color) {
			QJsonObject color = m_color->toJson();
			obj.insert("color", color);
		}
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("color")) {
			auto color = new Color();
			color->fromJson(json.value("color").toObject());
			m_color = color;
		}
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}

	}
};
const QString MethodWorkspaceDidChangeWorkspaceFolders = "workspace/didChangeWorkspaceFolders";
const QString MethodTextDocumentCodeLens = "textDocument/codeLens";
const QString MethodTextDocumentFoldingRange = "textDocument/foldingRange";
const QString RefactorRewrite = "refactor.rewrite";
const QString MethodWindowShowMessage = "window/showMessage";
const QString TraceVerbose = "verbose";
class FormattingOptions: virtual public GoPlsParams {
	bool *m_insertSpaces = nullptr;
	unsigned int *m_tabSize = nullptr;
	bool *m_trimTrailingWhitespace = nullptr;
	bool *m_insertFinalNewline = nullptr;
	bool *m_trimFinalNewlines = nullptr;
	QHash<QString,QJsonValue*> *m_key = nullptr;

public:
	FormattingOptions() = default;
	virtual ~FormattingOptions() {
		if(m_insertSpaces) { delete m_insertSpaces; }
		if(m_tabSize) { delete m_tabSize; }
		if(m_trimTrailingWhitespace) { delete m_trimTrailingWhitespace; }
		if(m_insertFinalNewline) { delete m_insertFinalNewline; }
		if(m_trimFinalNewlines) { delete m_trimFinalNewlines; }
		if(m_key) { qDeleteAll(m_key->values()); delete m_key; }
	}

	void setInsertSpaces(bool *insertSpaces) { m_insertSpaces = insertSpaces; }
	bool *getInsertSpaces() { return m_insertSpaces; }
	void setTabSize(unsigned int *tabSize) { m_tabSize = tabSize; }
	unsigned int *getTabSize() { return m_tabSize; }
	void setTrimTrailingWhitespace(bool *trimTrailingWhitespace) { m_trimTrailingWhitespace = trimTrailingWhitespace; }
	bool *getTrimTrailingWhitespace() { return m_trimTrailingWhitespace; }
	void setInsertFinalNewline(bool *insertFinalNewline) { m_insertFinalNewline = insertFinalNewline; }
	bool *getInsertFinalNewline() { return m_insertFinalNewline; }
	void setTrimFinalNewlines(bool *trimFinalNewlines) { m_trimFinalNewlines = trimFinalNewlines; }
	bool *getTrimFinalNewlines() { return m_trimFinalNewlines; }
	void setKey(QHash<QString,QJsonValue*> *key) { m_key = key; }
	QHash<QString,QJsonValue*> *getKey() { return m_key; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_insertSpaces) {
			QJsonValue insertSpaces = QJsonValue::fromVariant(QVariant::fromValue(*m_insertSpaces));
			obj.insert("insertSpaces", insertSpaces);
		}
		if(m_tabSize) {
			QJsonValue tabSize = QJsonValue::fromVariant(QVariant::fromValue(*m_tabSize));
			obj.insert("tabSize", tabSize);
		}
		if(m_trimTrailingWhitespace) {
			QJsonValue trimTrailingWhitespace = QJsonValue::fromVariant(QVariant::fromValue(*m_trimTrailingWhitespace));
			obj.insert("trimTrailingWhitespace", trimTrailingWhitespace);
		}
		if(m_insertFinalNewline) {
			QJsonValue insertFinalNewline = QJsonValue::fromVariant(QVariant::fromValue(*m_insertFinalNewline));
			obj.insert("insertFinalNewline", insertFinalNewline);
		}
		if(m_trimFinalNewlines) {
			QJsonValue trimFinalNewlines = QJsonValue::fromVariant(QVariant::fromValue(*m_trimFinalNewlines));
			obj.insert("trimFinalNewlines", trimFinalNewlines);
		}
		if(m_key) {
			QJsonObject key;
			for(auto hash_it = m_key->cbegin(); hash_it != m_key->cend(); ++hash_it) {
				auto m_hash_item = *hash_it;
				QJsonValue hash_item = *m_hash_item;
				key.insert(hash_it.key(), hash_item);
			}
			obj.insert("key", key);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("insertSpaces")) {
			auto insertSpaces = new bool(json.value("insertSpaces").toVariant().value<bool>());
			m_insertSpaces = insertSpaces;
		}
		if(json.contains("tabSize")) {
			auto tabSize = new unsigned int(json.value("tabSize").toVariant().value<unsigned int>());
			m_tabSize = tabSize;
		}
		if(json.contains("trimTrailingWhitespace")) {
			auto trimTrailingWhitespace = new bool(json.value("trimTrailingWhitespace").toVariant().value<bool>());
			m_trimTrailingWhitespace = trimTrailingWhitespace;
		}
		if(json.contains("insertFinalNewline")) {
			auto insertFinalNewline = new bool(json.value("insertFinalNewline").toVariant().value<bool>());
			m_insertFinalNewline = insertFinalNewline;
		}
		if(json.contains("trimFinalNewlines")) {
			auto trimFinalNewlines = new bool(json.value("trimFinalNewlines").toVariant().value<bool>());
			m_trimFinalNewlines = trimFinalNewlines;
		}
		if(json.contains("key")) {
			 QJsonObject obj = json.value("key").toObject();
			QStringList objKeys = obj.keys();
			auto key = new QHash<QString,QJsonValue*>();
			for(auto m_it = objKeys.cbegin(); m_it != objKeys.cend(); ++m_it) {
				auto hash = new QJsonValue(obj.value(*m_it));
				key->insert(*m_it, hash);
			}
			m_key = key;
		}

	}
};
class DocumentFormattingParams:  public WorkDoneProgressParams {
	FormattingOptions *m_options = nullptr;
	TextDocumentIdentifier *m_textDocument = nullptr;

public:
	DocumentFormattingParams() = default;
	virtual ~DocumentFormattingParams() {
		if(m_options) { delete m_options; }
		if(m_textDocument) { delete m_textDocument; }
	}

	void setOptions(FormattingOptions *options) { m_options = options; }
	FormattingOptions *getOptions() { return m_options; }
	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		if(m_options) {
			QJsonObject options = m_options->toJson();
			obj.insert("options", options);
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		if(json.contains("options")) {
			auto options = new FormattingOptions();
			options->fromJson(json.value("options").toObject());
			m_options = options;
		}
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}

	}
};
typedef DocumentColorClientCapabilities TextDocumentClientCapabilitiesColorProvider;
const int TextDocumentSaveReasonManual = 1;
typedef QList<InsertTextMode*> InsertTextModes;
class LogMessageParams: virtual public GoPlsParams {
	QString *m_message = nullptr;
	MessageType *m_type = nullptr;

public:
	LogMessageParams() = default;
	virtual ~LogMessageParams() {
		if(m_message) { delete m_message; }
		if(m_type) { delete m_type; }
	}

	void setMessage(QString *message) { m_message = message; }
	QString *getMessage() { return m_message; }
	void setType(MessageType *type) { m_type = type; }
	MessageType *getType() { return m_type; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_message) {
			QJsonValue message = QJsonValue::fromVariant(QVariant::fromValue(*m_message));
			obj.insert("message", message);
		}
		if(m_type) {
			QJsonValue type = QJsonValue::fromVariant(QVariant::fromValue(*m_type));
			obj.insert("type", type);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("message")) {
			auto message = new QString(json.value("message").toString());
			m_message = message;
		}
		if(json.contains("type")) {
			auto type = new double(json.value("type").toVariant().value<double>());
			m_type = type;
		}

	}
};
const int TextDocumentSyncKindNone = 0;
class ProgressParams: virtual public GoPlsParams {
	ProgressToken *m_token = nullptr;
	QJsonValue *m_value = nullptr;

public:
	ProgressParams() = default;
	virtual ~ProgressParams() {
		if(m_token) { delete m_token; }
		if(m_value) { delete m_value; }
	}

	void setToken(ProgressToken *token) { m_token = token; }
	ProgressToken *getToken() { return m_token; }
	void setValue(QJsonValue *value) { m_value = value; }
	QJsonValue *getValue() { return m_value; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_token) {
			QJsonValue token = *m_token;
			obj.insert("token", token);
		}
		if(m_value) {
			QJsonValue value = *m_value;
			obj.insert("value", value);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("token")) {
			auto token = new QJsonValue(json.value("token"));
			m_token = token;
		}
		if(json.contains("value")) {
			auto value = new QJsonValue(json.value("value"));
			m_value = value;
		}

	}
};
const QString Markdown = "markdown";
const QString RLanguage = "r";
typedef TypeDefinitionTextDocumentClientCapabilities TextDocumentClientCapabilitiesTypeDefinition;
class LinkedEditingRangeRegistrationOptions:  public TextDocumentRegistrationOptions, public LinkedEditingRangeOptions, public StaticRegistrationOptions {

public:
	LinkedEditingRangeRegistrationOptions() = default;
	virtual ~LinkedEditingRangeRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject LinkedEditingRangeOptions_json = LinkedEditingRangeOptions::toJson();
		for(const QString &key: LinkedEditingRangeOptions_json.keys()) {
			obj.insert(key, LinkedEditingRangeOptions_json.value(key));
		}
		QJsonObject StaticRegistrationOptions_json = StaticRegistrationOptions::toJson();
		for(const QString &key: StaticRegistrationOptions_json.keys()) {
			obj.insert(key, StaticRegistrationOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		LinkedEditingRangeOptions::fromJson(json);
		StaticRegistrationOptions::fromJson(json);

	}
};
const QString MethodCallHierarchyIncomingCalls = "callHierarchy/incomingCalls";
typedef QJsonValue Server;
class DocumentHighlightOptions:  public WorkDoneProgressOptions {

public:
	DocumentHighlightOptions() = default;
	virtual ~DocumentHighlightOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
const QString UniquenessLevelProject = "project";
typedef CompletionTextDocumentClientCapabilitiesItemResolveSupport TextDocumentClientCapabilitiesCompletionItemResolveSupport;
class DocumentSymbolOptions:  public WorkDoneProgressOptions {
	QString *m_label = nullptr;

public:
	DocumentSymbolOptions() = default;
	virtual ~DocumentSymbolOptions() {
		if(m_label) { delete m_label; }
	}

	void setLabel(QString *label) { m_label = label; }
	QString *getLabel() { return m_label; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}
		if(m_label) {
			QJsonValue label = QJsonValue::fromVariant(QVariant::fromValue(*m_label));
			obj.insert("label", label);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);
		if(json.contains("label")) {
			auto label = new QString(json.value("label").toString());
			m_label = label;
		}

	}
};
typedef QString FoldingRangeKind;
class FoldingRange: virtual public GoPlsParams {
	unsigned int *m_startLine = nullptr;
	unsigned int *m_startCharacter = nullptr;
	unsigned int *m_endLine = nullptr;
	unsigned int *m_endCharacter = nullptr;
	FoldingRangeKind *m_kind = nullptr;

public:
	FoldingRange() = default;
	virtual ~FoldingRange() {
		if(m_startLine) { delete m_startLine; }
		if(m_startCharacter) { delete m_startCharacter; }
		if(m_endLine) { delete m_endLine; }
		if(m_endCharacter) { delete m_endCharacter; }
		if(m_kind) { delete m_kind; }
	}

	void setStartLine(unsigned int *startLine) { m_startLine = startLine; }
	unsigned int *getStartLine() { return m_startLine; }
	void setStartCharacter(unsigned int *startCharacter) { m_startCharacter = startCharacter; }
	unsigned int *getStartCharacter() { return m_startCharacter; }
	void setEndLine(unsigned int *endLine) { m_endLine = endLine; }
	unsigned int *getEndLine() { return m_endLine; }
	void setEndCharacter(unsigned int *endCharacter) { m_endCharacter = endCharacter; }
	unsigned int *getEndCharacter() { return m_endCharacter; }
	void setKind(FoldingRangeKind *kind) { m_kind = kind; }
	FoldingRangeKind *getKind() { return m_kind; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_startLine) {
			QJsonValue startLine = QJsonValue::fromVariant(QVariant::fromValue(*m_startLine));
			obj.insert("startLine", startLine);
		}
		if(m_startCharacter) {
			QJsonValue startCharacter = QJsonValue::fromVariant(QVariant::fromValue(*m_startCharacter));
			obj.insert("startCharacter", startCharacter);
		}
		if(m_endLine) {
			QJsonValue endLine = QJsonValue::fromVariant(QVariant::fromValue(*m_endLine));
			obj.insert("endLine", endLine);
		}
		if(m_endCharacter) {
			QJsonValue endCharacter = QJsonValue::fromVariant(QVariant::fromValue(*m_endCharacter));
			obj.insert("endCharacter", endCharacter);
		}
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("startLine")) {
			auto startLine = new unsigned int(json.value("startLine").toVariant().value<unsigned int>());
			m_startLine = startLine;
		}
		if(json.contains("startCharacter")) {
			auto startCharacter = new unsigned int(json.value("startCharacter").toVariant().value<unsigned int>());
			m_startCharacter = startCharacter;
		}
		if(json.contains("endLine")) {
			auto endLine = new unsigned int(json.value("endLine").toVariant().value<unsigned int>());
			m_endLine = endLine;
		}
		if(json.contains("endCharacter")) {
			auto endCharacter = new unsigned int(json.value("endCharacter").toVariant().value<unsigned int>());
			m_endCharacter = endCharacter;
		}
		if(json.contains("kind")) {
			auto kind = new QString(json.value("kind").toString());
			m_kind = kind;
		}

	}
};
const QString SourceOrganizeImports = "source.organizeImports";
const QString WorkDoneProgressKindBegin = "begin";
typedef QList<TextDocumentEdit*> TextDocumentEdits;
class TypeDefinitionRegistrationOptions:  public TextDocumentRegistrationOptions, public TypeDefinitionOptions, public StaticRegistrationOptions {

public:
	TypeDefinitionRegistrationOptions() = default;
	virtual ~TypeDefinitionRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject TypeDefinitionOptions_json = TypeDefinitionOptions::toJson();
		for(const QString &key: TypeDefinitionOptions_json.keys()) {
			obj.insert(key, TypeDefinitionOptions_json.value(key));
		}
		QJsonObject StaticRegistrationOptions_json = StaticRegistrationOptions::toJson();
		for(const QString &key: StaticRegistrationOptions_json.keys()) {
			obj.insert(key, StaticRegistrationOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		TypeDefinitionOptions::fromJson(json);
		StaticRegistrationOptions::fromJson(json);

	}
};
const int SignatureHelpTriggerKindInvoked = 1;
typedef DocumentHighlightClientCapabilities TextDocumentClientCapabilitiesDocumentHighlight;
typedef CodeActionClientCapabilitiesKind TextDocumentClientCapabilitiesCodeActionKind;
const QString MethodSemanticTokensFull = "textDocument/semanticTokens/full";
const QString ShellscriptLanguage = "shellscript";
class CallHierarchyOptions:  public WorkDoneProgressOptions {

public:
	CallHierarchyOptions() = default;
	virtual ~CallHierarchyOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
class SignatureHelpRegistrationOptions:  public TextDocumentRegistrationOptions {
	QList<QString*> *m_triggerCharacters = nullptr;

public:
	SignatureHelpRegistrationOptions() = default;
	virtual ~SignatureHelpRegistrationOptions() {
		if(m_triggerCharacters) { qDeleteAll(*m_triggerCharacters); delete m_triggerCharacters; }
	}

	void setTriggerCharacters(QList<QString*> *triggerCharacters) { m_triggerCharacters = triggerCharacters; }
	QList<QString*> *getTriggerCharacters() { return m_triggerCharacters; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		if(m_triggerCharacters) {
				QJsonArray triggerCharacters;
			for(auto it = m_triggerCharacters->cbegin(); it != m_triggerCharacters->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				triggerCharacters.append(item);
			}

			obj.insert("triggerCharacters", triggerCharacters);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		if(json.contains("triggerCharacters")) {
			 QJsonArray arr = json.value("triggerCharacters").toArray();
			auto triggerCharacters = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				triggerCharacters->append(item);
			}
			m_triggerCharacters = triggerCharacters;
		}

	}
};
const int InsertTextFormatSnippet = 2;
const QString TokenFormatRelative = "relative";
const QString LatexLanguage = "latex";
class DocumentOnTypeFormattingParams: virtual public GoPlsParams {
	TextDocumentIdentifier *m_textDocument = nullptr;
	Position *m_position = nullptr;
	QString *m_ch = nullptr;
	FormattingOptions *m_options = nullptr;

public:
	DocumentOnTypeFormattingParams() = default;
	virtual ~DocumentOnTypeFormattingParams() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_position) { delete m_position; }
		if(m_ch) { delete m_ch; }
		if(m_options) { delete m_options; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setPosition(Position *position) { m_position = position; }
	Position *getPosition() { return m_position; }
	void setCh(QString *ch) { m_ch = ch; }
	QString *getCh() { return m_ch; }
	void setOptions(FormattingOptions *options) { m_options = options; }
	FormattingOptions *getOptions() { return m_options; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_position) {
			QJsonObject position = m_position->toJson();
			obj.insert("position", position);
		}
		if(m_ch) {
			QJsonValue ch = QJsonValue::fromVariant(QVariant::fromValue(*m_ch));
			obj.insert("ch", ch);
		}
		if(m_options) {
			QJsonObject options = m_options->toJson();
			obj.insert("options", options);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("position")) {
			auto position = new Position();
			position->fromJson(json.value("position").toObject());
			m_position = position;
		}
		if(json.contains("ch")) {
			auto ch = new QString(json.value("ch").toString());
			m_ch = ch;
		}
		if(json.contains("options")) {
			auto options = new FormattingOptions();
			options->fromJson(json.value("options").toObject());
			m_options = options;
		}

	}
};
const QString ClojureLanguage = "clojure";
class TypeDefinitionParams:  public TextDocumentPositionParams, public WorkDoneProgressParams, public PartialResultParams {

public:
	TypeDefinitionParams() = default;
	virtual ~TypeDefinitionParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);

	}
};
const int SymbolKindProperty = 7;
typedef ShowMessageRequestClientCapabilities ClientCapabilitiesShowMessageRequest;
typedef QList<TextEdit*> TextEdits;
const QString MethodCodeLensRefresh = "workspace/codeLens/refresh";
const int TextDocumentSyncKindFull = 1;
const int SymbolKindStruct = 23;
class RenameFileOptions: virtual public GoPlsParams {
	bool *m_overwrite = nullptr;
	bool *m_ignoreIfExists = nullptr;

public:
	RenameFileOptions() = default;
	virtual ~RenameFileOptions() {
		if(m_overwrite) { delete m_overwrite; }
		if(m_ignoreIfExists) { delete m_ignoreIfExists; }
	}

	void setOverwrite(bool *overwrite) { m_overwrite = overwrite; }
	bool *getOverwrite() { return m_overwrite; }
	void setIgnoreIfExists(bool *ignoreIfExists) { m_ignoreIfExists = ignoreIfExists; }
	bool *getIgnoreIfExists() { return m_ignoreIfExists; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_overwrite) {
			QJsonValue overwrite = QJsonValue::fromVariant(QVariant::fromValue(*m_overwrite));
			obj.insert("overwrite", overwrite);
		}
		if(m_ignoreIfExists) {
			QJsonValue ignoreIfExists = QJsonValue::fromVariant(QVariant::fromValue(*m_ignoreIfExists));
			obj.insert("ignoreIfExists", ignoreIfExists);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("overwrite")) {
			auto overwrite = new bool(json.value("overwrite").toVariant().value<bool>());
			m_overwrite = overwrite;
		}
		if(json.contains("ignoreIfExists")) {
			auto ignoreIfExists = new bool(json.value("ignoreIfExists").toVariant().value<bool>());
			m_ignoreIfExists = ignoreIfExists;
		}

	}
};
class RenameFile: virtual public GoPlsParams {
	ResourceOperationKind *m_kind = nullptr;
	DocumentURI *m_oldUri = nullptr;
	DocumentURI *m_newUri = nullptr;
	RenameFileOptions *m_options = nullptr;
	ChangeAnnotationIdentifier *m_annotationId = nullptr;

public:
	RenameFile() = default;
	virtual ~RenameFile() {
		if(m_kind) { delete m_kind; }
		if(m_oldUri) { delete m_oldUri; }
		if(m_newUri) { delete m_newUri; }
		if(m_options) { delete m_options; }
		if(m_annotationId) { delete m_annotationId; }
	}

	void setKind(ResourceOperationKind *kind) { m_kind = kind; }
	ResourceOperationKind *getKind() { return m_kind; }
	void setOldUri(DocumentURI *oldUri) { m_oldUri = oldUri; }
	DocumentURI *getOldUri() { return m_oldUri; }
	void setNewUri(DocumentURI *newUri) { m_newUri = newUri; }
	DocumentURI *getNewUri() { return m_newUri; }
	void setOptions(RenameFileOptions *options) { m_options = options; }
	RenameFileOptions *getOptions() { return m_options; }
	void setAnnotationId(ChangeAnnotationIdentifier *annotationId) { m_annotationId = annotationId; }
	ChangeAnnotationIdentifier *getAnnotationId() { return m_annotationId; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_oldUri) {
			QJsonValue oldUri = QJsonValue::fromVariant(QVariant::fromValue(*m_oldUri));
			obj.insert("oldUri", oldUri);
		}
		if(m_newUri) {
			QJsonValue newUri = QJsonValue::fromVariant(QVariant::fromValue(*m_newUri));
			obj.insert("newUri", newUri);
		}
		if(m_options) {
			QJsonObject options = m_options->toJson();
			obj.insert("options", options);
		}
		if(m_annotationId) {
			QJsonValue annotationId = QJsonValue::fromVariant(QVariant::fromValue(*m_annotationId));
			obj.insert("annotationId", annotationId);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("kind")) {
			auto kind = new QString(json.value("kind").toString());
			m_kind = kind;
		}
		if(json.contains("oldUri")) {
			auto oldUri = new QString(json.value("oldUri").toString());
			m_oldUri = oldUri;
		}
		if(json.contains("newUri")) {
			auto newUri = new QString(json.value("newUri").toString());
			m_newUri = newUri;
		}
		if(json.contains("options")) {
			auto options = new RenameFileOptions();
			options->fromJson(json.value("options").toObject());
			m_options = options;
		}
		if(json.contains("annotationId")) {
			auto annotationId = new QString(json.value("annotationId").toString());
			m_annotationId = annotationId;
		}

	}
};
const int InsertTextModeAdjustIndentation = 2;
const int SymbolKindNumber = 16;
typedef QHash<DocumentURI,QList<TextEdit*>*> TextEditsMap;
const QString MethodDidCreateFiles = "workspace/didCreateFiles";
const QString BatLanguage = "bat";
typedef QList<Unregistration*> Unregisterations;
class SelectionRangeParams:  public WorkDoneProgressParams, public PartialResultParams {
	TextDocumentIdentifier *m_textDocument = nullptr;
	QList<Position*> *m_positions = nullptr;

public:
	SelectionRangeParams() = default;
	virtual ~SelectionRangeParams() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_positions) { qDeleteAll(*m_positions); delete m_positions; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setPositions(QList<Position*> *positions) { m_positions = positions; }
	QList<Position*> *getPositions() { return m_positions; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_positions) {
				QJsonArray positions;
			for(auto it = m_positions->cbegin(); it != m_positions->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				positions.append(item);
			}

			obj.insert("positions", positions);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("positions")) {
			 QJsonArray arr = json.value("positions").toArray();
			auto positions = new QList<Position*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new Position();
			item->fromJson((m_item).toObject());
				positions->append(item);
			}
			m_positions = positions;
		}

	}
};
const int CompletionItemKindKeyword = 14;
typedef DocumentLinkClientCapabilities TextDocumentClientCapabilitiesDocumentLink;
const QString MethodSemanticTokensRange = "textDocument/semanticTokens/range";
typedef CodeLensWorkspaceClientCapabilities WorkspaceClientCapabilitiesCodeLens;
const int DiagnosticSeverityInformation = 3;
typedef QList<FileOperationFilter*> FileOperationFilters;
const QString FileOperationPatternKindFolder = "folder";
const int CompletionItemKindEnumMember = 20;
const QString MethodInitialized = "initialized";
const QString MethodTextDocumentCompletion = "textDocument/completion";
class TextDocumentSaveRegistrationOptions:  public TextDocumentRegistrationOptions {
	bool *m_includeText = nullptr;

public:
	TextDocumentSaveRegistrationOptions() = default;
	virtual ~TextDocumentSaveRegistrationOptions() {
		if(m_includeText) { delete m_includeText; }
	}

	void setIncludeText(bool *includeText) { m_includeText = includeText; }
	bool *getIncludeText() { return m_includeText; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		if(m_includeText) {
			QJsonValue includeText = QJsonValue::fromVariant(QVariant::fromValue(*m_includeText));
			obj.insert("includeText", includeText);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		if(json.contains("includeText")) {
			auto includeText = new bool(json.value("includeText").toVariant().value<bool>());
			m_includeText = includeText;
		}

	}
};
const int WatchKindDelete = 4;
const int MessageTypeInfo = 3;
const QString ImportsFoldingRange = "imports";
typedef QString SemanticTokenTypes;
const QString UniquenessLevelGroup = "group";
const QString VBLanguage = "vb";
const QString DartLanguage = "dart";
const QString SemanticTokenModifierDocumentation = "documentation";
const QString GitCommitLanguage = "git-commit";
const QString SemanticTokenModifierAsync = "async";
const int TextDocumentSyncKindIncremental = 2;
const int CompletionItemKindProperty = 10;
const int CompletionItemKindValue = 12;
typedef PublishDiagnosticsClientCapabilities TextDocumentClientCapabilitiesPublishDiagnostics;
class CancelParams: virtual public GoPlsParams {
	QJsonValue *m_id = nullptr;

public:
	CancelParams() = default;
	virtual ~CancelParams() {
		if(m_id) { delete m_id; }
	}

	void setId(QJsonValue *id) { m_id = id; }
	QJsonValue *getId() { return m_id; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_id) {
			QJsonValue id = *m_id;
			obj.insert("id", id);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("id")) {
			auto id = new QJsonValue(json.value("id"));
			m_id = id;
		}

	}
};
class ImplementationOptions:  public WorkDoneProgressOptions {

public:
	ImplementationOptions() = default;
	virtual ~ImplementationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
class ImplementationRegistrationOptions:  public TextDocumentRegistrationOptions, public ImplementationOptions, public StaticRegistrationOptions {

public:
	ImplementationRegistrationOptions() = default;
	virtual ~ImplementationRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject ImplementationOptions_json = ImplementationOptions::toJson();
		for(const QString &key: ImplementationOptions_json.keys()) {
			obj.insert(key, ImplementationOptions_json.value(key));
		}
		QJsonObject StaticRegistrationOptions_json = StaticRegistrationOptions::toJson();
		for(const QString &key: StaticRegistrationOptions_json.keys()) {
			obj.insert(key, StaticRegistrationOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		ImplementationOptions::fromJson(json);
		StaticRegistrationOptions::fromJson(json);

	}
};
const QString Source = "source";
class DidChangeTextDocumentParams: virtual public GoPlsParams {
	VersionedTextDocumentIdentifier *m_textDocument = nullptr;
	QList<TextDocumentContentChangeEvent*> *m_contentChanges = nullptr;

public:
	DidChangeTextDocumentParams() = default;
	virtual ~DidChangeTextDocumentParams() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_contentChanges) { qDeleteAll(*m_contentChanges); delete m_contentChanges; }
	}

	void setTextDocument(VersionedTextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	VersionedTextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setContentChanges(QList<TextDocumentContentChangeEvent*> *contentChanges) { m_contentChanges = contentChanges; }
	QList<TextDocumentContentChangeEvent*> *getContentChanges() { return m_contentChanges; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_contentChanges) {
				QJsonArray contentChanges;
			for(auto it = m_contentChanges->cbegin(); it != m_contentChanges->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				contentChanges.append(item);
			}

			obj.insert("contentChanges", contentChanges);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("textDocument")) {
			auto textDocument = new VersionedTextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("contentChanges")) {
			 QJsonArray arr = json.value("contentChanges").toArray();
			auto contentChanges = new QList<TextDocumentContentChangeEvent*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new TextDocumentContentChangeEvent();
			item->fromJson((m_item).toObject());
				contentChanges->append(item);
			}
			m_contentChanges = contentChanges;
		}

	}
};
class FileRename: virtual public GoPlsParams {
	QString *m_oldUri = nullptr;
	QString *m_newUri = nullptr;

public:
	FileRename() = default;
	virtual ~FileRename() {
		if(m_oldUri) { delete m_oldUri; }
		if(m_newUri) { delete m_newUri; }
	}

	void setOldUri(QString *oldUri) { m_oldUri = oldUri; }
	QString *getOldUri() { return m_oldUri; }
	void setNewUri(QString *newUri) { m_newUri = newUri; }
	QString *getNewUri() { return m_newUri; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_oldUri) {
			QJsonValue oldUri = QJsonValue::fromVariant(QVariant::fromValue(*m_oldUri));
			obj.insert("oldUri", oldUri);
		}
		if(m_newUri) {
			QJsonValue newUri = QJsonValue::fromVariant(QVariant::fromValue(*m_newUri));
			obj.insert("newUri", newUri);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("oldUri")) {
			auto oldUri = new QString(json.value("oldUri").toString());
			m_oldUri = oldUri;
		}
		if(json.contains("newUri")) {
			auto newUri = new QString(json.value("newUri").toString());
			m_newUri = newUri;
		}

	}
};
typedef QList<FileRename*> FileRenames;
class DeleteFilesParams: virtual public GoPlsParams {
	QList<FileDelete*> *m_files = nullptr;

public:
	DeleteFilesParams() = default;
	virtual ~DeleteFilesParams() {
		if(m_files) { qDeleteAll(*m_files); delete m_files; }
	}

	void setFiles(QList<FileDelete*> *files) { m_files = files; }
	QList<FileDelete*> *getFiles() { return m_files; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_files) {
				QJsonArray files;
			for(auto it = m_files->cbegin(); it != m_files->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				files.append(item);
			}

			obj.insert("files", files);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("files")) {
			 QJsonArray arr = json.value("files").toArray();
			auto files = new QList<FileDelete*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new FileDelete();
			item->fromJson((m_item).toObject());
				files->append(item);
			}
			m_files = files;
		}

	}
};
typedef RenameClientCapabilities TextDocumentClientCapabilitiesRename;
typedef SemanticTokensClientCapabilities TextDocumentClientCapabilitiesSemanticTokens;
const QString MethodWorkspaceSymbol = "workspace/symbol";
const QString SemanticTokenModifierAbstract = "abstract";
const QString UniquenessLevelGlobal = "global";
class ImplementationParams:  public TextDocumentPositionParams, public WorkDoneProgressParams, public PartialResultParams {

public:
	ImplementationParams() = default;
	virtual ~ImplementationParams() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);

	}
};
const QString SemanticTokenKeyword = "keyword";
class WorkDoneProgressEnd: virtual public GoPlsParams {
	WorkDoneProgressKind *m_kind = nullptr;
	QString *m_message = nullptr;

public:
	WorkDoneProgressEnd() = default;
	virtual ~WorkDoneProgressEnd() {
		if(m_kind) { delete m_kind; }
		if(m_message) { delete m_message; }
	}

	void setKind(WorkDoneProgressKind *kind) { m_kind = kind; }
	WorkDoneProgressKind *getKind() { return m_kind; }
	void setMessage(QString *message) { m_message = message; }
	QString *getMessage() { return m_message; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_message) {
			QJsonValue message = QJsonValue::fromVariant(QVariant::fromValue(*m_message));
			obj.insert("message", message);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("kind")) {
			auto kind = new QString(json.value("kind").toString());
			m_kind = kind;
		}
		if(json.contains("message")) {
			auto message = new QString(json.value("message").toString());
			m_message = message;
		}

	}
};
const QString SemanticTokenModifierDeclaration = "declaration";
typedef QList<TextDocumentContentChangeEvent*> TextDocumentContentChangeEvents;
class DocumentColorOptions:  public WorkDoneProgressOptions {

public:
	DocumentColorOptions() = default;
	virtual ~DocumentColorOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
const int SymbolKindKey = 20;
const QString MethodWorkspaceWorkspaceFolders = "workspace/workspaceFolders";
const QString MethodWorkDoneProgressCancel = "window/workDoneProgress/cancel";
const QString TraceOff = "off";
typedef CompletionTextDocumentClientCapabilitiesItemKind TextDocumentClientCapabilitiesCompletionItemKind;
typedef QList<FileSystemWatcher*> FileSystemWatchers;
const QString BibtexLanguage = "bibtex";
const QString JSONLanguage = "json";
class CompletionContext: virtual public GoPlsParams {
	QString *m_triggerCharacter = nullptr;
	CompletionTriggerKind *m_triggerKind = nullptr;

public:
	CompletionContext() = default;
	virtual ~CompletionContext() {
		if(m_triggerCharacter) { delete m_triggerCharacter; }
		if(m_triggerKind) { delete m_triggerKind; }
	}

	void setTriggerCharacter(QString *triggerCharacter) { m_triggerCharacter = triggerCharacter; }
	QString *getTriggerCharacter() { return m_triggerCharacter; }
	void setTriggerKind(CompletionTriggerKind *triggerKind) { m_triggerKind = triggerKind; }
	CompletionTriggerKind *getTriggerKind() { return m_triggerKind; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_triggerCharacter) {
			QJsonValue triggerCharacter = QJsonValue::fromVariant(QVariant::fromValue(*m_triggerCharacter));
			obj.insert("triggerCharacter", triggerCharacter);
		}
		if(m_triggerKind) {
			QJsonValue triggerKind = QJsonValue::fromVariant(QVariant::fromValue(*m_triggerKind));
			obj.insert("triggerKind", triggerKind);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("triggerCharacter")) {
			auto triggerCharacter = new QString(json.value("triggerCharacter").toString());
			m_triggerCharacter = triggerCharacter;
		}
		if(json.contains("triggerKind")) {
			auto triggerKind = new double(json.value("triggerKind").toVariant().value<double>());
			m_triggerKind = triggerKind;
		}

	}
};
const QString IniLanguage = "ini";
class SemanticTokensDeltaPartialResult: virtual public GoPlsParams {
	QList<SemanticTokensEdit*> *m_edits = nullptr;

public:
	SemanticTokensDeltaPartialResult() = default;
	virtual ~SemanticTokensDeltaPartialResult() {
		if(m_edits) { qDeleteAll(*m_edits); delete m_edits; }
	}

	void setEdits(QList<SemanticTokensEdit*> *edits) { m_edits = edits; }
	QList<SemanticTokensEdit*> *getEdits() { return m_edits; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_edits) {
				QJsonArray edits;
			for(auto it = m_edits->cbegin(); it != m_edits->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				edits.append(item);
			}

			obj.insert("edits", edits);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("edits")) {
			 QJsonArray arr = json.value("edits").toArray();
			auto edits = new QList<SemanticTokensEdit*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new SemanticTokensEdit();
			item->fromJson((m_item).toObject());
				edits->append(item);
			}
			m_edits = edits;
		}

	}
};
const QString MonikerKindImport = "import";
const int CompletionItemKindVariable = 6;
const QString MethodTextDocumentDeclaration = "textDocument/declaration";
const QString FsharpLanguage = "fsharp";
class ConfigurationParams: virtual public GoPlsParams {
	QList<ConfigurationItem*> *m_items = nullptr;

public:
	ConfigurationParams() = default;
	virtual ~ConfigurationParams() {
		if(m_items) { qDeleteAll(*m_items); delete m_items; }
	}

	void setItems(QList<ConfigurationItem*> *items) { m_items = items; }
	QList<ConfigurationItem*> *getItems() { return m_items; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_items) {
				QJsonArray items;
			for(auto it = m_items->cbegin(); it != m_items->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				items.append(item);
			}

			obj.insert("items", items);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("items")) {
			 QJsonArray arr = json.value("items").toArray();
			auto items = new QList<ConfigurationItem*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new ConfigurationItem();
			item->fromJson((m_item).toObject());
				items->append(item);
			}
			m_items = items;
		}

	}
};
const int CompletionItemKindInterface = 8;
class DocumentLink: virtual public GoPlsParams {
	Range *m_range = nullptr;
	DocumentURI *m_target = nullptr;
	QString *m_tooltip = nullptr;
	QJsonValue *m_data = nullptr;

public:
	DocumentLink() = default;
	virtual ~DocumentLink() {
		if(m_range) { delete m_range; }
		if(m_target) { delete m_target; }
		if(m_tooltip) { delete m_tooltip; }
		if(m_data) { delete m_data; }
	}

	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setTarget(DocumentURI *target) { m_target = target; }
	DocumentURI *getTarget() { return m_target; }
	void setTooltip(QString *tooltip) { m_tooltip = tooltip; }
	QString *getTooltip() { return m_tooltip; }
	void setData(QJsonValue *data) { m_data = data; }
	QJsonValue *getData() { return m_data; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_target) {
			QJsonValue target = QJsonValue::fromVariant(QVariant::fromValue(*m_target));
			obj.insert("target", target);
		}
		if(m_tooltip) {
			QJsonValue tooltip = QJsonValue::fromVariant(QVariant::fromValue(*m_tooltip));
			obj.insert("tooltip", tooltip);
		}
		if(m_data) {
			QJsonValue data = *m_data;
			obj.insert("data", data);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("target")) {
			auto target = new QString(json.value("target").toString());
			m_target = target;
		}
		if(json.contains("tooltip")) {
			auto tooltip = new QString(json.value("tooltip").toString());
			m_tooltip = tooltip;
		}
		if(json.contains("data")) {
			auto data = new QJsonValue(json.value("data"));
			m_data = data;
		}

	}
};
typedef CodeActionClientCapabilitiesLiteralSupport TextDocumentClientCapabilitiesCodeActionLiteralSupport;
typedef QHash<ChangeAnnotationIdentifier,ChangeAnnotation*> ChangeAnnotationsMap;
const QString JavaScriptLanguage = "javascript";
class DidChangeConfigurationParams: virtual public GoPlsParams {
	QJsonValue *m_settings = nullptr;

public:
	DidChangeConfigurationParams() = default;
	virtual ~DidChangeConfigurationParams() {
		if(m_settings) { delete m_settings; }
	}

	void setSettings(QJsonValue *settings) { m_settings = settings; }
	QJsonValue *getSettings() { return m_settings; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_settings) {
			QJsonValue settings = *m_settings;
			obj.insert("settings", settings);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("settings")) {
			auto settings = new QJsonValue(json.value("settings"));
			m_settings = settings;
		}

	}
};
class SignatureHelpContext: virtual public GoPlsParams {
	SignatureHelpTriggerKind *m_triggerKind = nullptr;
	QString *m_triggerCharacter = nullptr;
	bool *m_isRetrigger = nullptr;
	SignatureHelp *m_activeSignatureHelp = nullptr;

public:
	SignatureHelpContext() = default;
	virtual ~SignatureHelpContext() {
		if(m_triggerKind) { delete m_triggerKind; }
		if(m_triggerCharacter) { delete m_triggerCharacter; }
		if(m_isRetrigger) { delete m_isRetrigger; }
		if(m_activeSignatureHelp) { delete m_activeSignatureHelp; }
	}

	void setTriggerKind(SignatureHelpTriggerKind *triggerKind) { m_triggerKind = triggerKind; }
	SignatureHelpTriggerKind *getTriggerKind() { return m_triggerKind; }
	void setTriggerCharacter(QString *triggerCharacter) { m_triggerCharacter = triggerCharacter; }
	QString *getTriggerCharacter() { return m_triggerCharacter; }
	void setIsRetrigger(bool *isRetrigger) { m_isRetrigger = isRetrigger; }
	bool *getIsRetrigger() { return m_isRetrigger; }
	void setActiveSignatureHelp(SignatureHelp *activeSignatureHelp) { m_activeSignatureHelp = activeSignatureHelp; }
	SignatureHelp *getActiveSignatureHelp() { return m_activeSignatureHelp; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_triggerKind) {
			QJsonValue triggerKind = QJsonValue::fromVariant(QVariant::fromValue(*m_triggerKind));
			obj.insert("triggerKind", triggerKind);
		}
		if(m_triggerCharacter) {
			QJsonValue triggerCharacter = QJsonValue::fromVariant(QVariant::fromValue(*m_triggerCharacter));
			obj.insert("triggerCharacter", triggerCharacter);
		}
		if(m_isRetrigger) {
			QJsonValue isRetrigger = QJsonValue::fromVariant(QVariant::fromValue(*m_isRetrigger));
			obj.insert("isRetrigger", isRetrigger);
		}
		if(m_activeSignatureHelp) {
			QJsonObject activeSignatureHelp = m_activeSignatureHelp->toJson();
			obj.insert("activeSignatureHelp", activeSignatureHelp);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("triggerKind")) {
			auto triggerKind = new double(json.value("triggerKind").toVariant().value<double>());
			m_triggerKind = triggerKind;
		}
		if(json.contains("triggerCharacter")) {
			auto triggerCharacter = new QString(json.value("triggerCharacter").toString());
			m_triggerCharacter = triggerCharacter;
		}
		if(json.contains("isRetrigger")) {
			auto isRetrigger = new bool(json.value("isRetrigger").toVariant().value<bool>());
			m_isRetrigger = isRetrigger;
		}
		if(json.contains("activeSignatureHelp")) {
			auto activeSignatureHelp = new SignatureHelp();
			activeSignatureHelp->fromJson(json.value("activeSignatureHelp").toObject());
			m_activeSignatureHelp = activeSignatureHelp;
		}

	}
};
class SignatureHelpParams:  public TextDocumentPositionParams, public WorkDoneProgressParams {
	SignatureHelpContext *m_context = nullptr;

public:
	SignatureHelpParams() = default;
	virtual ~SignatureHelpParams() {
		if(m_context) { delete m_context; }
	}

	void setContext(SignatureHelpContext *context) { m_context = context; }
	SignatureHelpContext *getContext() { return m_context; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		if(m_context) {
			QJsonObject context = m_context->toJson();
			obj.insert("context", context);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);
		if(json.contains("context")) {
			auto context = new SignatureHelpContext();
			context->fromJson(json.value("context").toObject());
			m_context = context;
		}

	}
};
class CodeActionDisable: virtual public GoPlsParams {
	QString *m_reason = nullptr;

public:
	CodeActionDisable() = default;
	virtual ~CodeActionDisable() {
		if(m_reason) { delete m_reason; }
	}

	void setReason(QString *reason) { m_reason = reason; }
	QString *getReason() { return m_reason; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_reason) {
			QJsonValue reason = QJsonValue::fromVariant(QVariant::fromValue(*m_reason));
			obj.insert("reason", reason);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("reason")) {
			auto reason = new QString(json.value("reason").toString());
			m_reason = reason;
		}

	}
};
const QString MethodWorkDoneProgressCreate = "window/workDoneProgress/create";
typedef CodeLensClientCapabilities TextDocumentClientCapabilitiesCodeLens;
typedef QList<FileDelete*> FileDeletes;
class InsertReplaceEdit: virtual public GoPlsParams {
	QString *m_newText = nullptr;
	Range *m_insert = nullptr;
	Range *m_replace = nullptr;

public:
	InsertReplaceEdit() = default;
	virtual ~InsertReplaceEdit() {
		if(m_newText) { delete m_newText; }
		if(m_insert) { delete m_insert; }
		if(m_replace) { delete m_replace; }
	}

	void setNewText(QString *newText) { m_newText = newText; }
	QString *getNewText() { return m_newText; }
	void setInsert(Range *insert) { m_insert = insert; }
	Range *getInsert() { return m_insert; }
	void setReplace(Range *replace) { m_replace = replace; }
	Range *getReplace() { return m_replace; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_newText) {
			QJsonValue newText = QJsonValue::fromVariant(QVariant::fromValue(*m_newText));
			obj.insert("newText", newText);
		}
		if(m_insert) {
			QJsonObject insert = m_insert->toJson();
			obj.insert("insert", insert);
		}
		if(m_replace) {
			QJsonObject replace = m_replace->toJson();
			obj.insert("replace", replace);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("newText")) {
			auto newText = new QString(json.value("newText").toString());
			m_newText = newText;
		}
		if(json.contains("insert")) {
			auto insert = new Range();
			insert->fromJson(json.value("insert").toObject());
			m_insert = insert;
		}
		if(json.contains("replace")) {
			auto replace = new Range();
			replace->fromJson(json.value("replace").toObject());
			m_replace = replace;
		}

	}
};
const QString MethodTextDocumentDidClose = "textDocument/didClose";
const QString CLanguage = "c";
const QString SemanticTokenOperator = "operator";
const QString MethodMoniker = "textDocument/moniker";
const QString ScalaLanguage = "scala";
class SemanticTokensRangeParams:  public WorkDoneProgressParams, public PartialResultParams {
	TextDocumentIdentifier *m_textDocument = nullptr;
	Range *m_range = nullptr;

public:
	SemanticTokensRangeParams() = default;
	virtual ~SemanticTokensRangeParams() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_range) { delete m_range; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}

	}
};
class WorkDoneProgressBegin: virtual public GoPlsParams {
	WorkDoneProgressKind *m_kind = nullptr;
	QString *m_title = nullptr;
	bool *m_cancellable = nullptr;
	QString *m_message = nullptr;
	unsigned int *m_percentage = nullptr;

public:
	WorkDoneProgressBegin() = default;
	virtual ~WorkDoneProgressBegin() {
		if(m_kind) { delete m_kind; }
		if(m_title) { delete m_title; }
		if(m_cancellable) { delete m_cancellable; }
		if(m_message) { delete m_message; }
		if(m_percentage) { delete m_percentage; }
	}

	void setKind(WorkDoneProgressKind *kind) { m_kind = kind; }
	WorkDoneProgressKind *getKind() { return m_kind; }
	void setTitle(QString *title) { m_title = title; }
	QString *getTitle() { return m_title; }
	void setCancellable(bool *cancellable) { m_cancellable = cancellable; }
	bool *getCancellable() { return m_cancellable; }
	void setMessage(QString *message) { m_message = message; }
	QString *getMessage() { return m_message; }
	void setPercentage(unsigned int *percentage) { m_percentage = percentage; }
	unsigned int *getPercentage() { return m_percentage; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_title) {
			QJsonValue title = QJsonValue::fromVariant(QVariant::fromValue(*m_title));
			obj.insert("title", title);
		}
		if(m_cancellable) {
			QJsonValue cancellable = QJsonValue::fromVariant(QVariant::fromValue(*m_cancellable));
			obj.insert("cancellable", cancellable);
		}
		if(m_message) {
			QJsonValue message = QJsonValue::fromVariant(QVariant::fromValue(*m_message));
			obj.insert("message", message);
		}
		if(m_percentage) {
			QJsonValue percentage = QJsonValue::fromVariant(QVariant::fromValue(*m_percentage));
			obj.insert("percentage", percentage);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("kind")) {
			auto kind = new QString(json.value("kind").toString());
			m_kind = kind;
		}
		if(json.contains("title")) {
			auto title = new QString(json.value("title").toString());
			m_title = title;
		}
		if(json.contains("cancellable")) {
			auto cancellable = new bool(json.value("cancellable").toVariant().value<bool>());
			m_cancellable = cancellable;
		}
		if(json.contains("message")) {
			auto message = new QString(json.value("message").toString());
			m_message = message;
		}
		if(json.contains("percentage")) {
			auto percentage = new unsigned int(json.value("percentage").toVariant().value<unsigned int>());
			m_percentage = percentage;
		}

	}
};
const QString MethodDidRenameFiles = "workspace/didRenameFiles";
const QString MethodWillDeleteFiles = "workspace/willDeleteFiles";
class DocumentOnTypeFormattingRegistrationOptions:  public TextDocumentRegistrationOptions {
	QString *m_firstTriggerCharacter = nullptr;
	QList<QString*> *m_moreTriggerCharacter = nullptr;

public:
	DocumentOnTypeFormattingRegistrationOptions() = default;
	virtual ~DocumentOnTypeFormattingRegistrationOptions() {
		if(m_firstTriggerCharacter) { delete m_firstTriggerCharacter; }
		if(m_moreTriggerCharacter) { qDeleteAll(*m_moreTriggerCharacter); delete m_moreTriggerCharacter; }
	}

	void setFirstTriggerCharacter(QString *firstTriggerCharacter) { m_firstTriggerCharacter = firstTriggerCharacter; }
	QString *getFirstTriggerCharacter() { return m_firstTriggerCharacter; }
	void setMoreTriggerCharacter(QList<QString*> *moreTriggerCharacter) { m_moreTriggerCharacter = moreTriggerCharacter; }
	QList<QString*> *getMoreTriggerCharacter() { return m_moreTriggerCharacter; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		if(m_firstTriggerCharacter) {
			QJsonValue firstTriggerCharacter = QJsonValue::fromVariant(QVariant::fromValue(*m_firstTriggerCharacter));
			obj.insert("firstTriggerCharacter", firstTriggerCharacter);
		}
		if(m_moreTriggerCharacter) {
				QJsonArray moreTriggerCharacter;
			for(auto it = m_moreTriggerCharacter->cbegin(); it != m_moreTriggerCharacter->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				moreTriggerCharacter.append(item);
			}

			obj.insert("moreTriggerCharacter", moreTriggerCharacter);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		if(json.contains("firstTriggerCharacter")) {
			auto firstTriggerCharacter = new QString(json.value("firstTriggerCharacter").toString());
			m_firstTriggerCharacter = firstTriggerCharacter;
		}
		if(json.contains("moreTriggerCharacter")) {
			 QJsonArray arr = json.value("moreTriggerCharacter").toArray();
			auto moreTriggerCharacter = new QList<QString*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				moreTriggerCharacter->append(item);
			}
			m_moreTriggerCharacter = moreTriggerCharacter;
		}

	}
};
const QString CSSLanguage = "css";
const QString GroovyLanguage = "groovy";
typedef QList<unsigned int*> Uint32s;
class CallHierarchyRegistrationOptions:  public TextDocumentRegistrationOptions, public CallHierarchyOptions, public StaticRegistrationOptions {

public:
	CallHierarchyRegistrationOptions() = default;
	virtual ~CallHierarchyRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject CallHierarchyOptions_json = CallHierarchyOptions::toJson();
		for(const QString &key: CallHierarchyOptions_json.keys()) {
			obj.insert(key, CallHierarchyOptions_json.value(key));
		}
		QJsonObject StaticRegistrationOptions_json = StaticRegistrationOptions::toJson();
		for(const QString &key: StaticRegistrationOptions_json.keys()) {
			obj.insert(key, StaticRegistrationOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		CallHierarchyOptions::fromJson(json);
		StaticRegistrationOptions::fromJson(json);

	}
};
const int SymbolKindPackage = 4;
const QString SemanticTokenNumber = "number";
class DefinitionOptions:  public WorkDoneProgressOptions {

public:
	DefinitionOptions() = default;
	virtual ~DefinitionOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
const QString DeleteResourceOperation = "delete";
class CodeAction: virtual public GoPlsParams {
	QString *m_title = nullptr;
	CodeActionKind *m_kind = nullptr;
	QList<Diagnostic*> *m_diagnostics = nullptr;
	bool *m_isPreferred = nullptr;
	CodeActionDisable *m_disabled = nullptr;
	WorkspaceEdit *m_edit = nullptr;
	Command *m_command = nullptr;
	QJsonValue *m_data = nullptr;

public:
	CodeAction() = default;
	virtual ~CodeAction() {
		if(m_title) { delete m_title; }
		if(m_kind) { delete m_kind; }
		if(m_diagnostics) { qDeleteAll(*m_diagnostics); delete m_diagnostics; }
		if(m_isPreferred) { delete m_isPreferred; }
		if(m_disabled) { delete m_disabled; }
		if(m_edit) { delete m_edit; }
		if(m_command) { delete m_command; }
		if(m_data) { delete m_data; }
	}

	void setTitle(QString *title) { m_title = title; }
	QString *getTitle() { return m_title; }
	void setKind(CodeActionKind *kind) { m_kind = kind; }
	CodeActionKind *getKind() { return m_kind; }
	void setDiagnostics(QList<Diagnostic*> *diagnostics) { m_diagnostics = diagnostics; }
	QList<Diagnostic*> *getDiagnostics() { return m_diagnostics; }
	void setIsPreferred(bool *isPreferred) { m_isPreferred = isPreferred; }
	bool *getIsPreferred() { return m_isPreferred; }
	void setDisabled(CodeActionDisable *disabled) { m_disabled = disabled; }
	CodeActionDisable *getDisabled() { return m_disabled; }
	void setEdit(WorkspaceEdit *edit) { m_edit = edit; }
	WorkspaceEdit *getEdit() { return m_edit; }
	void setCommand(Command *command) { m_command = command; }
	Command *getCommand() { return m_command; }
	void setData(QJsonValue *data) { m_data = data; }
	QJsonValue *getData() { return m_data; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_title) {
			QJsonValue title = QJsonValue::fromVariant(QVariant::fromValue(*m_title));
			obj.insert("title", title);
		}
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_diagnostics) {
				QJsonArray diagnostics;
			for(auto it = m_diagnostics->cbegin(); it != m_diagnostics->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				diagnostics.append(item);
			}

			obj.insert("diagnostics", diagnostics);
		}
		if(m_isPreferred) {
			QJsonValue isPreferred = QJsonValue::fromVariant(QVariant::fromValue(*m_isPreferred));
			obj.insert("isPreferred", isPreferred);
		}
		if(m_disabled) {
			QJsonObject disabled = m_disabled->toJson();
			obj.insert("disabled", disabled);
		}
		if(m_edit) {
			QJsonObject edit = m_edit->toJson();
			obj.insert("edit", edit);
		}
		if(m_command) {
			QJsonObject command = m_command->toJson();
			obj.insert("command", command);
		}
		if(m_data) {
			QJsonValue data = *m_data;
			obj.insert("data", data);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("title")) {
			auto title = new QString(json.value("title").toString());
			m_title = title;
		}
		if(json.contains("kind")) {
			auto kind = new QString(json.value("kind").toString());
			m_kind = kind;
		}
		if(json.contains("diagnostics")) {
			 QJsonArray arr = json.value("diagnostics").toArray();
			auto diagnostics = new QList<Diagnostic*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new Diagnostic();
			item->fromJson((m_item).toObject());
				diagnostics->append(item);
			}
			m_diagnostics = diagnostics;
		}
		if(json.contains("isPreferred")) {
			auto isPreferred = new bool(json.value("isPreferred").toVariant().value<bool>());
			m_isPreferred = isPreferred;
		}
		if(json.contains("disabled")) {
			auto disabled = new CodeActionDisable();
			disabled->fromJson(json.value("disabled").toObject());
			m_disabled = disabled;
		}
		if(json.contains("edit")) {
			auto edit = new WorkspaceEdit();
			edit->fromJson(json.value("edit").toObject());
			m_edit = edit;
		}
		if(json.contains("command")) {
			auto command = new Command();
			command->fromJson(json.value("command").toObject());
			m_command = command;
		}
		if(json.contains("data")) {
			auto data = new QJsonValue(json.value("data"));
			m_data = data;
		}

	}
};
const int FileChangeTypeCreated = 1;
typedef QList<SymbolKind*> SymbolKinds;
const QString SQLLanguage = "sql";
class RenameFilesParams: virtual public GoPlsParams {
	QList<FileRename*> *m_files = nullptr;

public:
	RenameFilesParams() = default;
	virtual ~RenameFilesParams() {
		if(m_files) { qDeleteAll(*m_files); delete m_files; }
	}

	void setFiles(QList<FileRename*> *files) { m_files = files; }
	QList<FileRename*> *getFiles() { return m_files; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_files) {
				QJsonArray files;
			for(auto it = m_files->cbegin(); it != m_files->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				files.append(item);
			}

			obj.insert("files", files);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("files")) {
			 QJsonArray arr = json.value("files").toArray();
			auto files = new QList<FileRename*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new FileRename();
			item->fromJson((m_item).toObject());
				files->append(item);
			}
			m_files = files;
		}

	}
};
const QString MethodWorkspaceConfiguration = "workspace/configuration";
const QString MethodTextDocumentCodeAction = "textDocument/codeAction";
class RenameRegistrationOptions:  public TextDocumentRegistrationOptions {
	bool *m_prepareProvider = nullptr;

public:
	RenameRegistrationOptions() = default;
	virtual ~RenameRegistrationOptions() {
		if(m_prepareProvider) { delete m_prepareProvider; }
	}

	void setPrepareProvider(bool *prepareProvider) { m_prepareProvider = prepareProvider; }
	bool *getPrepareProvider() { return m_prepareProvider; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		if(m_prepareProvider) {
			QJsonValue prepareProvider = QJsonValue::fromVariant(QVariant::fromValue(*m_prepareProvider));
			obj.insert("prepareProvider", prepareProvider);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		if(json.contains("prepareProvider")) {
			auto prepareProvider = new bool(json.value("prepareProvider").toVariant().value<bool>());
			m_prepareProvider = prepareProvider;
		}

	}
};
const QString RazorLanguage = "razor";
typedef QList<FileCreate*> FileCreates;
class DocumentRangeFormattingOptions:  public WorkDoneProgressOptions {

public:
	DocumentRangeFormattingOptions() = default;
	virtual ~DocumentRangeFormattingOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
class DocumentColorRegistrationOptions:  public TextDocumentRegistrationOptions, public StaticRegistrationOptions, public DocumentColorOptions {

public:
	DocumentColorRegistrationOptions() = default;
	virtual ~DocumentColorRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject StaticRegistrationOptions_json = StaticRegistrationOptions::toJson();
		for(const QString &key: StaticRegistrationOptions_json.keys()) {
			obj.insert(key, StaticRegistrationOptions_json.value(key));
		}
		QJsonObject DocumentColorOptions_json = DocumentColorOptions::toJson();
		for(const QString &key: DocumentColorOptions_json.keys()) {
			obj.insert(key, DocumentColorOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		StaticRegistrationOptions::fromJson(json);
		DocumentColorOptions::fromJson(json);

	}
};
typedef DidChangeWatchedFilesWorkspaceClientCapabilities WorkspaceClientCapabilitiesDidChangeWatchedFiles;
const QString MethodSemanticTokensRefresh = "workspace/semanticTokens/refresh";
class SemanticTokensPartialResult: virtual public GoPlsParams {
	QList<unsigned int*> *m_data = nullptr;

public:
	SemanticTokensPartialResult() = default;
	virtual ~SemanticTokensPartialResult() {
		if(m_data) { qDeleteAll(*m_data); delete m_data; }
	}

	void setData(QList<unsigned int*> *data) { m_data = data; }
	QList<unsigned int*> *getData() { return m_data; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_data) {
				QJsonArray data;
			for(auto it = m_data->cbegin(); it != m_data->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				data.append(item);
			}

			obj.insert("data", data);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("data")) {
			 QJsonArray arr = json.value("data").toArray();
			auto data = new QList<unsigned int*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new unsigned int((m_item).toVariant().value<unsigned int>());
				data->append(item);
			}
			m_data = data;
		}

	}
};
class WorkspaceSymbolOptions:  public WorkDoneProgressOptions {

public:
	WorkspaceSymbolOptions() = default;
	virtual ~WorkspaceSymbolOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
typedef DocumentSymbolClientCapabilitiesTagSupport TextDocumentClientCapabilitiesDocumentSymbolTagSupport;
typedef QList<FileEvent*> FileEvents;
const QString JavaLanguage = "java";
class ColorInformation: virtual public GoPlsParams {
	Range *m_range = nullptr;
	Color *m_color = nullptr;

public:
	ColorInformation() = default;
	virtual ~ColorInformation() {
		if(m_range) { delete m_range; }
		if(m_color) { delete m_color; }
	}

	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setColor(Color *color) { m_color = color; }
	Color *getColor() { return m_color; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_color) {
			QJsonObject color = m_color->toJson();
			obj.insert("color", color);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("color")) {
			auto color = new Color();
			color->fromJson(json.value("color").toObject());
			m_color = color;
		}

	}
};
const QString MethodTelemetryEvent = "telemetry/event";
typedef DeclarationTextDocumentClientCapabilities TextDocumentClientCapabilitiesDeclaration;
const QString SemanticTokenEvent = "event";
class Moniker: virtual public GoPlsParams {
	QString *m_scheme = nullptr;
	QString *m_identifier = nullptr;
	UniquenessLevel *m_unique = nullptr;
	MonikerKind *m_kind = nullptr;

public:
	Moniker() = default;
	virtual ~Moniker() {
		if(m_scheme) { delete m_scheme; }
		if(m_identifier) { delete m_identifier; }
		if(m_unique) { delete m_unique; }
		if(m_kind) { delete m_kind; }
	}

	void setScheme(QString *scheme) { m_scheme = scheme; }
	QString *getScheme() { return m_scheme; }
	void setIdentifier(QString *identifier) { m_identifier = identifier; }
	QString *getIdentifier() { return m_identifier; }
	void setUnique(UniquenessLevel *unique) { m_unique = unique; }
	UniquenessLevel *getUnique() { return m_unique; }
	void setKind(MonikerKind *kind) { m_kind = kind; }
	MonikerKind *getKind() { return m_kind; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_scheme) {
			QJsonValue scheme = QJsonValue::fromVariant(QVariant::fromValue(*m_scheme));
			obj.insert("scheme", scheme);
		}
		if(m_identifier) {
			QJsonValue identifier = QJsonValue::fromVariant(QVariant::fromValue(*m_identifier));
			obj.insert("identifier", identifier);
		}
		if(m_unique) {
			QJsonValue unique = QJsonValue::fromVariant(QVariant::fromValue(*m_unique));
			obj.insert("unique", unique);
		}
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("scheme")) {
			auto scheme = new QString(json.value("scheme").toString());
			m_scheme = scheme;
		}
		if(json.contains("identifier")) {
			auto identifier = new QString(json.value("identifier").toString());
			m_identifier = identifier;
		}
		if(json.contains("unique")) {
			auto unique = new QString(json.value("unique").toString());
			m_unique = unique;
		}
		if(json.contains("kind")) {
			auto kind = new QString(json.value("kind").toString());
			m_kind = kind;
		}

	}
};
const QString QuickFix = "quickfix";
const QString SemanticTokenEnum = "enum";
class DocumentSymbol: virtual public GoPlsParams {
	QString *m_name = nullptr;
	QString *m_detail = nullptr;
	SymbolKind *m_kind = nullptr;
	QList<SymbolTag*> *m_tags = nullptr;
	bool *m_deprecated = nullptr;
	Range *m_range = nullptr;
	Range *m_selectionRange = nullptr;
	QList<DocumentSymbol*> *m_children = nullptr;

public:
	DocumentSymbol() = default;
	virtual ~DocumentSymbol() {
		if(m_name) { delete m_name; }
		if(m_detail) { delete m_detail; }
		if(m_kind) { delete m_kind; }
		if(m_tags) { qDeleteAll(*m_tags); delete m_tags; }
		if(m_deprecated) { delete m_deprecated; }
		if(m_range) { delete m_range; }
		if(m_selectionRange) { delete m_selectionRange; }
		if(m_children) { qDeleteAll(*m_children); delete m_children; }
	}

	void setName(QString *name) { m_name = name; }
	QString *getName() { return m_name; }
	void setDetail(QString *detail) { m_detail = detail; }
	QString *getDetail() { return m_detail; }
	void setKind(SymbolKind *kind) { m_kind = kind; }
	SymbolKind *getKind() { return m_kind; }
	void setTags(QList<SymbolTag*> *tags) { m_tags = tags; }
	QList<SymbolTag*> *getTags() { return m_tags; }
	void setDeprecated(bool *deprecated) { m_deprecated = deprecated; }
	bool *getDeprecated() { return m_deprecated; }
	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setSelectionRange(Range *selectionRange) { m_selectionRange = selectionRange; }
	Range *getSelectionRange() { return m_selectionRange; }
	void setChildren(QList<DocumentSymbol*> *children) { m_children = children; }
	QList<DocumentSymbol*> *getChildren() { return m_children; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_name) {
			QJsonValue name = QJsonValue::fromVariant(QVariant::fromValue(*m_name));
			obj.insert("name", name);
		}
		if(m_detail) {
			QJsonValue detail = QJsonValue::fromVariant(QVariant::fromValue(*m_detail));
			obj.insert("detail", detail);
		}
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_tags) {
				QJsonArray tags;
			for(auto it = m_tags->cbegin(); it != m_tags->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				tags.append(item);
			}

			obj.insert("tags", tags);
		}
		if(m_deprecated) {
			QJsonValue deprecated = QJsonValue::fromVariant(QVariant::fromValue(*m_deprecated));
			obj.insert("deprecated", deprecated);
		}
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_selectionRange) {
			QJsonObject selectionRange = m_selectionRange->toJson();
			obj.insert("selectionRange", selectionRange);
		}
		if(m_children) {
				QJsonArray children;
			for(auto it = m_children->cbegin(); it != m_children->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				children.append(item);
			}

			obj.insert("children", children);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("name")) {
			auto name = new QString(json.value("name").toString());
			m_name = name;
		}
		if(json.contains("detail")) {
			auto detail = new QString(json.value("detail").toString());
			m_detail = detail;
		}
		if(json.contains("kind")) {
			auto kind = new double(json.value("kind").toVariant().value<double>());
			m_kind = kind;
		}
		if(json.contains("tags")) {
			 QJsonArray arr = json.value("tags").toArray();
			auto tags = new QList<SymbolTag*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				tags->append(item);
			}
			m_tags = tags;
		}
		if(json.contains("deprecated")) {
			auto deprecated = new bool(json.value("deprecated").toVariant().value<bool>());
			m_deprecated = deprecated;
		}
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("selectionRange")) {
			auto selectionRange = new Range();
			selectionRange->fromJson(json.value("selectionRange").toObject());
			m_selectionRange = selectionRange;
		}
		if(json.contains("children")) {
			 QJsonArray arr = json.value("children").toArray();
			auto children = new QList<DocumentSymbol*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new DocumentSymbol();
			item->fromJson((m_item).toObject());
				children->append(item);
			}
			m_children = children;
		}

	}
};
typedef QList<DocumentSymbol*> DocumentSymbols;
const QString SemanticTokenNamespace = "namespace";
typedef DocumentRangeFormattingClientCapabilities TextDocumentClientCapabilitiesRangeFormatting;
class SemanticTokensLegend: virtual public GoPlsParams {
	QList<SemanticTokenTypes*> *m_tokenTypes = nullptr;
	QList<SemanticTokenModifiers*> *m_tokenModifiers = nullptr;

public:
	SemanticTokensLegend() = default;
	virtual ~SemanticTokensLegend() {
		if(m_tokenTypes) { qDeleteAll(*m_tokenTypes); delete m_tokenTypes; }
		if(m_tokenModifiers) { qDeleteAll(*m_tokenModifiers); delete m_tokenModifiers; }
	}

	void setTokenTypes(QList<SemanticTokenTypes*> *tokenTypes) { m_tokenTypes = tokenTypes; }
	QList<SemanticTokenTypes*> *getTokenTypes() { return m_tokenTypes; }
	void setTokenModifiers(QList<SemanticTokenModifiers*> *tokenModifiers) { m_tokenModifiers = tokenModifiers; }
	QList<SemanticTokenModifiers*> *getTokenModifiers() { return m_tokenModifiers; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_tokenTypes) {
				QJsonArray tokenTypes;
			for(auto it = m_tokenTypes->cbegin(); it != m_tokenTypes->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				tokenTypes.append(item);
			}

			obj.insert("tokenTypes", tokenTypes);
		}
		if(m_tokenModifiers) {
				QJsonArray tokenModifiers;
			for(auto it = m_tokenModifiers->cbegin(); it != m_tokenModifiers->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				tokenModifiers.append(item);
			}

			obj.insert("tokenModifiers", tokenModifiers);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("tokenTypes")) {
			 QJsonArray arr = json.value("tokenTypes").toArray();
			auto tokenTypes = new QList<SemanticTokenTypes*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				tokenTypes->append(item);
			}
			m_tokenTypes = tokenTypes;
		}
		if(json.contains("tokenModifiers")) {
			 QJsonArray arr = json.value("tokenModifiers").toArray();
			auto tokenModifiers = new QList<SemanticTokenModifiers*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new QString((m_item).toString());
				tokenModifiers->append(item);
			}
			m_tokenModifiers = tokenModifiers;
		}

	}
};
class RenameParams:  public TextDocumentPositionParams, public PartialResultParams {
	QString *m_newName = nullptr;

public:
	RenameParams() = default;
	virtual ~RenameParams() {
		if(m_newName) { delete m_newName; }
	}

	void setNewName(QString *newName) { m_newName = newName; }
	QString *getNewName() { return m_newName; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_newName) {
			QJsonValue newName = QJsonValue::fromVariant(QVariant::fromValue(*m_newName));
			obj.insert("newName", newName);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("newName")) {
			auto newName = new QString(json.value("newName").toString());
			m_newName = newName;
		}

	}
};
const int CompletionTriggerKindInvoked = 1;
const QString MethodTextDocumentHover = "textDocument/hover";
const QString RubyLanguage = "ruby";
const QString SemanticTokenModifierDefinition = "definition";
class CallHierarchyOutgoingCallsParams:  public WorkDoneProgressParams, public PartialResultParams {
	CallHierarchyItem *m_item = nullptr;

public:
	CallHierarchyOutgoingCallsParams() = default;
	virtual ~CallHierarchyOutgoingCallsParams() {
		if(m_item) { delete m_item; }
	}

	void setItem(CallHierarchyItem *item) { m_item = item; }
	CallHierarchyItem *getItem() { return m_item; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_item) {
			QJsonObject item = m_item->toJson();
			obj.insert("item", item);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("item")) {
			auto item = new CallHierarchyItem();
			item->fromJson(json.value("item").toObject());
			m_item = item;
		}

	}
};
class InitializeError: virtual public GoPlsParams {
	bool *m_retry = nullptr;

public:
	InitializeError() = default;
	virtual ~InitializeError() {
		if(m_retry) { delete m_retry; }
	}

	void setRetry(bool *retry) { m_retry = retry; }
	bool *getRetry() { return m_retry; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_retry) {
			QJsonValue retry = QJsonValue::fromVariant(QVariant::fromValue(*m_retry));
			obj.insert("retry", retry);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("retry")) {
			auto retry = new bool(json.value("retry").toVariant().value<bool>());
			m_retry = retry;
		}

	}
};
class CompletionParams:  public TextDocumentPositionParams, public WorkDoneProgressParams, public PartialResultParams {
	CompletionContext *m_context = nullptr;

public:
	CompletionParams() = default;
	virtual ~CompletionParams() {
		if(m_context) { delete m_context; }
	}

	void setContext(CompletionContext *context) { m_context = context; }
	CompletionContext *getContext() { return m_context; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentPositionParams_json = TextDocumentPositionParams::toJson();
		for(const QString &key: TextDocumentPositionParams_json.keys()) {
			obj.insert(key, TextDocumentPositionParams_json.value(key));
		}
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_context) {
			QJsonObject context = m_context->toJson();
			obj.insert("context", context);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentPositionParams::fromJson(json);
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("context")) {
			auto context = new CompletionContext();
			context->fromJson(json.value("context").toObject());
			m_context = context;
		}

	}
};
class ShowDocumentResult: virtual public GoPlsParams {
	bool *m_success = nullptr;

public:
	ShowDocumentResult() = default;
	virtual ~ShowDocumentResult() {
		if(m_success) { delete m_success; }
	}

	void setSuccess(bool *success) { m_success = success; }
	bool *getSuccess() { return m_success; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_success) {
			QJsonValue success = QJsonValue::fromVariant(QVariant::fromValue(*m_success));
			obj.insert("success", success);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("success")) {
			auto success = new bool(json.value("success").toVariant().value<bool>());
			m_success = success;
		}

	}
};
class DocumentLinkParams:  public WorkDoneProgressParams, public PartialResultParams {
	TextDocumentIdentifier *m_textDocument = nullptr;

public:
	DocumentLinkParams() = default;
	virtual ~DocumentLinkParams() {
		if(m_textDocument) { delete m_textDocument; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		QJsonObject PartialResultParams_json = PartialResultParams::toJson();
		for(const QString &key: PartialResultParams_json.keys()) {
			obj.insert(key, PartialResultParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		PartialResultParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}

	}
};
const QString RefactorExtract = "refactor.extract";
const QString MethodCancelRequest = "$/cancelRequest";
class LogTraceParams: virtual public GoPlsParams {
	QString *m_message = nullptr;
	TraceValue *m_verbose = nullptr;

public:
	LogTraceParams() = default;
	virtual ~LogTraceParams() {
		if(m_message) { delete m_message; }
		if(m_verbose) { delete m_verbose; }
	}

	void setMessage(QString *message) { m_message = message; }
	QString *getMessage() { return m_message; }
	void setVerbose(TraceValue *verbose) { m_verbose = verbose; }
	TraceValue *getVerbose() { return m_verbose; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_message) {
			QJsonValue message = QJsonValue::fromVariant(QVariant::fromValue(*m_message));
			obj.insert("message", message);
		}
		if(m_verbose) {
			QJsonValue verbose = QJsonValue::fromVariant(QVariant::fromValue(*m_verbose));
			obj.insert("verbose", verbose);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("message")) {
			auto message = new QString(json.value("message").toString());
			m_message = message;
		}
		if(json.contains("verbose")) {
			auto verbose = new QString(json.value("verbose").toString());
			m_verbose = verbose;
		}

	}
};
const QString MethodTextDocumentTypeDefinition = "textDocument/typeDefinition";
const QString HTMLLanguage = "html";
const QString FailureHandlingKindUndo = "undo";
typedef QList<SignatureInformation*> SignatureInformations;
typedef bool EnableSelectionRange;
const int CompletionTriggerKindTriggerForIncompleteCompletions = 3;
typedef ShowMessageRequestClientCapabilitiesMessageActionItem ClientCapabilitiesShowMessageRequestMessageActionItem;
typedef ExecuteCommandClientCapabilities WorkspaceClientCapabilitiesExecuteCommand;
const QString JavaScriptReactLanguage = "javascriptreact";
class FoldingRangeRegistrationOptions:  public TextDocumentRegistrationOptions, public FoldingRangeOptions, public StaticRegistrationOptions {

public:
	FoldingRangeRegistrationOptions() = default;
	virtual ~FoldingRangeRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject FoldingRangeOptions_json = FoldingRangeOptions::toJson();
		for(const QString &key: FoldingRangeOptions_json.keys()) {
			obj.insert(key, FoldingRangeOptions_json.value(key));
		}
		QJsonObject StaticRegistrationOptions_json = StaticRegistrationOptions::toJson();
		for(const QString &key: StaticRegistrationOptions_json.keys()) {
			obj.insert(key, StaticRegistrationOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		FoldingRangeOptions::fromJson(json);
		StaticRegistrationOptions::fromJson(json);

	}
};
const int CompletionItemKindText = 1;
class LocationLink: virtual public GoPlsParams {
	Range *m_originSelectionRange = nullptr;
	DocumentURI *m_targetUri = nullptr;
	Range *m_targetRange = nullptr;
	Range *m_targetSelectionRange = nullptr;

public:
	LocationLink() = default;
	virtual ~LocationLink() {
		if(m_originSelectionRange) { delete m_originSelectionRange; }
		if(m_targetUri) { delete m_targetUri; }
		if(m_targetRange) { delete m_targetRange; }
		if(m_targetSelectionRange) { delete m_targetSelectionRange; }
	}

	void setOriginSelectionRange(Range *originSelectionRange) { m_originSelectionRange = originSelectionRange; }
	Range *getOriginSelectionRange() { return m_originSelectionRange; }
	void setTargetUri(DocumentURI *targetUri) { m_targetUri = targetUri; }
	DocumentURI *getTargetUri() { return m_targetUri; }
	void setTargetRange(Range *targetRange) { m_targetRange = targetRange; }
	Range *getTargetRange() { return m_targetRange; }
	void setTargetSelectionRange(Range *targetSelectionRange) { m_targetSelectionRange = targetSelectionRange; }
	Range *getTargetSelectionRange() { return m_targetSelectionRange; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_originSelectionRange) {
			QJsonObject originSelectionRange = m_originSelectionRange->toJson();
			obj.insert("originSelectionRange", originSelectionRange);
		}
		if(m_targetUri) {
			QJsonValue targetUri = QJsonValue::fromVariant(QVariant::fromValue(*m_targetUri));
			obj.insert("targetUri", targetUri);
		}
		if(m_targetRange) {
			QJsonObject targetRange = m_targetRange->toJson();
			obj.insert("targetRange", targetRange);
		}
		if(m_targetSelectionRange) {
			QJsonObject targetSelectionRange = m_targetSelectionRange->toJson();
			obj.insert("targetSelectionRange", targetSelectionRange);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("originSelectionRange")) {
			auto originSelectionRange = new Range();
			originSelectionRange->fromJson(json.value("originSelectionRange").toObject());
			m_originSelectionRange = originSelectionRange;
		}
		if(json.contains("targetUri")) {
			auto targetUri = new QString(json.value("targetUri").toString());
			m_targetUri = targetUri;
		}
		if(json.contains("targetRange")) {
			auto targetRange = new Range();
			targetRange->fromJson(json.value("targetRange").toObject());
			m_targetRange = targetRange;
		}
		if(json.contains("targetSelectionRange")) {
			auto targetSelectionRange = new Range();
			targetSelectionRange->fromJson(json.value("targetSelectionRange").toObject());
			m_targetSelectionRange = targetSelectionRange;
		}

	}
};
const QString FailureHandlingKindTextOnlyTransactional = "textOnlyTransactional";
class DocumentRangeFormattingParams:  public WorkDoneProgressParams {
	TextDocumentIdentifier *m_textDocument = nullptr;
	Range *m_range = nullptr;
	FormattingOptions *m_options = nullptr;

public:
	DocumentRangeFormattingParams() = default;
	virtual ~DocumentRangeFormattingParams() {
		if(m_textDocument) { delete m_textDocument; }
		if(m_range) { delete m_range; }
		if(m_options) { delete m_options; }
	}

	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	void setRange(Range *range) { m_range = range; }
	Range *getRange() { return m_range; }
	void setOptions(FormattingOptions *options) { m_options = options; }
	FormattingOptions *getOptions() { return m_options; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressParams_json = WorkDoneProgressParams::toJson();
		for(const QString &key: WorkDoneProgressParams_json.keys()) {
			obj.insert(key, WorkDoneProgressParams_json.value(key));
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}
		if(m_range) {
			QJsonObject range = m_range->toJson();
			obj.insert("range", range);
		}
		if(m_options) {
			QJsonObject options = m_options->toJson();
			obj.insert("options", options);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressParams::fromJson(json);
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}
		if(json.contains("range")) {
			auto range = new Range();
			range->fromJson(json.value("range").toObject());
			m_range = range;
		}
		if(json.contains("options")) {
			auto options = new FormattingOptions();
			options->fromJson(json.value("options").toObject());
			m_options = options;
		}

	}
};
const QString PythonLanguage = "python";
const QString CoffeeScriptLanguage = "coffeescript";
class DidSaveTextDocumentParams: virtual public GoPlsParams {
	QString *m_text = nullptr;
	TextDocumentIdentifier *m_textDocument = nullptr;

public:
	DidSaveTextDocumentParams() = default;
	virtual ~DidSaveTextDocumentParams() {
		if(m_text) { delete m_text; }
		if(m_textDocument) { delete m_textDocument; }
	}

	void setText(QString *text) { m_text = text; }
	QString *getText() { return m_text; }
	void setTextDocument(TextDocumentIdentifier *textDocument) { m_textDocument = textDocument; }
	TextDocumentIdentifier *getTextDocument() { return m_textDocument; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_text) {
			QJsonValue text = QJsonValue::fromVariant(QVariant::fromValue(*m_text));
			obj.insert("text", text);
		}
		if(m_textDocument) {
			QJsonObject textDocument = m_textDocument->toJson();
			obj.insert("textDocument", textDocument);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("text")) {
			auto text = new QString(json.value("text").toString());
			m_text = text;
		}
		if(json.contains("textDocument")) {
			auto textDocument = new TextDocumentIdentifier();
			textDocument->fromJson(json.value("textDocument").toObject());
			m_textDocument = textDocument;
		}

	}
};
const int DiagnosticSeverityHint = 4;
const QString MethodInitialize = "initialize";
typedef QList<CompletionItem*> CompletionItems;
const QString SemanticTokenType = "type";
class RenameOptions: virtual public GoPlsParams {
	bool *m_prepareProvider = nullptr;

public:
	RenameOptions() = default;
	virtual ~RenameOptions() {
		if(m_prepareProvider) { delete m_prepareProvider; }
	}

	void setPrepareProvider(bool *prepareProvider) { m_prepareProvider = prepareProvider; }
	bool *getPrepareProvider() { return m_prepareProvider; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_prepareProvider) {
			QJsonValue prepareProvider = QJsonValue::fromVariant(QVariant::fromValue(*m_prepareProvider));
			obj.insert("prepareProvider", prepareProvider);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("prepareProvider")) {
			auto prepareProvider = new bool(json.value("prepareProvider").toVariant().value<bool>());
			m_prepareProvider = prepareProvider;
		}

	}
};
class SemanticTokensOptions:  public WorkDoneProgressOptions {

public:
	SemanticTokensOptions() = default;
	virtual ~SemanticTokensOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject WorkDoneProgressOptions_json = WorkDoneProgressOptions::toJson();
		for(const QString &key: WorkDoneProgressOptions_json.keys()) {
			obj.insert(key, WorkDoneProgressOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		WorkDoneProgressOptions::fromJson(json);

	}
};
const int InsertTextModeAsIs = 1;
class PublishDiagnosticsParams: virtual public GoPlsParams {
	DocumentURI *m_uri = nullptr;
	unsigned int *m_version = nullptr;
	QList<Diagnostic*> *m_diagnostics = nullptr;

public:
	PublishDiagnosticsParams() = default;
	virtual ~PublishDiagnosticsParams() {
		if(m_uri) { delete m_uri; }
		if(m_version) { delete m_version; }
		if(m_diagnostics) { qDeleteAll(*m_diagnostics); delete m_diagnostics; }
	}

	void setUri(DocumentURI *uri) { m_uri = uri; }
	DocumentURI *getUri() { return m_uri; }
	void setVersion(unsigned int *version) { m_version = version; }
	unsigned int *getVersion() { return m_version; }
	void setDiagnostics(QList<Diagnostic*> *diagnostics) { m_diagnostics = diagnostics; }
	QList<Diagnostic*> *getDiagnostics() { return m_diagnostics; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_uri) {
			QJsonValue uri = QJsonValue::fromVariant(QVariant::fromValue(*m_uri));
			obj.insert("uri", uri);
		}
		if(m_version) {
			QJsonValue version = QJsonValue::fromVariant(QVariant::fromValue(*m_version));
			obj.insert("version", version);
		}
		if(m_diagnostics) {
				QJsonArray diagnostics;
			for(auto it = m_diagnostics->cbegin(); it != m_diagnostics->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				diagnostics.append(item);
			}

			obj.insert("diagnostics", diagnostics);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("uri")) {
			auto uri = new QString(json.value("uri").toString());
			m_uri = uri;
		}
		if(json.contains("version")) {
			auto version = new unsigned int(json.value("version").toVariant().value<unsigned int>());
			m_version = version;
		}
		if(json.contains("diagnostics")) {
			 QJsonArray arr = json.value("diagnostics").toArray();
			auto diagnostics = new QList<Diagnostic*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new Diagnostic();
			item->fromJson((m_item).toObject());
				diagnostics->append(item);
			}
			m_diagnostics = diagnostics;
		}

	}
};
const int CompletionItemKindConstant = 21;
class CodeLensRegistrationOptions:  public TextDocumentRegistrationOptions {
	bool *m_resolveProvider = nullptr;

public:
	CodeLensRegistrationOptions() = default;
	virtual ~CodeLensRegistrationOptions() {
		if(m_resolveProvider) { delete m_resolveProvider; }
	}

	void setResolveProvider(bool *resolveProvider) { m_resolveProvider = resolveProvider; }
	bool *getResolveProvider() { return m_resolveProvider; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		if(m_resolveProvider) {
			QJsonValue resolveProvider = QJsonValue::fromVariant(QVariant::fromValue(*m_resolveProvider));
			obj.insert("resolveProvider", resolveProvider);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		if(json.contains("resolveProvider")) {
			auto resolveProvider = new bool(json.value("resolveProvider").toVariant().value<bool>());
			m_resolveProvider = resolveProvider;
		}

	}
};
const QString MethodTextDocumentRename = "textDocument/rename";
const int MessageTypeWarning = 2;
typedef QList<DiagnosticRelatedInformation*> DiagnosticRelatedInformations;
const QString SemanticTokenTypeParameter = "typeParameter";
class SemanticTokensRegistrationOptions:  public TextDocumentRegistrationOptions, public SemanticTokensOptions, public StaticRegistrationOptions {

public:
	SemanticTokensRegistrationOptions() = default;
	virtual ~SemanticTokensRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject SemanticTokensOptions_json = SemanticTokensOptions::toJson();
		for(const QString &key: SemanticTokensOptions_json.keys()) {
			obj.insert(key, SemanticTokensOptions_json.value(key));
		}
		QJsonObject StaticRegistrationOptions_json = StaticRegistrationOptions::toJson();
		for(const QString &key: StaticRegistrationOptions_json.keys()) {
			obj.insert(key, StaticRegistrationOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextDocumentRegistrationOptions::fromJson(json);
		SemanticTokensOptions::fromJson(json);
		StaticRegistrationOptions::fromJson(json);

	}
};
const QString MethodSemanticTokensFullDelta = "textDocument/semanticTokens/full/delta";
const QString SCSSLanguage = "scss";
const QString XslLanguage = "xsl";
const QString TraceMessage = "message";
const QString MethodLinkedEditingRange = "textDocument/linkedEditingRange";
const QString PerlLanguage = "perl";
const QString SwiftLanguage = "swift";
class SymbolInformation: virtual public GoPlsParams {
	QString *m_name = nullptr;
	SymbolKind *m_kind = nullptr;
	QList<SymbolTag*> *m_tags = nullptr;
	bool *m_deprecated = nullptr;
	Location *m_location = nullptr;
	QString *m_containerName = nullptr;

public:
	SymbolInformation() = default;
	virtual ~SymbolInformation() {
		if(m_name) { delete m_name; }
		if(m_kind) { delete m_kind; }
		if(m_tags) { qDeleteAll(*m_tags); delete m_tags; }
		if(m_deprecated) { delete m_deprecated; }
		if(m_location) { delete m_location; }
		if(m_containerName) { delete m_containerName; }
	}

	void setName(QString *name) { m_name = name; }
	QString *getName() { return m_name; }
	void setKind(SymbolKind *kind) { m_kind = kind; }
	SymbolKind *getKind() { return m_kind; }
	void setTags(QList<SymbolTag*> *tags) { m_tags = tags; }
	QList<SymbolTag*> *getTags() { return m_tags; }
	void setDeprecated(bool *deprecated) { m_deprecated = deprecated; }
	bool *getDeprecated() { return m_deprecated; }
	void setLocation(Location *location) { m_location = location; }
	Location *getLocation() { return m_location; }
	void setContainerName(QString *containerName) { m_containerName = containerName; }
	QString *getContainerName() { return m_containerName; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_name) {
			QJsonValue name = QJsonValue::fromVariant(QVariant::fromValue(*m_name));
			obj.insert("name", name);
		}
		if(m_kind) {
			QJsonValue kind = QJsonValue::fromVariant(QVariant::fromValue(*m_kind));
			obj.insert("kind", kind);
		}
		if(m_tags) {
				QJsonArray tags;
			for(auto it = m_tags->cbegin(); it != m_tags->cend(); ++it) {
				auto m_item = *it;
				QJsonValue item = QJsonValue::fromVariant(QVariant::fromValue(*m_item));;
				tags.append(item);
			}

			obj.insert("tags", tags);
		}
		if(m_deprecated) {
			QJsonValue deprecated = QJsonValue::fromVariant(QVariant::fromValue(*m_deprecated));
			obj.insert("deprecated", deprecated);
		}
		if(m_location) {
			QJsonObject location = m_location->toJson();
			obj.insert("location", location);
		}
		if(m_containerName) {
			QJsonValue containerName = QJsonValue::fromVariant(QVariant::fromValue(*m_containerName));
			obj.insert("containerName", containerName);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("name")) {
			auto name = new QString(json.value("name").toString());
			m_name = name;
		}
		if(json.contains("kind")) {
			auto kind = new double(json.value("kind").toVariant().value<double>());
			m_kind = kind;
		}
		if(json.contains("tags")) {
			 QJsonArray arr = json.value("tags").toArray();
			auto tags = new QList<SymbolTag*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new double((m_item).toVariant().value<double>());
				tags->append(item);
			}
			m_tags = tags;
		}
		if(json.contains("deprecated")) {
			auto deprecated = new bool(json.value("deprecated").toVariant().value<bool>());
			m_deprecated = deprecated;
		}
		if(json.contains("location")) {
			auto location = new Location();
			location->fromJson(json.value("location").toObject());
			m_location = location;
		}
		if(json.contains("containerName")) {
			auto containerName = new QString(json.value("containerName").toString());
			m_containerName = containerName;
		}

	}
};
const int SymbolKindEnum = 10;
const int SymbolKindMethod = 6;
const QString SemanticTokenModifierDefaultLibrary = "defaultLibrary";
const QString MethodTextDocumentPublishDiagnostics = "textDocument/publishDiagnostics";
const QString MethodSetTrace = "$/setTrace";
class DeclarationRegistrationOptions:  public DeclarationOptions, public TextDocumentRegistrationOptions, public StaticRegistrationOptions {

public:
	DeclarationRegistrationOptions() = default;
	virtual ~DeclarationRegistrationOptions() {
	}

	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject DeclarationOptions_json = DeclarationOptions::toJson();
		for(const QString &key: DeclarationOptions_json.keys()) {
			obj.insert(key, DeclarationOptions_json.value(key));
		}
		QJsonObject TextDocumentRegistrationOptions_json = TextDocumentRegistrationOptions::toJson();
		for(const QString &key: TextDocumentRegistrationOptions_json.keys()) {
			obj.insert(key, TextDocumentRegistrationOptions_json.value(key));
		}
		QJsonObject StaticRegistrationOptions_json = StaticRegistrationOptions::toJson();
		for(const QString &key: StaticRegistrationOptions_json.keys()) {
			obj.insert(key, StaticRegistrationOptions_json.value(key));
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		DeclarationOptions::fromJson(json);
		TextDocumentRegistrationOptions::fromJson(json);
		StaticRegistrationOptions::fromJson(json);

	}
};
class SetTraceParams: virtual public GoPlsParams {
	TraceValue *m_value = nullptr;

public:
	SetTraceParams() = default;
	virtual ~SetTraceParams() {
		if(m_value) { delete m_value; }
	}

	void setValue(TraceValue *value) { m_value = value; }
	TraceValue *getValue() { return m_value; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_value) {
			QJsonValue value = QJsonValue::fromVariant(QVariant::fromValue(*m_value));
			obj.insert("value", value);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("value")) {
			auto value = new QString(json.value("value").toString());
			m_value = value;
		}

	}
};
const QString MethodTextDocumentFormatting = "textDocument/formatting";
class AnnotatedTextEdit:  public TextEdit {
	ChangeAnnotationIdentifier *m_annotationId = nullptr;

public:
	AnnotatedTextEdit() = default;
	virtual ~AnnotatedTextEdit() {
		if(m_annotationId) { delete m_annotationId; }
	}

	void setAnnotationId(ChangeAnnotationIdentifier *annotationId) { m_annotationId = annotationId; }
	ChangeAnnotationIdentifier *getAnnotationId() { return m_annotationId; }
	QJsonObject toJson() const {
		QJsonObject obj;
		QJsonObject TextEdit_json = TextEdit::toJson();
		for(const QString &key: TextEdit_json.keys()) {
			obj.insert(key, TextEdit_json.value(key));
		}
		if(m_annotationId) {
			QJsonValue annotationId = QJsonValue::fromVariant(QVariant::fromValue(*m_annotationId));
			obj.insert("annotationId", annotationId);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		TextEdit::fromJson(json);
		if(json.contains("annotationId")) {
			auto annotationId = new QString(json.value("annotationId").toString());
			m_annotationId = annotationId;
		}

	}
};
class CallHierarchyOutgoingCall: virtual public GoPlsParams {
	CallHierarchyItem *m_to = nullptr;
	QList<Range*> *m_fromRanges = nullptr;

public:
	CallHierarchyOutgoingCall() = default;
	virtual ~CallHierarchyOutgoingCall() {
		if(m_to) { delete m_to; }
		if(m_fromRanges) { qDeleteAll(*m_fromRanges); delete m_fromRanges; }
	}

	void setTo(CallHierarchyItem *to) { m_to = to; }
	CallHierarchyItem *getTo() { return m_to; }
	void setFromRanges(QList<Range*> *fromRanges) { m_fromRanges = fromRanges; }
	QList<Range*> *getFromRanges() { return m_fromRanges; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_to) {
			QJsonObject to = m_to->toJson();
			obj.insert("to", to);
		}
		if(m_fromRanges) {
				QJsonArray fromRanges;
			for(auto it = m_fromRanges->cbegin(); it != m_fromRanges->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				fromRanges.append(item);
			}

			obj.insert("fromRanges", fromRanges);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("to")) {
			auto to = new CallHierarchyItem();
			to->fromJson(json.value("to").toObject());
			m_to = to;
		}
		if(json.contains("fromRanges")) {
			 QJsonArray arr = json.value("fromRanges").toArray();
			auto fromRanges = new QList<Range*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new Range();
			item->fromJson((m_item).toObject());
				fromRanges->append(item);
			}
			m_fromRanges = fromRanges;
		}

	}
};
const int CompletionItemKindStruct = 22;
typedef SemanticTokensWorkspaceClientCapabilities WorkspaceClientCapabilitiesSemanticTokens;
typedef WorkspaceSymbolClientCapabilities WorkspaceClientCapabilitiesSymbol;
const QString MethodTextDocumentDocumentColor = "textDocument/documentColor";
const QString SemanticTokenVariable = "variable";
const int SignatureHelpTriggerKindContentChange = 3;
const int SymbolKindTypeParameter = 26;
const int SymbolTagDeprecated = 1;
const int SymbolKindArray = 18;
const QString MethodTextDocumentWillSave = "textDocument/willSave";
class WorkDoneProgressCancelParams: virtual public GoPlsParams {
	ProgressToken *m_token = nullptr;

public:
	WorkDoneProgressCancelParams() = default;
	virtual ~WorkDoneProgressCancelParams() {
		if(m_token) { delete m_token; }
	}

	void setToken(ProgressToken *token) { m_token = token; }
	ProgressToken *getToken() { return m_token; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_token) {
			QJsonValue token = *m_token;
			obj.insert("token", token);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("token")) {
			auto token = new QJsonValue(json.value("token"));
			m_token = token;
		}

	}
};
class CallHierarchy: virtual public GoPlsParams {
	bool *m_dynamicRegistration = nullptr;

public:
	CallHierarchy() = default;
	virtual ~CallHierarchy() {
		if(m_dynamicRegistration) { delete m_dynamicRegistration; }
	}

	void setDynamicRegistration(bool *dynamicRegistration) { m_dynamicRegistration = dynamicRegistration; }
	bool *getDynamicRegistration() { return m_dynamicRegistration; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_dynamicRegistration) {
			QJsonValue dynamicRegistration = QJsonValue::fromVariant(QVariant::fromValue(*m_dynamicRegistration));
			obj.insert("dynamicRegistration", dynamicRegistration);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("dynamicRegistration")) {
			auto dynamicRegistration = new bool(json.value("dynamicRegistration").toVariant().value<bool>());
			m_dynamicRegistration = dynamicRegistration;
		}

	}
};
class LinkedEditingRanges: virtual public GoPlsParams {
	QList<Range*> *m_ranges = nullptr;
	QString *m_wordPattern = nullptr;

public:
	LinkedEditingRanges() = default;
	virtual ~LinkedEditingRanges() {
		if(m_ranges) { qDeleteAll(*m_ranges); delete m_ranges; }
		if(m_wordPattern) { delete m_wordPattern; }
	}

	void setRanges(QList<Range*> *ranges) { m_ranges = ranges; }
	QList<Range*> *getRanges() { return m_ranges; }
	void setWordPattern(QString *wordPattern) { m_wordPattern = wordPattern; }
	QString *getWordPattern() { return m_wordPattern; }
	QJsonObject toJson() const {
		QJsonObject obj;
		if(m_ranges) {
				QJsonArray ranges;
			for(auto it = m_ranges->cbegin(); it != m_ranges->cend(); ++it) {
				auto m_item = *it;
				QJsonObject item = m_item->toJson();;
				ranges.append(item);
			}

			obj.insert("ranges", ranges);
		}
		if(m_wordPattern) {
			QJsonValue wordPattern = QJsonValue::fromVariant(QVariant::fromValue(*m_wordPattern));
			obj.insert("wordPattern", wordPattern);
		}

		return obj;
	}

	void fromJson(const QJsonObject &json) {
		if(json.contains("ranges")) {
			 QJsonArray arr = json.value("ranges").toArray();
			auto ranges = new QList<Range*>();
			for(auto it = arr.cbegin(); it != arr.cend(); ++it) {
				auto m_item = *it;
				auto item = new Range();
			item->fromJson((m_item).toObject());
				ranges->append(item);
			}
			m_ranges = ranges;
		}
		if(json.contains("wordPattern")) {
			auto wordPattern = new QString(json.value("wordPattern").toString());
			m_wordPattern = wordPattern;
		}

	}
};
}
#endif
