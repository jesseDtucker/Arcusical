#pragma once

#include "MulticastDelegate.hpp"
#include "Subscription.hpp"

/************************************************************************/
/* Author: Jesse Tucker
/* Date: Dec. 21st 2013
/*
/* Description:
/*	Set of helper macros for generating properties for objects, creates
/*	a basic getter and/or setter that can be added to an object along with
/*	the backing data.
/************************************************************************/

// needed for properties that have multiple template arguments in their type
#define COMMA ,
#define NOT_CONST  // used for non const getters that shouldn't have setters.

#define PROP_GET_EX(dataType, externalName, internalName, constModifier) \
  \
protected:                                                               \
  \
dataType internalName;                                                   \
  \
public:                                                                  \
  \
constModifier dataType& Get##externalName() constModifier \
{           \
    return internalName;                                                 \
  \
}

#define PROP_GET(dataType, externalName) PROP_GET_EX(dataType, externalName, m_##externalName, const)

#define PROP_SET_AND_GET_EX(dataType, externalName, internalName, constModifier) \
  \
PROP_GET_EX(dataType, externalName, internalName, constModifier)                 \
  \
void Set##externalName(dataType value) \
{                                      \
    if (internalName != value) {                                                 \
      internalName = value;                                                      \
      /*On ## externalName ## Changed(internalName);*/                           \
    }                                                                            \
  \
}                                                                           \
  \
dataType& GetMutable##externalName() \
{                                        \
    return internalName;                                                         \
  \
}
//Util::MulticastDelegate<void(dataType&)> On ## externalName ## Changed; \

#define PROP_SET_AND_GET(dataType, externalName) PROP_SET_AND_GET_EX(dataType, externalName, m_##externalName, const)

#define PROP_SET_AND_GET_WINRT(dataType, externalName)                                             \
  \
private:                                                                                           \
  \
dataType m_##externalName;                                                                         \
  \
internal:                                                                                          \
  \
Util::MulticastDelegate<void(dataType&)>                                                           \
      On##externalName##Changed;                                                                   \
  \
public:                                                                                            \
  \
property dataType externalName \
{                                                                \
    dataType get() { return m_##externalName; }                                                    \
    void set(dataType value) {                                                                     \
      ARC_ASSERT_MSG(Arcusical::HasThreadAccess(), "Property can only be accessed by UI thread!"); \
      if (value != m_##externalName) {                                                             \
        m_##externalName = value;                                                                  \
        On##externalName##Changed(m_##externalName);                                               \
        OnPropertyChanged(#externalName);                                                          \
      }                                                                                            \
    }                                                                                              \
  \
}
