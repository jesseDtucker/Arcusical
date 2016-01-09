#pragma once

#include <memory>
#include <string>

#include "Alac.hpp"
#include "Box.hpp"
#include "Esds.hpp"
#include "ImageTypes.hpp"
#include "MPEG4_Tree.hpp"
#include "Mdat.hpp"
#include "Mp4a.hpp"
#include "Mvhd.hpp"
#include "iAlbum.hpp"
#include "iAlbumArtist.hpp"
#include "iArtist.hpp"
#include "iComposer.hpp"
#include "iCover.hpp"
#include "iDiskNumber.hpp"
#include "iGenre.hpp"
#include "iName.hpp"
#include "iReleaseDate.hpp"
#include "iTrackNumber.hpp"
/*
*	Author: Jesse Tucker
*	Date: Feb 24th 2013
*
*	Description:
*	This class wraps the MPEG4_Tree and provides a simple way of accessing and writing its information.
*	TODO::JT: implement means of writing information, currently only works as a reader
*/


namespace Arcusical {
namespace MPEG4 {

enum class Encoding { ALAC, AAC, UNKNOWN };

class MPEG4_SongFile {
 public:
  /*
  *	Creates a new Media File that will wrap the provided tree and use it as its source of data
  */
  MPEG4_SongFile(std::shared_ptr<MPEG4_Tree> sourceTree);

#pragma region Song Info

  std::string GetAlbum();
  std::string GetArtist();
  std::string GetAlbumArtist();
  std::string GetTitle();
  std::string GetReleaseDate();
  GenreType GetGenre();
  std::pair<uint16_t, uint16_t> GetTrackNumber();
  std::pair<uint16_t, uint16_t> GetDiskNumber();

  /*
  *	Returns the raw image data.
  */
  std::shared_ptr<std::vector<unsigned char>> GetImageData();
  ImageType GetImageType();

#pragma endregion

#pragma region Playback Information

  Encoding GetEncoding();

  /*
  *	Returns the raw media data
  */
  std::shared_ptr<std::vector<unsigned char>> GetMediaData();

  /*
  *	Returns the length in seconds. Obtained by dividing the duration
  *	by the sample rate.
  */
  uint64_t GetLength();

  /*
  *	Returns the number of samples in the playback.
  *	Note: Divide this by the sample rate to get the length
  *	in seconds.
  */
  uint64_t GetDuration();

  /*
  *	Returns the number of samples per second. A common value
  *	is 44100.
  */
  uint32_t GetSampleRate();

  /*
  *	Returns a multiplier for playback rate, typically 1.0
  */
  float GetPlaybackRate();

  /*
  *	Returns a multiplier to be applied to the volume for playback.
  *	Typically 1.0
  */
  float GetVolume();

  /*
  *	Returns the number of encoded channels
  */
  uint16_t GetNumberOfChannels();

  /*
  *	Returns the size, in bits, of a sample. Common values are 16 and 24.
  */
  uint16_t GetSampleSize();

#pragma endregion

  std::shared_ptr<MPEG4_Tree> GetTree();

 protected:
  /*
  *	The tree that this object will use as a backing data structure for all information
  */
  std::shared_ptr<MPEG4_Tree> m_tree;

#pragma region BackingReferences

  std::shared_ptr<Alac> m_alac;
  std::shared_ptr<Mp4a> m_mp4a;
  std::shared_ptr<Mvhd> m_mvhd;
  std::shared_ptr<Mdat> m_mdat;
  std::shared_ptr<iName> m_iName;
  std::shared_ptr<iReleaseDate> m_iReleaseDate;
  std::shared_ptr<iArtist> m_iArtist;
  std::shared_ptr<iAlbum> m_iAlbum;
  std::shared_ptr<iTrackNumber> m_iTrackNumber;
  std::shared_ptr<iDiskNumber> m_iDiskNumber;
  std::shared_ptr<iAlbumArtist> m_iAlbumArtist;
  std::shared_ptr<iGenre> m_iGenre;

  std::shared_ptr<iCover> m_iCover;
  std::shared_ptr<std::vector<unsigned char>> m_imageData;  // loaded on request from iCover

#pragma endregion

  /*
  *	Loads the tree and indexing all of the information for easy access.
  */
  void LoadTree(std::vector<std::shared_ptr<Box>>& boxes);

  /*
  *	Overloaded call for loading boxes
  *	base version does nothing and just ignores the box, overloads will take map the box
  *	for later reference
  */
  void LoadBox(const std::shared_ptr<Box>& box);
  bool LoadBox(const std::shared_ptr<Alac>& box);
  bool LoadBox(const std::shared_ptr<Mp4a>& box);
  bool LoadBox(const std::shared_ptr<Mvhd>& box);
  bool LoadBox(const std::shared_ptr<Mdat>& box);
  bool LoadBox(const std::shared_ptr<iName>& box);
  bool LoadBox(const std::shared_ptr<iCover>& box);
  bool LoadBox(const std::shared_ptr<iReleaseDate>& box);
  bool LoadBox(const std::shared_ptr<iArtist>& box);
  bool LoadBox(const std::shared_ptr<iAlbum>& box);
  bool LoadBox(const std::shared_ptr<iAlbumArtist>& box);
  bool LoadBox(const std::shared_ptr<iTrackNumber>& box);
  bool LoadBox(const std::shared_ptr<iDiskNumber>& box);
};

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
