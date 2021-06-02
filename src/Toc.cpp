#include "Toc.h"

const uint8_t AccessType::RW = 0;
const uint8_t AccessType::RO = 0;

TocItem::TocItem(const bitcraze::crazyflieLinkCpp::Packet &p_recv)
{
    _paramId = 0;
    memcpy(&_paramId, &p_recv.payload()[1], sizeof(_paramId));
    uint8_t typeAndAccessType = 0;
    memcpy(&typeAndAccessType, &p_recv.payload()[3], sizeof(typeAndAccessType));
    
    _groupName = (const char *)(p_recv.payload()) + 4;
    _paramName = (const char *)(p_recv.payload()) + 4 + _groupName.length() + 1;
    _paramType = typeAndAccessType & ~ACCESS_TYPE_BYTE;

    if ((bool)(typeAndAccessType & ACCESS_TYPE_BYTE) == (bool)AccessType::RO)
    {
        _paramAccessType = AccessType::RO;
    }
    else
    {
        _paramAccessType = AccessType::RW;
    }
}

bool TocItem::operator<(const TocItem &other) const
{
    return ((unsigned)_paramId) < ((unsigned)other._paramId);
}

bool TocItem::operator>(const TocItem &other) const
{
    return ((unsigned)_paramId) > ((unsigned)other._paramId);
}

std::string to_string(AccessType const &self)
{
    return AccessType::RO == self._accessType ? "RO" : "RW";
}

AccessType &AccessType::operator=(const std::string &strAccessType)
{
    if (strAccessType == "RW")
        _accessType = AccessType::RW;
    else if (strAccessType == "RO")
        _accessType = AccessType::RO;
    return *this;
}
AccessType &AccessType::operator=(const uint8_t &accessType)
{
    _accessType = accessType;

    return *this;
}

std::ostream &operator<<(std::ostream &out, const TocItem &tocItem)
{
    out << tocItem._paramId << ": " << to_string(tocItem._paramAccessType) << ":" << to_string(tocItem._paramType) << "  " << tocItem._groupName << "." << tocItem._paramName << "  val=";

    return out;
}

TocInfo::TocInfo(const bitcraze::crazyflieLinkCpp::Packet &p_recv)
{
    memcpy(&_numberOfElements, &p_recv.payload()[1], sizeof(_numberOfElements));
    memcpy(&_crc, &p_recv.payload()[3], sizeof(_crc));
}

std::ostream &operator<<(std::ostream &out, const TocInfo &tocInfo)
{
    out << "numberOfElements: " << (int)tocInfo._numberOfElements << std::endl;
    out << "crc: " << (int)tocInfo._crc << std::endl;
    return out;
}

std::string to_string(ParamType const &self)
{
    auto res = PARAM_TYPES.find(self._paramtype);

    if (res != PARAM_TYPES.end())
    {
        return res->second;
    }
    return "? " + (int)self._paramtype;
}

ParamType &ParamType::operator=(const std::string &strParamType)
{
    for (auto element : PARAM_TYPES)
    {
        if (element.second == strParamType)
        {
            _paramtype = element.first;
            break;
        }
    }
    return *this;
}

ParamType &ParamType::operator=(const uint8_t &paramType)
{
    _paramtype = paramType;
    return *this;
}

TocInfo::TocInfo()
{
}

TocItem::~TocItem()
{
}

TocItem::TocItem()
{
}

TocInfo::~TocInfo()
{
}
TocItem::TocItem(const TocItem& other)  
{
    _groupName = other._groupName;
    _paramName = other._paramName;
    _paramType = other._paramType;
    _paramAccessType = other._paramAccessType;
    _paramId = other._paramId;
}
void Toc::insert(const TocItem& tocItem)
{
    _tocItems.insert({{tocItem._groupName, tocItem._paramName},tocItem});
}

TocItem Toc::getItem(const std::string &groupName, const std::string &paramName, bool caching) const
{
    auto res = _tocItemsCache.find({groupName, paramName});
    if(res == _tocItemsCache.end())
    {
        res = _tocItems.find({groupName, paramName});

        if(caching)
        {
            const_cast<std::map<StrPair,TocItem>&>(_tocItemsCache).insert(*res);
        }
    }
    return res->second;
}

uint16_t Toc::getItemId(const std::string &groupName, const std::string &paramName, bool caching) const
{
    return this->getItem(groupName, paramName, caching)._paramId;
}

std::vector<TocItem> Toc::getAllTocItems() const
{
    std::vector<TocItem> tocItemsVector;
    for (auto element : _tocItems)
    {
        TocItem tocItem = element.second;
        tocItemsVector.push_back(tocItem);
    }
    std::sort(tocItemsVector.begin(), tocItemsVector.end());
    return tocItemsVector;
}

std::vector<TocItem> Toc::getAllCachedTocItems() const
{
    std::vector<TocItem> tocItemsVector;
    for (auto element : _tocItemsCache)
    {
        TocItem tocItem = element.second;
        tocItemsVector.push_back(tocItem);
    }
    std::sort(tocItemsVector.begin(), tocItemsVector.end());
    return tocItemsVector;
}

void Toc::clearCache()
{
    _tocItemsCache.clear();
    _tocItems.clear();
}

void Toc::clearToc()
{
    _tocItemsCache.clear();
}


bool TocItem::isFloat() const
{
    return _paramType == FLOAT_PARAM_TYPE;
}


bool ParamType::operator==(const std::string& val) const
{
    return to_string(*this) == val; 
}

bool ParamType::operator==(uint8_t val) const
{
    return _paramtype == val;
}
