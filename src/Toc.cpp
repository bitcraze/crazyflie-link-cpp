#include "Toc.h"

const uint8_t AccessType::RW = 0;
const uint8_t AccessType::RO = 0;

TocItem::TocItem(const bitcraze::crazyflieLinkCpp::Packet &p_recv)
{
    _paramId = 0;
    memcpy(&_paramId, &p_recv.payload()[1], sizeof(_paramId));
    uint8_t typeAndAccessType = p_recv.payload()[3];
    _groupName = (const char *)(p_recv.payload()) + 4;
    _paramName = (const char *)(p_recv.payload()) + 4 + _groupName.length() + 1;

    if ((bool)(typeAndAccessType & ACCESS_TYPE_BYTE) == (bool)AccessType::RO)
    {
        _paramAccessType = AccessType::RO;
        _paramType = typeAndAccessType & ~ACCESS_TYPE_BYTE;
    }
    else
    {
        _paramAccessType = AccessType::RW;
    }
}


bool TocItem::operator<(const TocItem& other) 
{
    return (_paramId) < (other._paramId);
}

bool TocItem::operator>(const TocItem& other) 
{
    return (_paramId) > (other._paramId);
}


std::string to_string(AccessType const& self)
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
    out << "paramId: " << (int)tocItem._paramId << std::endl;
    out << "paramType: " << to_string(tocItem._paramType) << std::endl;
    out << "paramAccessType: " << to_string(tocItem._paramAccessType) << std::endl;
    out << "groupName: " << tocItem._groupName << std::endl;
    out << "paramName: " << tocItem._paramName << std::endl;
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

std::string to_string(ParamType const& self) 
{
    auto res = PARAM_TYPES.find(self._paramtype);

    if(res != PARAM_TYPES.end())
    {
        return res->second;
    }
    return "?";
}

ParamType& ParamType::operator=(const std::string& strParamType) 
{
    for(auto element : PARAM_TYPES)
    {
        if(element.second ==strParamType)
        {
            _paramtype = element.first;
            break;
        }
    }
    return *this;
}

ParamType& ParamType::operator=(const uint8_t& paramType) 
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
