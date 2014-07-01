#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <vector>

#include "data/graph/Graph.h"
#include "data/location/TokenLocationCollection.h"
#include "data/parser/ParserClient.h"

class Storage: public ParserClient
{
public:
	Storage();
	virtual ~Storage();

	virtual void onTypedefParsed(
		const ParseLocation& location, const std::string& fullName, const std::string& underlyingFullName,
		AccessType access
	);
	virtual void onClassParsed(const ParseLocation& location, const std::string& fullName, AccessType access);
	virtual void onStructParsed(const ParseLocation& location, const std::string& fullName, AccessType access);

	virtual void onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable);
	virtual void onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access);

	virtual void onFunctionParsed(
		const ParseLocation& location, const std::string& fullName, const std::string& returnTypeName,
		const std::vector<ParseVariable>& parameters);
	virtual void onMethodParsed(
		const ParseLocation& location, const std::string& fullName, const std::string& returnTypeName,
		const std::vector<ParseVariable>& parameters, AccessType access, AbstractionType abstraction,
		bool isConst, bool isStatic);

	virtual void onNamespaceParsed(const ParseLocation& location, const std::string& fullName);

	virtual void onEnumParsed(const ParseLocation& location, const std::string& fullName, AccessType access);
	virtual void onEnumFieldParsed(const ParseLocation& location, const std::string& fullName);

	void logGraph() const;
	void logLocations() const;

private:
	Edge::AccessType convertAccessType(ParserClient::AccessType access) const;
	TokenLocation* addTokenLocation(Token* token, const ParseLocation& location);

	void log(std::string type, std::string str, const ParseLocation& location) const;

	Graph m_graph;
	TokenLocationCollection m_locationCollection;
};

#endif // STORAGE_H
