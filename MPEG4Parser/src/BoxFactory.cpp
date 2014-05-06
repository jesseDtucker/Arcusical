/*
*	Author: Jesse Tucker
*	Date: December 27th 2012
*	Name: BoxFactory.hpp
*
*	Description: Creates Boxes based upon type.
*/

#include "pch.h"

#include "BoxFactory.hpp"

#pragma region Box Definition Includes
#include "Box.hpp"
#include "Ftyp.hpp"
#include "UnknownBox.hpp"
#include "FreeBox.hpp"
#include "Moov.hpp"
#include "Mvhd.hpp"
#include "Trak.hpp"
#include "Tkhd.hpp"
#include "Mdia.hpp"
#include "Mdhd.hpp"
#include "Hdlr.hpp"
#include "Minf.hpp"
#include "Smhd.hpp"
#include "Dinf.hpp"
#include "Dref.hpp"
#include "Url.hpp"
#include "Stbl.hpp"
#include "Stsd.hpp"
#include "Mp4a.hpp"
#include "Esds.hpp"
#include "Pinf.hpp"
#include "Sbtd.hpp"
#include "Stts.hpp"
#include "Stsc.hpp"
#include "Stsz.hpp"
#include "Stco.hpp"
#include "Udta.hpp"
#include "Meta.hpp"
#include "Ilst.hpp"
#include "ItunesDashBox.hpp"
#include "MeanBox.hpp"
#include "Name.hpp"
#include "iName.hpp"
#include "iArtist.hpp"
#include "iAlbumArtist.hpp"
#include "iAlbum.hpp"
#include "iGenre.hpp"
#include "iTrackNumber.hpp"
#include "iDiskNumber.hpp"
#include "iCpil.hpp"
#include "iPgap.hpp"
#include "iReleaseDate.hpp"
#include "iAppleID.hpp"
#include "Cprt.hpp"
#include "iCnid.hpp"
#include "iRtng.hpp"
#include "iAtID.hpp"
#include "iPlID.hpp"
#include "iGeID.hpp"
#include "iSfID.hpp"
#include "iAkID.hpp"
#include "iStik.hpp"
#include "iPurchaseDate.hpp"
#include "iXid.hpp"
#include "iFlvr.hpp"
#include "iCover.hpp"
#include "iTmpo.hpp"
#include "iComposer.hpp"
#include "Mdat.hpp"
#include "Alac.hpp"
#include "Xtra.hpp"
#include "iEncoderName.hpp"
#pragma endregion

#include "UnknownBoxTypeException.hpp"

using namespace std;

namespace Arcusical { namespace MPEG4 {

	//public static
	std::shared_ptr<Box> BoxFactory::GetBox(BoxType type)
	{
		std::shared_ptr<Box> result;

		switch(type)
		{
		case BoxType::FTYP :
			result = make_shared<Ftyp>();
			break;
		case BoxType::UNKNOWN :
			result = make_shared<UnknownBox>();
			break;
		case BoxType::FREE :
			result = make_shared<FreeBox>();
			break;
		case BoxType::MOOV :
			result = make_shared<Moov>();
			break;
		case BoxType::MVHD :
			result = make_shared<Mvhd>();
			break;
		case BoxType::TRAK :
			result = make_shared<Trak>();
			break;
		case BoxType::TKHD :
			result = make_shared<Tkhd>();
			break;
		case BoxType::MDIA :
			result = make_shared<Mdia>();
			break;
		case BoxType::MDHD :
			result = make_shared<Mdhd>();
			break;
		case BoxType::HDLR :
			result = make_shared<Hdlr>();
			break;
		case BoxType::MINF:
			result = make_shared<Minf>();
			break;
		case BoxType::SMHD:
			result = make_shared<Smhd>();
			break;
		case BoxType::DINF:
			result = make_shared<Dinf>();
			break;
		case BoxType::DREF:
			result = make_shared<Dref>();
			break;
		case BoxType::URL:
			result = make_shared<Url>();
			break;
		case BoxType::STBL:
			result = make_shared<Stbl>();
			break;
		case BoxType::STSD:
			result = make_shared<Stsd>();
			break;
		case BoxType::MP4A:
			result = make_shared<Mp4a>();
			break;
		case BoxType::ESDS:
			result = make_shared<Esds>();
			break;
		case BoxType::PINF:
			result = make_shared<Pinf>();
			break;
		case BoxType::SBTD:
			result = make_shared<Sbtd>();
			break;
		case BoxType::STTS:
			result = make_shared<Stts>();
			break;
		case BoxType::STSC:
			result = make_shared<Stsc>();
			break;
		case BoxType::STSZ:
			result = make_shared<Stsz>();
			break;
		case BoxType::STCO:
			result = make_shared<Stco>();
			break;
		case BoxType::UDTA:
			result = make_shared<Udta>();
			break;
		case BoxType::META:
			result = make_shared<Meta>();
			break;
		case BoxType::ILST:
			result = make_shared<Ilst>();
			break;
		case BoxType::ITUNES_DASH:
			result = make_shared<ItunesDashBox>();
			break;
		case BoxType::MEAN:
			result = make_shared<MeanBox>();
			break;
		case BoxType::NAME:
			result = make_shared<Name>();
			break;
		case BoxType::DATA:
			result = make_shared<DataBox>();
			break;
		case BoxType::iNAME:
			result = make_shared<iName>();
			break;
		case BoxType::iArtist:
			result = make_shared<iArtist>();
			break;
		case BoxType::iAlbumArtist:
			result = make_shared<iAlbumArtist>();
			break;
		case BoxType::iAlbum:
			result = make_shared<iAlbum>();
			break;
		case BoxType::iGenre:
			result = make_shared<iGenre>();
			break;
		case BoxType::iTrackNumber:
			result = make_shared<iTrackNumber>();
			break;
		case BoxType::iDiskNumber:
			result = make_shared<iDiskNumber>();
			break;
		case BoxType::iCpil:
			result = make_shared<iCpil>();
			break;
		case BoxType::iPgap:
			result = make_shared<iPgap>();
			break;
		case BoxType::iReleaseDate:
			result = make_shared<iReleaseDate>();
			break;
		case BoxType::iAppleID:
			result = make_shared<iAppleID>();
			break;
		case BoxType::CPRT:
			result = make_shared<Cprt>();
			break;
		case BoxType::iCNID:
			result = make_shared<iCnid>();
			break;
		case BoxType::iRtng:
			result = make_shared<iRtng>();
			break;
		case BoxType::iATID:
			result = make_shared<iAtID>();
			break;
		case BoxType::iPLID:
			result = make_shared<iPlID>();
			break;
		case BoxType::iGEID:
			result = make_shared<iGeID>();
			break;
		case BoxType::iSFID:
			result = make_shared<iSfID>();
			break;
		case BoxType::iAKID:
			result = make_shared<iAkID>();
			break;
		case BoxType::iSTIK:
			result = make_shared<iStik>();
			break;
		case BoxType::iPurchaseDate:
			result = make_shared<iPurchaseDate>();
			break;
		case BoxType::iXID:
			result = make_shared<iXID>();
			break;
		case BoxType::iFLVR:
			result = make_shared<iFlvr>();
			break;
		case BoxType::iCOVR:
			result = make_shared<iCover>();
			break;
		case BoxType::MDAT:
			result = make_shared<Mdat>();
			break;
		case BoxType::ALAC:
			result = make_shared<Alac>();
			break;
		case BoxType::iTMPO:
			result = make_shared<iTmpo>();
			break;
		case BoxType::iEncoderName:
			result = make_shared<iEncoderName>();
			break;
		case BoxType::iComposer:
			result = make_shared<iComposer>();
			break;
		case BoxType::XTRA:
			result = make_shared<Xtra>();
			break;
		default:
			throw UnknownBoxTypeException("That box type cannot be created! Check that it has been added to the factory!");
		}

		return result;
	}

}/*MPEG4*/}/*Arcusical*/