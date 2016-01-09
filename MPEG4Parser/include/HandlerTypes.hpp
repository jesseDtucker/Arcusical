#pragma once

#include "boost\assign.hpp"
#include <string>
#include <unordered_map>

/*
*	Author: Jesse Tucker
*	Date: December 29th 2012
*	Name: HandlerTypes.hpp
*
*	Description: Defines the types of handles that can be encountered
*	in an MPEG4 container.
*/

namespace Arcusical {
namespace MPEG4 {

enum class HandlerType { Video, Audio, Hint, MetaData, Unknown };

const std::unordered_map<std::string, HandlerType> StringToHandlerType =
    boost::assign::map_list_of<std::string, HandlerType>("vide", HandlerType::Video)("soun", HandlerType::Audio)(
        "hint", HandlerType::Hint)("meta", HandlerType::MetaData);

const std::unordered_map<HandlerType, std::string> HandlerTypeToString =
    boost::assign::map_list_of<HandlerType, std::string>(HandlerType::Video, "vide")(HandlerType::Audio, "soun")(
        HandlerType::Hint, "hint")(HandlerType::MetaData, "meta")(HandlerType::Unknown, "unknown");

std::ostream& operator<<(std::ostream& outStream, HandlerType type);

} /*namespace: MPEG4*/
} /*namespace: Arcusical*/
