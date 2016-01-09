/*
*	Author: Jesse Tucker
*	File: GenreTypes.hpp
*	Description: This header contains an enermuration of the predefined Genre types. The types are
*	defined as ID3 genres, however the value itunes encodes appear to be exactly 1 more than the
*	value referenced by ID3.
*/

#ifndef GENRE_TYPES_HPP
#define GENRE_TYPES_HPP

#include <stdint.h>
#include <string>
#include <unordered_map>

#include "boost\assign.hpp"

namespace Arcusical {
namespace MPEG4 {

enum class GenreType {
  BLUES,
  CLASSIC_ROCK,
  COUNTRY,
  DANCE,
  DISCO,
  FUNK,
  GRUNGE,
  HIP_HOP,
  JAZZ,
  METAL,
  NEW_AGE,
  OLDIES,
  OTHER,
  POP,
  R_AND_B,
  RAP,
  REGGAE,
  ROCK,
  TECHNO,
  INDUSTRIAL,
  ALTERNATIVE,
  SKA,
  DEATH_METAL,
  PRANKS,
  SOUNDTRACK,
  EURO_TECHNO,
  AMBIENT,
  TRIP_HOP,
  VOCAL,
  JAZZ_FUNK,
  FUSION,
  TRANCE,
  CLASSICAL,
  INSTRUMENTAL,
  ACID,
  HOUSE,
  GAME,
  SOUND_CLIP,
  GOSPEL,
  NOISE,
  ALTERNATIVE_ROCK,
  BASS,
  SOUL,
  PUNK,
  SPACE,
  MEDITATIVE,
  INSTRUMENTAL_POP,
  INSTRUMENTAL_ROCK,
  ETHNIC,
  GOTHIC,
  DARKWAVE,
  TECHNO_INDUSTRIAL,
  ELECTRONIC,
  POP_FOLK,
  EURODANCE,
  DREAM,
  SOUTHERN_ROCK,
  COMEDY,
  CULT,
  GANGSTA,
  TOP_40,
  CHRISTIAN_RAP,
  POP_FUNK,
  JUNGLE,
  NATIVE_US,
  CABARET,
  NEW_WAVE,
  PSYCHADELIC,
  RAVE,
  SHOWTUNES,
  TRAILER,
  LO_FI,
  TRIBAL,
  ACID_PUNK,
  ACID_JAZZ,
  POLKA,
  RETRO,
  MUSICAL,
  ROCK_AND_ROLL,
  HARD_ROCK,
  FOLK,
  FOLK_ROCK,
  NATIONAL_FOLK,
  SWING,
  FAST_FUSION,
  BEBOB,
  LATIN,
  REVIVAL,
  CELTIC,
  BLUEGRASS,
  AVANTGARDE,
  GOTHIC_ROCK,
  PROGRESSIVE_ROCK,
  PSYCHEDELIC_ROCK,
  SYMPHONIC_ROCK,
  SLOW_ROCK,
  BIG_BAND,
  CHORUS,
  EASY_LISTENING,
  ACOUSTIC,
  HUMOUR,
  SPEECH,
  CHANSON,
  OPERA,
  CHAMBER_MUSIC,
  SONATA,
  SYMPHONY,
  BOOTY_BASS,
  PRIMUS,
  PORN_GROOVE,
  SATIRE,
  SLOW_JAM,
  CLUB,
  TANGO,
  SAMBA,
  FOLKLORE,
  BALLAD,
  POWER_BALLAD,
  RHYTHMIC_SOUL,
  FREESTYLE,
  DUET,
  PUNK_ROCK,
  DRUM_SOLO,
  ACAPELLA,
  EURO_HOUSE,
  DANCE_HALL,
  GOA,
  DRUM_AND_BASS,
  CLUB_HOUSE,
  HARDCORE,
  TERROR,
  INDIE,
  BRITPOP,
  NEGERPUNK,
  POLSK_PUNK,
  BEAT,
  CHRISTIAN_GANGSTA_RAP,
  HEAVY_METAL,
  BLACK_METAL,
  CROSSOVER,
  CONTEMPORARY_CHRISTIAN,
  CHRISTIAN_ROCK,
  MERENGUE,
  SALSA,
  THRASH_METAL,
  ANIME,
  JPOP,
  SYNTHPOP,
  UNKNOWN
};

const std::unordered_map<uint16_t, GenreType> NumToGenre = boost::assign::map_list_of<uint16_t, GenreType>(
    1, GenreType::BLUES)(2, GenreType::CLASSIC_ROCK)(3, GenreType::COUNTRY)(4, GenreType::DANCE)(5, GenreType::DISCO)(
    6, GenreType::FUNK)(7, GenreType::GRUNGE)(8, GenreType::HIP_HOP)(9, GenreType::JAZZ)(10, GenreType::METAL)(
    11, GenreType::NEW_AGE)(12, GenreType::OLDIES)(13, GenreType::OTHER)(14, GenreType::POP)(15, GenreType::R_AND_B)(
    16, GenreType::RAP)(17, GenreType::REGGAE)(18, GenreType::ROCK)(19, GenreType::TECHNO)(20, GenreType::INDUSTRIAL)(
    21, GenreType::ALTERNATIVE)(22, GenreType::SKA)(23, GenreType::DEATH_METAL)(24, GenreType::PRANKS)(
    25, GenreType::SOUNDTRACK)(26, GenreType::EURO_TECHNO)(27, GenreType::AMBIENT)(28, GenreType::TRIP_HOP)(
    29, GenreType::VOCAL)(30, GenreType::JAZZ_FUNK)(31, GenreType::FUSION)(32, GenreType::TRANCE)(
    33, GenreType::CLASSICAL)(34, GenreType::INSTRUMENTAL)(35, GenreType::ACID)(36, GenreType::HOUSE)(
    37, GenreType::GAME)(38, GenreType::SOUND_CLIP)(39, GenreType::GOSPEL)(40, GenreType::NOISE)(
    41, GenreType::ALTERNATIVE_ROCK)(42, GenreType::BASS)(43, GenreType::SOUL)(44, GenreType::PUNK)(
    45, GenreType::SPACE)(46, GenreType::MEDITATIVE)(47, GenreType::INSTRUMENTAL_POP)(48, GenreType::INSTRUMENTAL_ROCK)(
    49, GenreType::ETHNIC)(50, GenreType::GOTHIC)(51, GenreType::DARKWAVE)(52, GenreType::TECHNO_INDUSTRIAL)(
    53, GenreType::ELECTRONIC)(54, GenreType::POP_FOLK)(55, GenreType::EURODANCE)(56, GenreType::DREAM)(
    57, GenreType::SOUTHERN_ROCK)(58, GenreType::COMEDY)(59, GenreType::CULT)(60, GenreType::GANGSTA)(
    61, GenreType::TOP_40)(62, GenreType::CHRISTIAN_RAP)(63, GenreType::POP_FUNK)(64, GenreType::JUNGLE)(
    65, GenreType::NATIVE_US)(66, GenreType::CABARET)(67, GenreType::NEW_WAVE)(68, GenreType::PSYCHADELIC)(
    69, GenreType::RAVE)(70, GenreType::SHOWTUNES)(71, GenreType::TRAILER)(72, GenreType::LO_FI)(73, GenreType::TRIBAL)(
    74, GenreType::ACID_PUNK)(75, GenreType::ACID_JAZZ)(76, GenreType::POLKA)(77, GenreType::RETRO)(
    78, GenreType::MUSICAL)(79, GenreType::ROCK_AND_ROLL)(80, GenreType::HARD_ROCK)(81, GenreType::FOLK)(
    82, GenreType::FOLK_ROCK)(83, GenreType::NATIONAL_FOLK)(84, GenreType::SWING)(85, GenreType::FAST_FUSION)(
    86, GenreType::BEBOB)(87, GenreType::LATIN)(88, GenreType::REVIVAL)(89, GenreType::CELTIC)(
    90, GenreType::BLUEGRASS)(91, GenreType::AVANTGARDE)(92, GenreType::GOTHIC_ROCK)(93, GenreType::PROGRESSIVE_ROCK)(
    94, GenreType::PSYCHEDELIC_ROCK)(95, GenreType::SYMPHONIC_ROCK)(96, GenreType::SLOW_ROCK)(97, GenreType::BIG_BAND)(
    98, GenreType::CHORUS)(99, GenreType::EASY_LISTENING)(100, GenreType::ACOUSTIC)(101, GenreType::HUMOUR)(
    102, GenreType::SPEECH)(103, GenreType::CHANSON)(104, GenreType::OPERA)(104, GenreType::CHAMBER_MUSIC)(
    106, GenreType::SONATA)(107, GenreType::SYMPHONY)(108, GenreType::BOOTY_BASS)(109, GenreType::PRIMUS)(
    110, GenreType::PORN_GROOVE)(111, GenreType::SATIRE)(112, GenreType::SLOW_JAM)(113, GenreType::CLUB)(
    114, GenreType::TANGO)(115, GenreType::SAMBA)(116, GenreType::FOLKLORE)(117, GenreType::BALLAD)(
    118, GenreType::POWER_BALLAD)(119, GenreType::RHYTHMIC_SOUL)(120, GenreType::FREESTYLE)(121, GenreType::DUET)(
    122, GenreType::PUNK_ROCK)(123, GenreType::DRUM_SOLO)(124, GenreType::ACAPELLA)(125, GenreType::EURO_HOUSE)(
    126, GenreType::DANCE_HALL)(127, GenreType::GOA)(128, GenreType::DRUM_AND_BASS)(129, GenreType::CLUB_HOUSE)(
    130, GenreType::HARDCORE)(131, GenreType::TERROR)(132, GenreType::INDIE)(133, GenreType::BRITPOP)(
    134, GenreType::NEGERPUNK)(135, GenreType::POLSK_PUNK)(136, GenreType::BEAT)(137, GenreType::CHRISTIAN_GANGSTA_RAP)(
    138, GenreType::HEAVY_METAL)(139, GenreType::BLACK_METAL)(140, GenreType::CROSSOVER)(
    141, GenreType::CONTEMPORARY_CHRISTIAN)(142, GenreType::CHRISTIAN_ROCK)(143, GenreType::MERENGUE)(
    144, GenreType::SALSA)(145, GenreType::THRASH_METAL)(146, GenreType::ANIME)(147, GenreType::JPOP)(
    148, GenreType::SYNTHPOP);

const std::unordered_map<GenreType, std::string> GenreToString = boost::assign::map_list_of<GenreType, std::string>(
    GenreType::BLUES, "Blues")(GenreType::CLASSIC_ROCK, "Classic Rock")(GenreType::COUNTRY, "Country")(
    GenreType::DANCE, "Dance")(GenreType::DISCO, "Disco")(GenreType::FUNK, "Funk")(GenreType::GRUNGE, "Grunge")(
    GenreType::HIP_HOP, "Hip Hop")(GenreType::JAZZ, "Jazz")(GenreType::METAL, "Metal")(GenreType::NEW_AGE, "New Age")(
    GenreType::OLDIES, "Oldies")(GenreType::OTHER, "Other")(GenreType::POP, "Pop")(GenreType::R_AND_B, "R&B")(
    GenreType::RAP, "Rap")(GenreType::REGGAE, "Reggae")(GenreType::ROCK, "Rock")(GenreType::TECHNO, "Techno")(
    GenreType::INDUSTRIAL, "Industrial")(GenreType::ALTERNATIVE, "Alternative")(GenreType::SKA, "Ska")(
    GenreType::DEATH_METAL, "Death Metal")(GenreType::PRANKS, "Pranks")(GenreType::SOUNDTRACK, "Soundtrack")(
    GenreType::EURO_TECHNO, "Euro Techno")(GenreType::AMBIENT, "Ambient")(GenreType::TRIP_HOP, "Trip Hop")(
    GenreType::VOCAL, "Vocal")(GenreType::JAZZ_FUNK, "Jazz Funk")(GenreType::FUSION, "Fusion")(
    GenreType::TRANCE, "Trance")(GenreType::CLASSICAL, "Classical")(GenreType::INSTRUMENTAL, "Instrumental")(
    GenreType::ACID, "Acid")(GenreType::HOUSE, "House")(GenreType::GAME, "Game")(GenreType::SOUND_CLIP, "Sound Clip")(
    GenreType::GOSPEL, "Gospel")(GenreType::NOISE, "Noise")(GenreType::ALTERNATIVE_ROCK, "Alternative Rock")(
    GenreType::BASS, "Bass")(GenreType::SOUL, "Soul")(GenreType::PUNK, "Punk")(GenreType::SPACE, "Space")(
    GenreType::MEDITATIVE, "Meditative")(GenreType::INSTRUMENTAL_POP, "Instrumental Pop")(
    GenreType::INSTRUMENTAL_ROCK, "Instrumental Rock")(GenreType::ETHNIC, "Ethnic")(GenreType::GOTHIC, "Gothic")(
    GenreType::DARKWAVE, "Darkwave")(GenreType::TECHNO_INDUSTRIAL, "Techno Industrial")(
    GenreType::ELECTRONIC, "Electronic")(GenreType::POP_FOLK, "Pop Folk")(GenreType::EURODANCE, "Eurodance")(
    GenreType::DREAM, "Dream")(GenreType::SOUTHERN_ROCK, "Southern Rock")(GenreType::COMEDY, "Comedy")(
    GenreType::CULT, "Cult")(GenreType::GANGSTA, "Gangsta")(GenreType::TOP_40, "Top 40")(
    GenreType::CHRISTIAN_RAP, "Christian Rap")(GenreType::POP_FUNK, "Pop Funk")(GenreType::JUNGLE, "Jungle")(
    GenreType::NATIVE_US, "Native US")(GenreType::CABARET, "Cabaret")(GenreType::NEW_WAVE, "New Wave")(
    GenreType::PSYCHADELIC, "Psychadelic")(GenreType::RAVE, "Rave")(GenreType::SHOWTUNES, "Showtunes")(
    GenreType::TRAILER, "Trailer")(GenreType::LO_FI, "Lo Fi")(GenreType::TRIBAL, "Tribal")(
    GenreType::ACID_PUNK, "Acid Punk")(GenreType::ACID_JAZZ, "Acid Jazz")(GenreType::POLKA, "Polka")(
    GenreType::RETRO, "Retro")(GenreType::MUSICAL, "Musical")(GenreType::ROCK_AND_ROLL, "Rock & Roll")(
    GenreType::HARD_ROCK, "Hard Rock")(GenreType::FOLK, "Folk")(GenreType::FOLK_ROCK, "Folk Rock")(
    GenreType::NATIONAL_FOLK, "National Folk")(GenreType::SWING, "Swing")(GenreType::FAST_FUSION, "Fast Fusion")(
    GenreType::BEBOB, "Bobob")(GenreType::LATIN, "Latin")(GenreType::REVIVAL, "Revival")(GenreType::CELTIC, "Celtic")(
    GenreType::BLUEGRASS, "Bluegrass")(GenreType::AVANTGARDE, "Avant Garde")(GenreType::GOTHIC_ROCK, "Gothic Rock")(
    GenreType::PROGRESSIVE_ROCK, "Progressive Rock")(GenreType::PSYCHEDELIC_ROCK, "Psychedelic Rock")(
    GenreType::SYMPHONIC_ROCK, "Symphonic Rock")(GenreType::SLOW_ROCK, "Slow Rock")(GenreType::BIG_BAND, "Big Band")(
    GenreType::CHORUS, "Chorus")(GenreType::EASY_LISTENING, "Easy Listening")(GenreType::ACOUSTIC, "Acostic")(
    GenreType::HUMOUR, "Humour")(GenreType::SPEECH, "Speech")(GenreType::CHANSON, "Chanson")(GenreType::OPERA, "Opera")(
    GenreType::CHAMBER_MUSIC, "Chamber Music")(GenreType::SONATA, "Sonata")(GenreType::SYMPHONY, "Symphony")(
    GenreType::BOOTY_BASS, "Booty Bass")(GenreType::PRIMUS, "Primus")(GenreType::PORN_GROOVE, "Porn Groove")(
    GenreType::SATIRE, "Satire")(GenreType::SLOW_JAM, "Slow Jam")(GenreType::CLUB, "Club")(GenreType::TANGO, "Tango")(
    GenreType::SAMBA, "Samba")(GenreType::FOLKLORE, "Folklore")(GenreType::BALLAD, "Ballad")(
    GenreType::POWER_BALLAD, "Power Ballad")(GenreType::RHYTHMIC_SOUL, "Rhythmic Soul")(
    GenreType::FREESTYLE, "Freestyle")(GenreType::DUET, "Duet")(GenreType::PUNK_ROCK, "Punk Rock")(
    GenreType::DRUM_SOLO, "Drum Solo")(GenreType::ACAPELLA, "Acapella")(GenreType::EURO_HOUSE, "Euro House")(
    GenreType::DANCE_HALL, "Dance Hall")(GenreType::GOA, "Goa")(GenreType::DRUM_AND_BASS, "Drum&Bass")(
    GenreType::CLUB_HOUSE, "Club House")(GenreType::HARDCORE, "HardCore")(GenreType::TERROR, "Terror")(
    GenreType::INDIE, "Indie")(GenreType::BRITPOP, "Brit-pop")(GenreType::NEGERPUNK, "NegerPunk")(
    GenreType::POLSK_PUNK, "Polsk Punk")(GenreType::BEAT, "Beat")(
    GenreType::CHRISTIAN_GANGSTA_RAP, "Christian Gangsta Rap")(GenreType::HEAVY_METAL, "Heavy Metal")(
    GenreType::BLACK_METAL, "Black Metal")(GenreType::CROSSOVER, "Crossover")(
    GenreType::CONTEMPORARY_CHRISTIAN, "Contempory Christian")(GenreType::CHRISTIAN_ROCK, "Christian Rock")(
    GenreType::MERENGUE, "Merengue")(GenreType::SALSA, "Salsa")(GenreType::THRASH_METAL, "Thrash Metal")(
    GenreType::ANIME, "anime")(GenreType::JPOP, "Jpop")(GenreType::SYNTHPOP, "Synthpop")(GenreType::UNKNOWN, "Unkown");

std::ostream& operator<<(std::ostream& outStream, GenreType type);

} /*MPEG4*/
} /*Arcusical*/

#endif