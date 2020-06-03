#include "MM_MaterialCategory.h"

#include <locale>

#include <IBK_StringUtils.h>
#include <IBK_MultiLanguageString.h>
#include <IBK_messages.h>
#include <IBK_FormatString.h>

#include "MM_KeywordList.h"

namespace MM {

std::vector<std::map<std::string,std::string> > MaterialCategory::m_categories;

//---------------------------------------------------------------------------
// MaterialCategory
//---------------------------------------------------------------------------

MaterialCategory::MaterialCategory(CategoryID first, CategoryID second, CategoryID third) :
	m_first(first),
	m_second(second),
	m_third(third),
	m_defaultLanguage("de")
{
}

MaterialCategory::MaterialCategory(const std::string& src) :
	m_first(Invalid),
	m_second(Invalid),
	m_third(Invalid),
	m_old(src)
{
	set(src);
}

bool MaterialCategory::addCategory( CategoryID newID){

	// first we check if id is allready stored
	if (m_first == newID)
		return true;
	else if (m_second == newID)
		return true;
	else if (m_third == newID)
		return true;

	// then we add it to the first free slot
	if (m_first == Invalid){
		m_first = newID;
		return true;
	} else if (m_second == Invalid){
		m_second = newID;
		return true;
	} else if (m_third == Invalid){
		m_third = newID;
		return true;
	}

	return false;
}

void MaterialCategory::setDefaultLanguage(const std::string& lang) {
	if( lang == "de" || lang == "en")
		m_defaultLanguage = lang;
}

void MaterialCategory::set(const std::string& src) {

	const char * const FUNC_ID = "[MaterialCategory::set]  ";

	m_old = src;
	std::vector<std::string> catvect;

	// just for compatibility reasons we split comma and white space, but we only safe space separated
	size_t size = IBK::explode(src, catvect, ',', true);
	if (size == 1){
		size = IBK::explode(src, catvect, ' ', true);
	}

	m_first = m_second = m_third = Invalid;

	// just for compatibility reasons we parse old masea numbers
	// and string based values
	try	{

		// first we try to parse numbers
		if( size > 0 && !catvect[0].empty() && catvect[0][0] >= '0' && catvect[0][0] <= '9'){
			m_first = (CategoryID)IBK::string2val<int>(catvect[0]);
		}
		if( size > 1 && !catvect[1].empty() && catvect[1][0] >= '0' && catvect[1][0] <= '9'){
			m_second = (CategoryID)IBK::string2val<int>(catvect[1]);
		}
		if( size > 2 && !catvect[2].empty() && catvect[2][0] >= '0' && catvect[2][0] <= '9'){
			m_third = (CategoryID)IBK::string2val<int>(catvect[2]);
		}

		// capitalize keywords
		bool oldMatStyle = false;
		for (unsigned int i=0; i<size; ++i) {
			catvect[i] = IBK::toupper_string(catvect[i]);
			if(catvect[i] == "-")
				oldMatStyle = true;
		}

		// last evaluation try
		if( m_first == Invalid){
			m_first = (CategoryID) KeywordList::Enumeration("MaterialCategory::CategoryID", catvect[0]);
		}

		// handles old category version with kind '03 - Building Bricks' as example
		if(oldMatStyle)
			return;

		// second category isn't really necessary
		try {
			if ( size > 1 && m_second == Invalid){
				m_second = (CategoryID) KeywordList::Enumeration("MaterialCategory::CategoryID", catvect[1]);
			}
		}
		catch(std::exception& ex) {
			IBK::IBK_Message( IBK::FormatString("Couldn't parse 2nd category. %1").arg(ex.what()), IBK::MSG_WARNING, FUNC_ID );
			m_second = Invalid;
		}

		// third category isn't really necessary
		try {
			if ( size > 2 && m_third == Invalid){
				m_third = (CategoryID) KeywordList::Enumeration("MaterialCategory::CategoryID", catvect[2]);
			}
		}
		catch(std::exception& ex) {
			IBK::IBK_Message( IBK::FormatString("Couldn't parse 3rd category. %1").arg(ex.what()), IBK::MSG_WARNING, FUNC_ID );
			m_second = Invalid;
		}

	}
	catch(std::exception& ex) {
		IBK::IBK_Message( ex.what(), IBK::MSG_ERROR, FUNC_ID, 1);
		throw IBK::Exception( IBK::FormatString("Invalid parameter value '%1' for keyword '%1'!").arg(src).arg("CATEGORY"), FUNC_ID);
	}


}

std::string MaterialCategory::firstString(const std::string& languageId) const {
	if (m_first > 0)
		return toString(m_first, languageId);
	std::map<std::string,std::string> taglist;
	IBK::MultiLanguageString::explodeLangIDString(m_old, taglist);
	std::string lang(IBK::tolower_string(languageId));
	std::map<std::string,std::string>::iterator fit = taglist.find(lang);
	if( taglist.empty() || fit == taglist.end())
		return m_old;
	return fit->second;
}

std::string MaterialCategory::secondString(const std::string& languageId) const {
	return toString(m_second, languageId);
}

std::string MaterialCategory::thirdString(const std::string& languageId) const {
	return toString(m_third, languageId);
}

void MaterialCategory::clear() {
	MaterialCategory temp;
	std::swap(*this, temp);
}


// STATIC FUNCTIONS

void MaterialCategory::initCategoryNames() {
	// resize vector with category name map to number of categories
	m_categories.resize(NumCategories);

	// set english names
	for (unsigned int i=0; i<NumCategories; ++i)
		m_categories[i]["en"]				= KeywordList::Description("MaterialCategory::CategoryID", i);
}


void MaterialCategory::languages(std::vector<std::string>& langs) {
	if (m_categories.empty())
		initCategoryNames();
	langs.clear();
	for (std::map<std::string,std::string>::const_iterator it = m_categories[0].begin(), end = m_categories[0].end();
		it!=end; ++it)
	{
		langs.push_back(it->first);
	}
}


std::string MaterialCategory::toString(CategoryID id, const std::string& language) {
	if (m_categories.empty())
		initCategoryNames();
	if( id == 0 || id >= (int)m_categories.size())
		return "";
	std::map<std::string,std::string>::const_iterator it = m_categories[id].find(IBK::tolower_string(language));
	if( it == m_categories[id].end())
		return "";
	return it->second;
}

unsigned int MaterialCategory::size() {
	if (m_categories.empty())
		initCategoryNames();
	return (unsigned int)m_categories.size();
}

unsigned int MaterialCategory::toUInt() const{
	return ((0xFF & m_first) | ((0xFF & m_second) << 8) | ((0xFF & m_third) << 16));
}

void MaterialCategory::fromUInt( unsigned int myInt ){

	switch( (0xFF & myInt) ){
		case Invalid: m_first = Invalid; break;
		case Coating: m_first = Coating; break;
		case Plaster: m_first = Plaster; break;
		case Bricks: m_first = Bricks; break;
		case NaturalStones: m_first = NaturalStones; break;
		case Cementitious: m_first = Cementitious; break;
		case Insulations: m_first = Insulations; break;
		case BuildingBoards: m_first = BuildingBoards; break;
		case Woodbased: m_first = Woodbased; break;
		case NaturalMaterials: m_first = NaturalMaterials; break;
		case Soils: m_first = Soils; break;
		case CladdingSystems: m_first = CladdingSystems; break;
		case Foils: m_first = Foils; break;
		case Miscellaneous: m_first = Miscellaneous; break;
		default: break;
	}

	switch( ((0xFF00 & myInt) >> 8) ){
		case Invalid : m_second = Invalid; break;
		case Coating : m_second = Coating; break;
		case Plaster: m_second = Plaster; break;
		case Bricks: m_second = Bricks; break;
		case NaturalStones: m_second = NaturalStones; break;
		case Cementitious: m_second = Cementitious; break;
		case Insulations: m_second = Insulations; break;
		case BuildingBoards: m_second = BuildingBoards; break;
		case Woodbased: m_second = Woodbased; break;
		case NaturalMaterials: m_second = NaturalMaterials; break;
		case Soils: m_second = Soils; break;
		case CladdingSystems: m_second = CladdingSystems; break;
		case Foils: m_second = Foils; break;
		case Miscellaneous: m_second = Miscellaneous; break;
		default:
			break;
	}

	switch( ((0xFF0000 & myInt) >> 16) ){
		case Invalid : m_third = Invalid; break;
		case Coating : m_third = Coating; break;
		case Plaster: m_third = Plaster; break;
		case Bricks: m_third = Bricks; break;
		case NaturalStones: m_third = NaturalStones; break;
		case Cementitious: m_third = Cementitious; break;
		case Insulations: m_third = Insulations; break;
		case BuildingBoards: m_third = BuildingBoards; break;
		case Woodbased: m_third = Woodbased; break;
		case NaturalMaterials: m_third = NaturalMaterials; break;
		case Soils: m_third = Soils; break;
		case CladdingSystems: m_third = CladdingSystems; break;
		case Foils: m_third = Foils; break;
		case Miscellaneous: m_third = Miscellaneous; break;
		default:
			break;
	}

}

MaterialCategory::CategoryID MaterialCategory::fromString( const std::string& str ) {

	// we could clean starting and tailing spaces, tabs, etc..
	return (CategoryID) KeywordList::Enumeration("MaterialCategory::CategoryID", str);

//	if( str.find("Brick") != std::string::npos || str.find("brick") != std::string::npos)
//		return Bricks;
//	if( str.find("Mortar") != std::string::npos || str.find("mortar") != std::string::npos)
//		return Plaster;
//	if( str.find("Plaster") != std::string::npos || str.find("plaster") != std::string::npos)
//		return Plaster;
//	if( str.find("NaturalStones") != std::string::npos)
//		return NaturalStones;
//	if( str.find("Natural Stones") != std::string::npos)
//		return NaturalStones;
//	if( str.find("LightStones") != std::string::npos)
//		return Cementitious;
//	if( str.find("Cement Containing") != std::string::npos)
//		return Cementitious;
//	if( str.find("Concrete") != std::string::npos)
//		return Cementitious;
//	if( str.find("Loam") != std::string::npos)
//		return NaturalMaterials;
//	if( str.find("Insulations") != std::string::npos)
//		return Insulations;
//	if( str.find("Gypsum") != std::string::npos)
//		return BuildingBoards;
//	if( str.find("Wood") != std::string::npos)
//		return Woodbased;
//	if( str.find("Soils") != std::string::npos)
//		return Soils;
//	if( str.find("Vapour retarders") != std::string::npos)
//		return Foils;
//	if( str.find("Miscellaneous") != std::string::npos)
//		return Miscellaneous;
//	return Invalid;
}

// *** GLOBAL FUNCTIONS ***

std::ostream& operator<<(std::ostream& out, const MaterialCategory& val) {
	if( val.m_first == MM::MaterialCategory::Invalid  )
		out << val.m_old;
	else
	{
		out << MM::KeywordList::Keyword("MaterialCategory::CategoryID", val.m_first );
		if( val.m_second != MM::MaterialCategory::Invalid )
		{
			out << " " << MM::KeywordList::Keyword("MaterialCategory::CategoryID", val.m_second );
			if( val.m_third != MM::MaterialCategory::Invalid )
				out << " " << MM::KeywordList::Keyword("MaterialCategory::CategoryID", val.m_third );
		}
	}
	return out;
}


} // namespace MM

