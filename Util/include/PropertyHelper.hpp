#ifndef PROPERTY_HELPER_HPP
#define PROPERTY_HELPER_HPP

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

#define PROP_GET_EX(dataType, externalName, internalName)	\
protected:													\
dataType internalName;										\
public:														\
const dataType& Get ## externalName()	const				\
{															\
	return internalName;									\
}															\


#define PROP_GET(dataType, externalName) PROP_GET_EX(dataType, externalName, m_ ## externalName)

#define PROP_SET_AND_GET_EX(dataType, externalName, internalName)	\
PROP_GET_EX(dataType, externalName, internalName)					\
void Set ## externalName(dataType value)							\
{																	\
	internalName = value;											\
}																	\
dataType& GetMutable ## externalName()								\
{																	\
	return internalName;											\
}																	\

#define PROP_SET_AND_GET(dataType, externalName) PROP_SET_AND_GET_EX(dataType, externalName, m_ ## externalName)

#define PROP_SET_AND_GET_WINRT(dataType, externalName)	\
private:												\
dataType m_ ## externalName;							\
public:													\
property dataType externalName							\
{														\
	dataType get() { return m_ ## externalName; }		\
	void set(dataType value)							\
	{													\
		Arcusical::DispatchToUI([this, value]()			\
		{												\
			if (value != m_ ## externalName)			\
			{											\
				m_ ## externalName = value;				\
				OnPropertyChanged(#externalName);		\
			}											\
		});												\
	}													\
}														\

#endif