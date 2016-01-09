/*
*	Author: Jesse Tucker
*	Date: December 16th 2012
*	Name: BoxTypes.hpp
*
*	Description: Defines the types of boxes that can be found in an MPEG4 Container. As the container
*	can be extended this may not be an exhaustive list.
*/

#ifndef BOX_TYPES_H
#define BOX_TYPES_H

#include <string>
#include <unordered_map>

#include "boost\assign.hpp"

namespace Arcusical {
namespace MPEG4 {

enum class BoxType {
  FTYP,
  FREE,
  MOOV,
  MVHD,
  TRAK,
  TKHD,
  MDIA,
  MDHD,
  HDLR,
  MINF,
  SMHD,
  DINF,
  DREF,
  URL,
  STBL,
  STSD,
  MP4A,
  ESDS,
  PINF,
  SBTD,
  STTS,
  STSC,
  STSZ,
  STCO,
  UDTA,
  META,
  ILST,
  MEAN,
  ITUNES_DASH,
  NAME,
  DATA,
  iNAME,
  iArtist,
  iAlbumArtist,
  iAlbum,
  iGenre,
  iTrackNumber,
  iDiskNumber,
  iCpil,
  iPgap,
  iReleaseDate,
  iAppleID,
  CPRT,
  iCNID,
  iRtng,
  iATID,
  iPLID,
  iGEID,
  iSFID,
  iAKID,
  iSTIK,
  iPurchaseDate,
  iXID,
  iFLVR,
  iCOVR,
  iTMPO,
  iComposer,
  iEncoderName,
  XTRA,
  MDAT,
  ALAC,
  UNKNOWN
};

const std::unordered_map<std::string, BoxType> StringToBoxType = boost::assign::map_list_of<std::string, BoxType>(
    "ftyp", BoxType::FTYP)("free", BoxType::FREE)("moov", BoxType::MOOV)("mvhd", BoxType::MVHD)("trak", BoxType::TRAK)(
    "tkhd", BoxType::TKHD)("mdia", BoxType::MDIA)("mdhd", BoxType::MDHD)("hdlr", BoxType::HDLR)("minf", BoxType::MINF)(
    "smhd", BoxType::SMHD)("dinf", BoxType::DINF)("dref", BoxType::DREF)("url ", BoxType::URL)("stbl", BoxType::STBL)(
    "stsd", BoxType::STSD)("mp4a", BoxType::MP4A)("esds", BoxType::ESDS)("pinf", BoxType::PINF)("sbtd", BoxType::SBTD)(
    "stts", BoxType::STTS)("stsc", BoxType::STSC)("stsz", BoxType::STSZ)("stco", BoxType::STCO)("udta", BoxType::UDTA)(
    "meta", BoxType::META)("ilst", BoxType::ILST)("----", BoxType::ITUNES_DASH)("mean", BoxType::MEAN)(
    "name", BoxType::NAME)("data", BoxType::DATA)("©nam", BoxType::iNAME)("©ART", BoxType::iArtist)(
    "aART", BoxType::iAlbumArtist)("©alb", BoxType::iAlbum)("gnre", BoxType::iGenre)("trkn", BoxType::iTrackNumber)(
    "disk", BoxType::iDiskNumber)("cpil", BoxType::iCpil)("pgap", BoxType::iPgap)("©day", BoxType::iReleaseDate)(
    "apID", BoxType::iAppleID)("cprt", BoxType::CPRT)("cnID", BoxType::iCNID)("rtng", BoxType::iRtng)(
    "atID", BoxType::iATID)("plID", BoxType::iPLID)("geID", BoxType::iGEID)("sfID", BoxType::iSFID)(
    "akID", BoxType::iAKID)("stik", BoxType::iSTIK)("tmpo", BoxType::iTMPO)("purd", BoxType::iPurchaseDate)(
    "xid ", BoxType::iXID)("flvr", BoxType::iFLVR)("covr", BoxType::iCOVR)("©too", BoxType::iEncoderName)(
    "©wrt", BoxType::iComposer)("mdat", BoxType::MDAT)("alac", BoxType::ALAC)("Xtra", BoxType::XTRA);

const std::unordered_map<BoxType, std::string> BoxTypeToString = boost::assign::map_list_of<BoxType, std::string>(
    BoxType::FTYP, "ftyp")(BoxType::FREE, "free")(BoxType::MOOV, "moov")(BoxType::MVHD, "mvhd")(BoxType::TRAK, "trak")(
    BoxType::TKHD, "tkhd")(BoxType::MDIA, "mdia")(BoxType::MDHD, "mdhd")(BoxType::HDLR, "hdlr")(BoxType::MINF, "minf")(
    BoxType::SMHD, "smhd")(BoxType::DINF, "dinf")(BoxType::DREF, "dref")(BoxType::URL, "url ")(BoxType::STBL, "stbl")(
    BoxType::STSD, "stsd")(BoxType::MP4A, "mp4a")(BoxType::ESDS, "esds")(BoxType::PINF, "pinf")(BoxType::SBTD, "sbtd")(
    BoxType::STTS, "stts")(BoxType::STSC, "stsc")(BoxType::STSZ, "stsz")(BoxType::STCO, "stco")(BoxType::UDTA, "udta")(
    BoxType::META, "meta")(BoxType::ILST, "ilst")(BoxType::ITUNES_DASH, "----")(BoxType::MEAN, "mean")(
    BoxType::NAME, "name")(BoxType::DATA, "data")(BoxType::iNAME, "©nam")(BoxType::iArtist, "©ART")(
    BoxType::iAlbum, "aART")(BoxType::iAlbumArtist, "©alb")(BoxType::iGenre, "gnre")(BoxType::iTrackNumber, "trkn")(
    BoxType::iDiskNumber, "disk")(BoxType::iCpil, "cpil")(BoxType::iPgap, "pgap")(BoxType::iReleaseDate, "©day")(
    BoxType::iAppleID, "apID")(BoxType::CPRT, "cprt")(BoxType::iCNID, "cnID")(BoxType::iRtng, "rtng")(
    BoxType::iATID, "atID")(BoxType::iPLID, "plID")(BoxType::iGEID, "geID")(BoxType::iSFID, "sfID")(
    BoxType::iAKID, "akID")(BoxType::iSTIK, "stik")(BoxType::iTMPO, "tmpo")(BoxType::iPurchaseDate, "purd")(
    BoxType::iXID, "xid ")(BoxType::iFLVR, "flvr")(BoxType::iCOVR, "covr")(BoxType::iEncoderName, "©too")(
    BoxType::iComposer, "©wrt")(BoxType::MDAT, "mdat")(BoxType::ALAC, "alac")(BoxType::XTRA, "Xtra");

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/

#endif /*!Box_TYPES_H*/