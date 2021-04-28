#pragma once

#include "Crazyradio.h"
#include <cstdint>

static int const CRTP_MAX_DATA_SIZE = 30;
static int const CRTP_MAXSIZE = 31;
#define CHECKSIZE(s) static_assert(sizeof(s) <= CRTP_MAXSIZE, #s " packet is too large");
#define CHECKSIZE_WITH_STATE(s, stateSize) static_assert(sizeof(s) - stateSize <= CRTP_MAXSIZE, #s " packet is too large");

static int const CRTP_MAXSIZE_RESPONSE = 32;
#define CHECKSIZE_RESPONSE(s) static_assert(sizeof(s) <= CRTP_MAXSIZE_RESPONSE, #s " packet is too large");

void quatdecompress(uint32_t comp, float q[4]);

// Header
struct crtp
{
  constexpr crtp(uint8_t port, uint8_t channel)
    : channel(channel)
    , link(3)
    , port(port)
  {
  }

  crtp(uint8_t byte)
  {
    channel = (byte >> 0) & 0x3;
    link    = (byte >> 2) & 0x3;
    port    = (byte >> 4) & 0xF;
  }

  bool operator==(const crtp& other) const {
    return channel == other.channel && port == other.port;
  }

  uint8_t channel:2;
  uint8_t link:2;
  uint8_t port:4;
} __attribute__((packed));

// Packet structure definition
typedef struct {
  uint8_t size;
  union {
    struct {
      uint8_t header;
      uint8_t data[CRTP_MAX_DATA_SIZE];
    };
    uint8_t raw[CRTP_MAX_DATA_SIZE+1];
  };
} crtpPacket_t;

struct crtpEmpty
{
  const uint8_t cmd = 0xFF;
};

// Port 0 (Console)
struct crtpConsoleResponse
{
    static bool match(const Crazyradio::Ack& response) {
      return crtp(response.data[0]) == crtp(0, 0);
    }

    crtp header;
    char text[31];
};
CHECKSIZE_RESPONSE(crtpConsoleResponse)

// Port 2 (Parameters)

enum ParamType : uint8_t 
{
  ParamTypeUint8  = 0x00 | (0x00<<2) | (0x01<<3),
  ParamTypeInt8   = 0x00 | (0x00<<2) | (0x00<<3),
  ParamTypeUint16 = 0x01 | (0x00<<2) | (0x01<<3),
  ParamTypeInt16  = 0x01 | (0x00<<2) | (0x00<<3),
  ParamTypeUint32 = 0x02 | (0x00<<2) | (0x01<<3),
  ParamTypeInt32  = 0x02 | (0x00<<2) | (0x00<<3),
  ParamTypeFloat  = 0x02 | (0x01<<2) | (0x00<<3),
};

struct crtpParamTocGetItemResponse;
struct crtpParamTocGetItemRequest
{
  crtpParamTocGetItemRequest(
    uint8_t id)
    : header(2, 0)
    , command(0)
    , id(id)
  {
  }

  bool operator==(const crtpParamTocGetItemRequest& other) const {
    return header == other.header && command == other.command && id == other.id;
  }

  typedef crtpParamTocGetItemResponse Response;

  const crtp header;
  const uint8_t command;
  uint8_t id;
} __attribute__((packed));
CHECKSIZE(crtpParamTocGetItemRequest)

struct crtpParamTocGetItemResponse
{
  static bool match(const Crazyradio::Ack& response) {
    return response.size > 5 &&
           crtp(response.data[0]) == crtp(2, 0) &&
           response.data[1] == 0;
  }

  crtpParamTocGetItemRequest request;
  uint8_t length:2; // one of ParamLength
  uint8_t type:1;   // one of ParamType
  uint8_t sign:1;   // one of ParamSign
  uint8_t res0:2;   // reserved
  uint8_t readonly:1;
  uint8_t group:1;  // one of ParamGroup
  char text[28]; // group, name
} __attribute__((packed));
CHECKSIZE_RESPONSE(crtpParamTocGetItemResponse)

struct crtpParamTocGetInfoResponse;
struct crtpParamTocGetInfoRequest
{
  crtpParamTocGetInfoRequest()
    : header(2, 0)
    , command(1)
  {
  }

  bool operator==(const crtpParamTocGetInfoRequest& other) const {
    return header == other.header && command == other.command;
  }

  typedef crtpParamTocGetInfoResponse Response;

  const crtp header;
  const uint8_t command;
} __attribute__((packed));
CHECKSIZE(crtpParamTocGetInfoRequest)

struct crtpParamTocGetInfoResponse
{
  static bool match(const Crazyradio::Ack& response) {
    return response.size == 7 &&
           crtp(response.data[0]) == crtp(2, 0) &&
           response.data[1] == 1;
  }

  crtpParamTocGetInfoRequest request;
  uint8_t numParam;
  uint32_t crc;
} __attribute__((packed));
CHECKSIZE_RESPONSE(crtpParamTocGetInfoResponse)

struct crtpParamValueResponse;
struct crtpParamReadRequest
{
  crtpParamReadRequest(
    uint8_t id)
    : header(2, 1)
    , id(id)
  {
  }

  bool operator==(const crtpParamReadRequest& other) const {
    return header == other.header && id == other.id;
  }

  typedef crtpParamValueResponse Response;

  const crtp header;
  const uint8_t id;
} __attribute__((packed));
CHECKSIZE(crtpParamReadRequest)

template <class T>
struct crtpParamWriteRequest
{
  crtpParamWriteRequest(
    uint8_t id,
    const T& value)
    : header(2, 2)
    , id(id)
    , value(value)
    {
    }

    const crtp header;
    const uint8_t id;
    const T value;
} __attribute__((packed));
CHECKSIZE(crtpParamWriteRequest<double>) // largest kind of param

struct crtpParamValueResponse
{
  static bool match(const Crazyradio::Ack& response) {
    return response.size > 2 &&
           (crtp(response.data[0]) == crtp(2, 1) ||
            crtp(response.data[0]) == crtp(2, 2));
  }

  crtpParamReadRequest request;
  union {
    uint8_t valueUint8;
    int8_t valueInt8;
    uint16_t valueUint16;
    int16_t valueInt16;
    uint32_t valueUint32;
    int32_t valueInt32;
    float valueFloat;
  };
} __attribute__((packed));
CHECKSIZE_RESPONSE(crtpParamValueResponse)
