#include "Crazyflie.h"

using namespace bitcraze::crazyflieLinkCpp;

Crazyflie::Crazyflie(const std::string &uri) : _con(uri), _conWrapperParamRead(_con), _conWrapperParamWrite(_con), _conWrapperToc(_con), _conWrapperAppchannel(_con)
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

void Crazyflie::sendAppChannelData(const void *data, const size_t &dataLen)
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
    std::string strType = to_string(tocItem._paramType);

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
    std::string strType = to_string(tocItem._paramType);
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

float Crazyflie::getFloatByName(const std::string &group, const std::string &name) const
{
    uint16_t numOfElements = this->_toc._tocInfo._numberOfElements;

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

uint32_t Crazyflie::getUIntByName(const std::string &group, const std::string &name) const
{
    uint16_t numOfElements = this->_toc._tocInfo._numberOfElements;

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

bool Crazyflie::setParam(uint16_t paramId, uint32_t newValue, const size_t &valueSize)
{
    _conWrapperParamWrite.sendData(&paramId, sizeof(paramId), &newValue, valueSize);

    return true;
}

void Crazyflie::initToc()
{
    // ask for the toc info
    uint8_t cmd = CMD_TOC_INFO_V2;
    _conWrapperToc.sendData(&cmd, sizeof(uint8_t));
    Packet p_recv = _conWrapperToc.recvFilteredData(0);
    TocInfo cfTocInfo(p_recv);
 
    std::ifstream tocCsvFile(cfTocInfo._crc+".csv");
    bool doesFileExist = tocCsvFile.good();
    tocCsvFile.close();
    if(doesFileExist)
    {
        loadToc(""+cfTocInfo._crc);
        return;
    }

    _toc._tocInfo = cfTocInfo;


    uint16_t num_of_elements = _toc._tocInfo._numberOfElements;

    for (uint16_t i = 1; i <= num_of_elements; i++)
    {
        TocItem tocItem = getItemFromToc(i);
        _toc._tocItems.insert({{tocItem._groupName, tocItem._paramName}, tocItem});
    }
    
}

TocItem Crazyflie::getItemFromToc(uint16_t id) const
{
    uint8_t cmd = CMD_TOC_ITEM_V2;

    // ask for a param with the given id
    _conWrapperToc.sendData(&cmd, sizeof(uint8_t), &id, sizeof(id));
    Packet p_recv = _conWrapperToc.recvFilteredData(0);
    return TocItem(p_recv);
}

//print the TOC with values!
void Crazyflie::printToc()
{
    auto tocItems = _toc._tocItems;
    std::cout << "size: " << _toc._tocItems.size() << std::endl;
    std::cout << "size: " << _toc._tocInfo._numberOfElements << std::endl;
    for (auto element : tocItems)
    {
        TocItem tocItem = element.second;
        // tocItem
        // std::cout<< tocItem << std::endl;
        // std::string strType = to_string(tocItem._paramType);
        // std::cout << tocItem._paramId << ": " << to_string(tocItem._paramAccessType) << ":" << strType << "  " << tocItem._groupName << "." << tocItem._paramName << "  val=";
        // if (strType.find("int") != std::string::npos)
        //     std::cout << getUIntById(tocItem._paramId) << std::endl;
        // else
        //     std::cout << getFloatById(tocItem._paramId) << std::endl;
    }
}

//save the TOC to a .csv file
void Crazyflie::saveToc(const std::string& filename) const
{
    std::ofstream tocParamsFile;
    tocParamsFile.open(filename + ".csv");
    // tocParamsFile << "ID,AccessType,AccessType,Group,Name,Value" << std::endl; // the parameters will be saved to the file like this
    auto tocItems = _toc._tocItems;
    for (auto element : tocItems)
    {
        TocItem tocItem = element.second;
        // tocItem
        std::string strType = to_string(tocItem._paramType);
        std::string strAccessType = to_string(tocItem._paramAccessType);
        tocParamsFile << tocItem._paramId << "," << strAccessType << "," << strType<< "," << tocItem._groupName << "," << tocItem._paramName << ",";
        if (strAccessType.find("int") != std::string::npos)
            tocParamsFile << getUIntById(tocItem._paramId) << std::endl;
        else
            tocParamsFile << getFloatById(tocItem._paramId) << std::endl;
    }
    std::cout << "\nsuccessfully saved TOC to 'build/toc.csv'" << std::endl;
    tocParamsFile.close();
}
std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str)
{
    std::vector<std::string>   result;
    std::string                line;
    std::getline(str,line);

    std::stringstream          lineStream(line);
    std::string                cell;

    while(std::getline(lineStream,cell, ','))
    {
        result.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        result.push_back("");
    }
    return result;
}
//load the TOC from a .csv file
void Crazyflie::loadToc(const std::string& filename)
{
    std::ifstream tocParamsFile;
    tocParamsFile.open(filename + ".csv");
   // tocParamsFile << "ID,AccessType,AccessType,Group,Name,Value" << std::endl; // the parameters will be saved to the file like this
    std::vector<std::string> tocParamsFileLine;
    while (!(tocParamsFileLine = getNextLineAndSplitIntoTokens(tocParamsFile)).empty())
    {
        TocItem tocItem;
        tocItem._paramId = std::stoi(tocParamsFileLine[0]);
        tocItem._paramAccessType = tocParamsFileLine[1];
        tocItem._paramType = std::stoi(tocParamsFileLine[2]);
        tocItem._paramType = std::stoi(tocParamsFileLine[2]);
        
     
    }

    std::cout << "\nsuccessfully saved TOC to 'build/toc.csv'" << std::endl;
    tocParamsFile.close();
}



bool Crazyflie::init()
{
    initToc();
    return true;
}

