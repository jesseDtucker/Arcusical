/*
*	Author: Jesse Tucker
*	Date: December 30th 2012
*	Name: Esds.hpp
*
*	Description: box contained within the Mp4a box. Contains information
*	regarding the encoding.
*/

#ifndef ESDS_HPP
#define ESDS_HPP

#include <unordered_map>

#include "Box.hpp"

#include "boost\assign.hpp"

namespace Arcusical {
namespace MPEG4 {

enum class ES_Tag : char { UNKNOWN, ES_DESC, DECODER_CONFIG, DECODER_INFO, SL_CONFIG_DESC };

#pragma region Tag Definitions

const std::unordered_map<uint8_t, ES_Tag> CharToESTag = boost::assign::map_list_of<uint8_t, ES_Tag>(
    0x03, ES_Tag::ES_DESC)(0x04, ES_Tag::DECODER_CONFIG)(0x05, ES_Tag::DECODER_INFO)(0x06, ES_Tag::SL_CONFIG_DESC);

struct ES_Description {
  uint16_t m_esID;
  uint8_t m_streamPriority;

  ES_Description() : m_esID(0), m_streamPriority(0) {}
};

struct DecoderConfigDescription {
  uint8_t m_objectTypeID;
  uint8_t m_streamType;
  uint32_t m_bufferSize;  // encoded as 24 bit number
  uint32_t m_maxBitRate;
  uint32_t m_averageBitRate;

  DecoderConfigDescription()
      : m_objectTypeID(0), m_streamType(0), m_bufferSize(0), m_maxBitRate(0), m_averageBitRate(0) {}
};

struct DecoderSpecificInfo {
  std::vector<unsigned char> m_data;  // no idea what the data actually is...
};

struct SL_ConfigDescription {
  std::vector<unsigned char> m_data;  // no idea what this actually is...
};

// preserves unknown sections in this tag
struct UnknownTagSection {
  std::vector<unsigned char> m_data;
};

#pragma region Tag Printing

std::ostream& operator<<(std::ostream& outStream, ES_Description& descriptor);
std::ostream& operator<<(std::ostream& outStream, DecoderConfigDescription& configDescriptor);

#pragma endregion
#pragma endregion

class Esds : public Box {
 public:
  Esds();
  virtual ~Esds() override;

  virtual void PrintBox(std::ostream& outStream, int depth = 0) override;

  uint8_t GetVersion();
  uint32_t GetFlags();
  std::shared_ptr<ES_Description> GetES_Description();
  std::shared_ptr<DecoderConfigDescription> GetDecoderConfigDescription();
  std::shared_ptr<DecoderSpecificInfo> GetDecoderSpecificInfo();
  std::shared_ptr<SL_ConfigDescription> GetSlConfigDescription();
  std::vector<UnknownTagSection> GetUnknownSections();

 protected:
  virtual void ReadContents(Util::Stream& stream) override;

  uint8_t m_version;
  uint32_t m_flags;
  std::shared_ptr<ES_Description> m_esDescription;
  std::shared_ptr<DecoderConfigDescription> m_decoderConfigDescription;
  std::shared_ptr<DecoderSpecificInfo> m_decoderSpecificInfo;
  std::shared_ptr<SL_ConfigDescription> m_slConfigDescription;
  std::vector<UnknownTagSection> m_unknown;

  /*
  *	Will parse the tags in this esds atom
  *	Preconditions: startItr must point to the first
  *	tag element.
  */
  void ParseTags(Util::Stream& stream, int bytesToRead);

  /*
  *	Reads in the next tag
  */
  ES_Tag ReadTag(Util::Stream& stream);

  /*
  *	Reads the size of the tag. Note: returned value is not the full size but rather
  *	the size of the data portion, ie size not including tag and size fields.
  */
  uint32_t ReadSize(Util::Stream& stream);

  void ReadEsDescription(Util::Stream& stream);
  void ReadDecoderConfigDescription(Util::Stream& stream);
  void ReadDecoderSpecificInfo(Util::Stream& stream, uint32_t size);
  void ReadSlConfigDescription(Util::Stream& stream, uint32_t size);
  void ReadUnknownTagSection(Util::Stream& stream, uint32_t size);
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif