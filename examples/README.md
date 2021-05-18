# TODO list
TOC/Param(s) - one class or many? maybe Manager class with a container (list)

Caching for TOC params ?
## Single struct returned instead of multiple API calls:
    auto tocItem = toc.getItemFromToc(i);
    std::string strType = toc.getAccessAndStrType(tocItem._paramType).second;
    std::string strAccessType = toc.accessTypeToStr(toc.getAccessAndStrType(tocItem._paramType).first);
## TOC.h
    std::map<uint8_t, StrPair> - change StrPair to only string
