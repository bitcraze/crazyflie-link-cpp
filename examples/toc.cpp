#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>

#include "crazyflieLinkCpp/Connection.h"

using namespace bitcraze::crazyflieLinkCpp;

class Benchmark
{
public:
    Benchmark(const std::string& uri)
        : uri_(uri)
    {
    }

    void run()
    {
        Connection con(uri_);

        // empty receiver queue
        while (true)
        {
            Packet p = con.recv(100);
            if (!p)
            {
                break;
            }
        }
    }


void Crazyflie::requestParamToc(bool forceNoCache)
{
  m_param_use_V2 = true;
  uint16_t numParam;
  uint32_t crc;

  // // Lazily initialize protocol version
  // if (m_protocolVersion < 0) {
  //   m_protocolVersion = getProtocolVersion();
  // }

  // Find the number of parameters in TOC
  crtpParamTocGetInfoV2Request infoRequest;
  startBatchRequest();
  // std::cout << "infoReq" << std::endl;
  addRequest(infoRequest, 1);
  if (m_protocolVersion >= 4) {
    handleRequests();
    numParam = getRequestResult<crtpParamTocGetInfoV2Response>(0)->numParam;
    crc = getRequestResult<crtpParamTocGetInfoV2Response>(0)->crc;
  } else {
    // std::cout << "Fall back to V1 param API" << std::endl;
    m_param_use_V2 = false;

    crtpParamTocGetInfoRequest infoRequest;
    startBatchRequest();
    addRequest(infoRequest, 1);
    handleRequests();
    numParam = getRequestResult<crtpParamTocGetInfoResponse>(0)->numParam;
    crc = getRequestResult<crtpParamTocGetInfoResponse>(0)->crc;
  }

  // check if it is in the cache
  std::string fileName = "params" + std::to_string(crc) + ".csv";
  std::ifstream infile(fileName);

  if (forceNoCache || !infile.good()) {
    m_logger.info("Params: " + std::to_string(numParam));

    // Request detailed information and values
    startBatchRequest();
    if (!m_param_use_V2) {
      for (uint16_t i = 0; i < numParam; ++i) {
        crtpParamTocGetItemRequest itemRequest(i);
        addRequest(itemRequest, 2);
        crtpParamReadRequest readRequest(i);
        addRequest(readRequest, 1);
      }
    } else {
      for (uint16_t i = 0; i < numParam; ++i) {
        crtpParamTocGetItemV2Request itemRequest(i);
        addRequest(itemRequest, 2);
        crtpParamReadV2Request readRequest(i);
        addRequest(readRequest, 1);
      }
    }
    handleRequests();
    // Update internal structure with obtained data
    m_paramTocEntries.resize(numParam);

    if (!m_param_use_V2) {
      for (uint16_t i = 0; i < numParam; ++i) {
        auto r = getRequestResult<crtpParamTocGetItemResponse>(i*2+0);
        auto val = getRequestResult<crtpParamValueResponse>(i*2+1);

        ParamTocEntry& entry = m_paramTocEntries[i];
        entry.id = i;
        entry.type = (ParamType)(r->length | r-> type << 2 | r->sign << 3);
        entry.readonly = r->readonly;
        entry.group = std::string(&r->text[0]);
        entry.name = std::string(&r->text[entry.group.size() + 1]);

        ParamValue v;
        std::memcpy(&v, &val->valueFloat, 4);
        m_paramValues[i] = v;
      }
    } else {
      for (uint16_t i = 0; i < numParam; ++i) {
        auto r = getRequestResult<crtpParamTocGetItemV2Response>(i*2+0);
        auto val = getRequestResult<crtpParamValueV2Response>(i*2+1);

        ParamTocEntry& entry = m_paramTocEntries[i];
        entry.id = i;
        entry.type = (ParamType)(r->length | r-> type << 2 | r->sign << 3);
        entry.readonly = r->readonly;
        entry.group = std::string(&r->text[0]);
        entry.name = std::string(&r->text[entry.group.size() + 1]);

        ParamValue v;
        std::memcpy(&v, &val->valueFloat, 4);
        m_paramValues[i] = v;
      }
    }

    // Write a cache file
    {
      // Atomic file write: write in temporary file first to avoid race conditions
      std::string fileNameTemp = fileName + ".tmp";
      std::ofstream output(fileNameTemp);
      output << "id,type,readonly,group,name" << std::endl;
      for (const auto& entry : m_paramTocEntries) {
        output << std::to_string(entry.id) << ","
               << std::to_string(entry.type) << ","
               << std::to_string(entry.readonly) << ","
               << entry.group << ","
               << entry.name << std::endl;
      }
      // change the filename
      rename(fileNameTemp.c_str(), fileName.c_str());
    }
  } else {
    m_logger.info("Found variables in cache.");
    m_paramTocEntries.clear();
    std::string line, cell;
    std::getline(infile, line); // ignore header
    while (std::getline(infile, line)) {
      std::stringstream lineStream(line);
      m_paramTocEntries.resize(m_paramTocEntries.size() + 1);
      std::getline(lineStream, cell, ',');
      m_paramTocEntries.back().id = std::stoi(cell);
      std::getline(lineStream, cell, ',');
      m_paramTocEntries.back().type = (ParamType)std::stoi(cell);
      std::getline(lineStream, cell, ',');
      m_paramTocEntries.back().readonly = std::stoi(cell);
      std::getline(lineStream, cell, ',');
      m_paramTocEntries.back().group = cell;
      std::getline(lineStream, cell, ',');
      m_paramTocEntries.back().name = cell;
    }

    // Request values
    if (!m_param_use_V2) {
      startBatchRequest();
      for (size_t i = 0; i < numParam; ++i) {
        crtpParamReadRequest readRequest(i);
        addRequest(readRequest, 1);
      }
      handleRequests();
      for (size_t i = 0; i < numParam; ++i) {
        auto val = getRequestResult<crtpParamValueResponse>(i);
        ParamValue v;
        std::memcpy(&v, &val->valueFloat, 4);
        m_paramValues[i] = v;
      }
    } else {
      startBatchRequest();
      for (size_t i = 0; i < numParam; ++i) {
        crtpParamReadV2Request readRequest(i);
        addRequest(readRequest, 1);
      }
      handleRequests();
      for (size_t i = 0; i < numParam; ++i) {
        auto val = getRequestResult<crtpParamValueV2Response>(i);
        ParamValue v;
        std::memcpy(&v, &val->valueFloat, 4);
        m_paramValues[i] = v;
      }
    }
  }
}



public:
    double latency_avg;
    double latency_min;
    double bandwidth;

private:
    std::string uri_;

};

int main()
{
    // std::vector<std::string> uris({"radio://*/80/2M/E7E7E7E7E7", "radio://*/90/2M/E7E7E7E7E7"});
    // std::vector<std::string> uris({"radio://*/80/2M/E7E7E7E7E7"});
    std::vector<std::string> uris({"usb://0"});

    std::vector<Benchmark> benchmarks;
    // std::cout << "1" << std::endl;
    benchmarks.reserve(uris.size());
    // std::cout << "1" << std::endl;
    std::vector<std::thread> threads;
    // std::cout << "1" << std::endl;
    threads.reserve(uris.size());

    for (const auto& uri : uris) {
        // std::cout << "2" << std::endl;
        benchmarks.emplace_back(Benchmark(uri));
        // std::cout << "2" << std::endl;
        threads.emplace_back(std::thread(&Benchmark::run, &benchmarks.back()));
    }

    for (auto& t : threads) {
        t.join();
    }

    for (auto& b: benchmarks) {
        std::cout << "Latency: Min: " << b.latency_min * 1000.0f << " ms; Avg: " << b.latency_avg * 1000.0f << " ms" << std::endl;
        std::cout << "Bandwidth: " << b.bandwidth << " packets/s" << std::endl;
    }

    return 0;
}