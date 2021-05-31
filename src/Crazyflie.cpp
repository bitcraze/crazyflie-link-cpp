#include "Crazyflie.h"

using namespace bitcraze::crazyflieLinkCpp;

Crazyflie::Crazyflie(const std::string& uri) : _con(uri), _conWrapperParamRead(_con),_conWrapperParamWrite(_con), _conWrapperToc(_con), _conWrapperAppchannel(_con)
{
    _conWrapperToc.setPort(PARAM_PORT);
    _conWrapperParamRead.setPort(PARAM_PORT);
    _conWrapperParamWrite.setPort(PARAM_PORT);
    _conWrapperAppchannel.setPort(APPCHANNEL_PORT);

    _conWrapperToc.setChannel(TOC_CHANNEL);
    _conWrapperParamRead.setChannel(PARAM_READ_CHANNEL);
    _conWrapperParamWrite.setChannel(PARAM_WRITE_CHANNEL);
    _conWrapperAppchannel.setChannel(APP_CHANNEL);

}

void Crazyflie::sendAppChannelData(const void* data, const size_t& dataLen)
{
    _conWrapperAppchannel.sendData(data, dataLen);
}

std::vector<uint8_t> Crazyflie::recvAppChannelData()
{

    Packet p = _conWrapperAppchannel.recvFilteredData(0);

    std::vector<uint8_t> res;
    std::copy(p.payload(), p.payload() + p.payloadSize(), std::back_inserter(res));
    return res;
}



float Crazyflie::getFloat(uint16_t paramId) const
{
    float res = 0;

    _conWrapperParamRead.sendData(&paramId, sizeof(paramId));
    Packet p = _conWrapperParamRead.recvFilteredData(0);
    std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, sizeof(res));

    return res;
}

uint32_t Crazyflie::getUInt(uint16_t paramId) const
{

    uint32_t res = 0;

    _conWrapperParamRead.sendData(&paramId, sizeof(paramId));

   Packet p = _conWrapperParamRead.recvFilteredData(0);
 
    std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, p.payloadSize() - PAYLOAD_VALUE_BEGINING_INDEX);

    return res;
}

float Crazyflie::getFloatById(uint16_t paramId) const
{
    auto tocItem = this->getItemFromToc(paramId);
    std::string strType = this->getAccessAndStrType(tocItem._paramType).second;

    if ("float" == strType)
    {
        return getFloat(paramId);
    }
    else
    {
        std::cout << "Didn't find anything!" << std::endl;
        return NOT_FOUND;
    }
}

uint32_t Crazyflie::getUIntById(uint16_t paramId) const
{

    auto tocItem = this->getItemFromToc(paramId);
    std::string strType = this->getAccessAndStrType(tocItem._paramType).second;
    if (strType.find("int") != std::string::npos)
    {
        return getUInt(paramId);
    }
    else
    {
        std::cout << "Didn't find anything!" << std::endl;
        return NOT_FOUND;
    }
}

float Crazyflie::getFloatByName(const std::string& group, const std::string& name) const
{
    uint16_t numOfElements = this->getTocInfo()._numberOfElements;

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = this->getItemFromToc(i);

        if (tocItem._groupName == group && tocItem._paramName == name)
        {

            return getFloatById(tocItem._paramId);
        }
    }
    std::cout << "Didn't find anything!" << std::endl;
    return NOT_FOUND;
}

uint32_t Crazyflie::getUIntByName(const std::string& group, const std::string& name) const
{
    uint16_t numOfElements = this->getTocInfo()._numberOfElements;

    for (uint16_t i = 0; i < numOfElements; i++)
    {
        auto tocItem = this->getItemFromToc(i);

        if (tocItem._groupName == group && tocItem._paramName == name)
        {
            return getUIntById(tocItem._paramId);
        }
    }
    std::cout << "Didn't find anything!" << std::endl;
    return NOT_FOUND;
}

Crazyflie::~Crazyflie()
{
}

bool Crazyflie::setParam(uint16_t paramId, float newValue) 
{
    _conWrapperParamWrite.sendData(&paramId, sizeof(paramId), &newValue, sizeof(newValue));

    return true;
}

bool Crazyflie::setParam(uint16_t paramId, uint32_t newValue, const size_t& valueSize) 
{
    _conWrapperParamWrite.sendData(&paramId, sizeof(paramId), &newValue, valueSize);

    return true;
}

TocItem::TocItem(const Packet& p_recv)
{
    _cmdNum = p_recv.payload()[0];
    _paramId = 0;
    memcpy(&_paramId, &p_recv.payload()[1], sizeof(_paramId));
    _paramType = p_recv.payload()[3];
    _groupName = (const char *)(p_recv.payload())+4;
    _paramName = (const char *)(p_recv.payload())+ 4 + _groupName.length() + 1;
}

std::ostream &operator<<(std::ostream &out, const TocItem &tocItem)
{
    out << "cmdNum: " << (int)tocItem._cmdNum << std::endl;
    out << "paramId: " << (int)tocItem._paramId << std::endl;
    out << "paramType: " << (int)tocItem._paramType << std::endl;
    out << "groupName: " << tocItem._groupName << std::endl;
    out << "paramName: " << tocItem._paramName << std::endl;
    return out;
}




TocInfo::TocInfo(const Packet &p_recv) 
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


TocInfo Crazyflie::getTocInfo() const 
{
    // ask for the toc info
    uint8_t cmd = CMD_TOC_INFO_V2;
    _conWrapperToc.sendData(&cmd, sizeof(uint8_t));
   Packet p_recv = _conWrapperToc.recvFilteredData(0);

    return TocInfo(p_recv);
}

TocItem Crazyflie::getItemFromToc(uint16_t id) const
{
    uint8_t cmd = CMD_TOC_ITEM_V2;

    // ask for a param with the given id
    _conWrapperToc.sendData(&cmd, sizeof(uint8_t), &id, sizeof(id));
   Packet p_recv = _conWrapperToc.recvFilteredData(0);
    return TocItem(p_recv);
}

std::vector<TocItem> Crazyflie::getToc()
{
    std::vector<TocItem> tocItems;
    uint16_t num_of_elements = getTocInfo()._numberOfElements;
    for (uint16_t i = 0; i < num_of_elements; i++)
    {
        tocItems.push_back(getItemFromToc(i));
    }
    return tocItems;
}

//print the TOC with values!
void Crazyflie::printToc()
{
    auto tocItems = getToc(); 
    for (TocItem tocItem : tocItems)
    {
        // tocItem
        auto accessAndType = getAccessAndStrType(tocItem._paramType);
        std::cout << tocItem._paramId << ": " << accessTypeToStr(accessAndType.first) << ":" << accessAndType.second << "  " << tocItem._groupName << "." << tocItem._paramName << "  val=";
        if(accessAndType.second.find("int") != std::string::npos)
            std::cout << getUIntById(tocItem._paramId) << std::endl;
        else 
            std::cout << getFloatById(tocItem._paramId) << std::endl;
    }
}

//save the TOC to a .csv file
void Crazyflie::saveToc(std::string filename)
{
    std::ofstream tocParamsFile;
    tocParamsFile.open(filename + ".csv");
    // tocParamsFile << "ID,AccessType,AccessType,Group,Name,Value" << std::endl; // the parameters will be saved to the file like this
    auto tocItems = getToc(); 
    for (TocItem tocItem : tocItems)
    {
        // tocItem
        auto accessAndType = getAccessAndStrType(tocItem._paramType);
        tocParamsFile << tocItem._paramId << "," << accessTypeToStr(accessAndType.first) << "," << accessAndType.second << "," << tocItem._groupName << "," << tocItem._paramName << ",";
        if(accessAndType.second.find("int") != std::string::npos)
            tocParamsFile << getUIntById(tocItem._paramId) << std::endl;
        else 
            tocParamsFile << getFloatById(tocItem._paramId) << std::endl;
    }
    std::cout << "\nsuccessfully saved TOC to 'build/toc.csv'" << std::endl;
    tocParamsFile.close();
}

std::string Crazyflie::accessTypeToStr(int accessType)
{
    return RO_ACCESS == accessType ? "RO" : "RW";
}

std::pair<int, std::string> Crazyflie::getAccessAndStrType(uint8_t type)
{
    int accessType;

    if ((bool)(type & ACCESS_TYPE_BYTE) == (bool)RO_ACCESS)
    {
        accessType = RO_ACCESS;
        type = type & ~ACCESS_TYPE_BYTE;
    }
    else
    {
        accessType = RW_ACCESS;
    }
    
    return std::pair<int, std::string>(accessType, PARAM_TYPES.find(type)->second.first);
}
