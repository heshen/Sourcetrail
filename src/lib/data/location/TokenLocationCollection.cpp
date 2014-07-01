#include "data/location/TokenLocationCollection.h"

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "utility/logging/logging.h"

TokenLocationCollection::TokenLocationCollection()
{
}

TokenLocationCollection::~TokenLocationCollection()
{
}

const TokenLocationCollection::TokenLocationFileMapType& TokenLocationCollection::getTokenLocationFiles() const
{
	return m_files;
}

size_t TokenLocationCollection::getTokenLocationFileCount() const
{
	return m_files.size();
}

const std::map<Id, TokenLocation*>& TokenLocationCollection::getTokenLocations() const
{
	return m_locations;
}

size_t TokenLocationCollection::getTokenLocationCount() const
{
	return m_locations.size();
}

TokenLocation* TokenLocationCollection::addTokenLocation(
	Id tokenId, const std::string& filePath,
	unsigned int startLineNumber, unsigned int startColumnNumber,
	unsigned int endLineNumber, unsigned int endColumnNumber)
{
	if (startLineNumber > endLineNumber || (startLineNumber == endLineNumber && startColumnNumber > endColumnNumber))
	{
		LOG_ERROR("Can't create TokenLocation with wrong boundaries.");
		return nullptr;
	}

	TokenLocationFile* file = createTokenLocationFile(filePath);
	TokenLocation* location =
		file->addTokenLocation(tokenId, startLineNumber, startColumnNumber, endLineNumber, endColumnNumber);

	m_locations.emplace(location->getId(), location);
	return location;
}

void TokenLocationCollection::removeTokenLocation(TokenLocation* location)
{
	if (!findTokenLocationById(location->getId()))
	{
		LOG_ERROR("TokenLocation is not part of this TokenLocationCollection.");
		return;
	}

	m_locations.erase(location->getId());

	TokenLocationFile* file = location->getTokenLocationFile();
	file->removeTokenLocation(location);

	if (!file->getTokenLocationLineCount())
	{
		m_files.erase(file->getFilePath());
	}
}

TokenLocation* TokenLocationCollection::findTokenLocationById(Id id) const
{
	std::map<Id, TokenLocation*>::const_iterator it = m_locations.find(id);

	if (it != m_locations.end())
	{
		return it->second;
	}

	return nullptr;
}

TokenLocationFile* TokenLocationCollection::findTokenLocationFileByPath(const std::string& filePath) const
{
	std::map<std::string, std::shared_ptr<TokenLocationFile> >::const_iterator it = m_files.find(filePath);

	if (it != m_files.end())
	{
		return it->second.get();
	}

	return nullptr;
}

void TokenLocationCollection::forEachTokenLocationFile(std::function<void(TokenLocationFile*)> func) const
{
	for (const TokenLocationFilePairType& file : m_files)
	{
		func(file.second.get());
	}
}

TokenLocation* TokenLocationCollection::addTokenLocationAsPlainCopy(const TokenLocation* location)
{
	const std::string& filePath = location->getTokenLocationLine()->getTokenLocationFile()->getFilePath();
	TokenLocationFile* file = createTokenLocationFile(filePath);
	TokenLocation* copy = file->addTokenLocationAsPlainCopy(location);

	m_locations.emplace(copy->getId(), copy);
	return copy;
}

TokenLocationFile* TokenLocationCollection::createTokenLocationFile(const std::string& filePath)
{
	TokenLocationFile* file = findTokenLocationFileByPath(filePath);

	if (file)
	{
		return file;
	}

	std::shared_ptr<TokenLocationFile> filePtr = std::make_shared<TokenLocationFile>(filePath);
	m_files.emplace(filePath, filePtr);
	return filePtr.get();
}

std::ostream& operator<<(std::ostream& ostream, const TokenLocationCollection& base)
{
	ostream << "Locations:\n";
	base.forEachTokenLocationFile([&ostream](TokenLocationFile* f)
	{
		ostream << *f;
	});
	return ostream;
}
