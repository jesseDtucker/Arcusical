#pragma once
#ifndef INTERFACE_WRAPPER_HPP
#define INTERFACE_WRAPPER_HPP

#include <functional>
#include "Arc_Assert.hpp"

//////////////////////////////////////////////////////////////////////////
//
//	Provides a mechanism for defining an interface and wrapping that interface
//	around an implementation without using any form of inheritance. Suitable
//	for cases where several unrelated objects that adhere to an interface must
//	be used interchangeably and the duck typing provided by a template would not
//	work (e.g. Pure virtual function that takes anything with a particular interface)
//
//	General Usage:
//		1) Define a set of methods and/or operators that can be used in an interface
//		2) Define an interface wrapper that is an aggregate of the desired methods/operators
//		3) Instantiate by providing an std::weak_ptr<T> that points to the wrapped object.
//			However, anything that contains a lock method that returns a pointer to the implementation
//			will suffice.
//		4) Use interface where needed. Pass by value will not copy the underlying object
//
//		Example:
//			DefineMethod(Update, void); // method called Update, takes nothing and returns nothing
//			DefineMethod(Rotate, void, Matrix3D); // method called Rotate, takes a Matrix3D and returns nothing
//			DefineMethod(Translate, bool, Matrix3D, GroundCollisionMesh); // returns a bool and takes 2 parameters
//			DefineOperator(DereferenceOperator, *, RawCamera&);	// dereference operator that returns a RawCamera reference
//
//			// Note: typedef is just a convenience
//			typedef InterfaceWrapper<Update, Rotate, Translate, DereferenceOperator> CameraInterface;	// define the interface
//
//
//	General Notes:
//		- Method definitions only need to be defined once, a single definition can be used
//			across multiple interfaces; each interface does not need its own definition when
//			the method signatures are identical.
//
//	Technical Debt:
//		- Post and Pre Incrment/Decrement do not play nicely with the operator definitions
//			As such a work around class that contains both has been provided.
//		- Interface methods cannot reference the interface itself. Ie. a method cannot
//			return a pointer or reference or copy of the interface as methods and interfaces
//			are defined independently. This makes this mechanism ineffective for wrapping
//			iterators.
//		- Operators that require the interface as a parameter will not work. This makes comparison
//			impossible. Also methods that take another instance of an interface will not compile.
//
//////////////////////////////////////////////////////////////////////////
namespace Util
{

#define NULL_PTR_WARNING "Attempted to access freed interface!"

// Wraps any provided method and a pointer to its implementation
#define METHOD_WRAPPER(Name)																\
template<typename ReturnType, typename... Params>											\
class t_ ## Name																			\
{																							\
public:																						\
	template<typename Pimpl>																\
	t_ ## Name(Pimpl pimpl)																	\
	{																						\
		m_implCall = [pimpl](Params... params)												\
		{																					\
			auto sharedPtr = pimpl.lock();													\
			ARC_ASSERT_MSG(sharedPtr != nullptr, NULL_PTR_WARNING);	\
			return sharedPtr->Name(params...);												\
		};																					\
	}																						\
																							\
	ReturnType Name(Params... params)														\
	{																						\
		return m_implCall(params...);														\
	}																						\
																							\
private:																					\
	std::function<ReturnType(Params...)> m_implCall;										\
};																							\

// Wraps most operators, does not work well with post and pre increment (on or the other can be used,
// using both can cause problems
#define OPERATOR_WRAPPER(OperatorName, OperatorType)										\
template<typename ReturnType, typename... Params>											\
class t_ ## OperatorName																	\
{																							\
public:																						\
	template<typename Pimpl>																\
	t_ ## OperatorName(Pimpl pimpl)															\
	{																						\
		m_implCall = [pimpl](Params... params)												\
		{																					\
			auto sharedPtr = pimpl.lock();													\
			ARC_ASSERT_MSG(sharedPtr != nullptr, NULL_PTR_WARNING);	\
			return sharedPtr->operator ## OperatorType(params...);							\
		};																					\
	}																						\
																							\
	ReturnType operator ## OperatorType(Params... params)									\
	{																						\
		return m_implCall(params...);														\
	}																						\
																							\
private:																					\
	std::function<ReturnType(Params...)> m_implCall;										\
};																							\

// Explicit version of operator used for post and pre increment
template<typename ReturnType, typename... Params>
class PostAndPreIncrementOperator
{
public:
	template<typename Pimpl>
	PostAndPreIncrementOperator(Pimpl pimpl)
	{
		m_preImplCall = [pimp]()
		{
			auto sharedPtr = pimpl.lock();
			ARC_ASSERT_MSG(sharedPtr != nullptr, NULL_PTR_WARNING);
			return ++(*pimpl);
		}

		m_postImplCall = [pimpl]()
		{
			auto sharePtr = pimpl.lock();
			ARC_ASSERT_MSG(sharePtr != nullptr, NULL_PTR_WARNING);
			return (*pimpl)++;
		}
	}

	ReturnType operator++()
	{
		return m_preImplCall();
	}

	ReturnType operator++(int)
	{
		return m_postImplCall();
	}
private:
	std::function<ReturnType(Params...)> m_preImplCall;
	std::function<ReturnType(Params...)> m_postImplCall;
};

template<typename ReturnType, typename... Params>
class PostAndPreDecrementOperator
{
public:
	template<typename Pimpl>
	PostAndPreDecrementOperator(Pimpl pimpl)
	{
		m_preImplCall = [pimp]()
		{
			auto sharedPtr = pimpl.lock();
			ARC_ASSERT_MSG(sharedPtr != nullptr, NULL_PTR_WARNING);
			return --(*pimpl);
		}

		m_postImplCall = [pimpl]()
		{
			auto sharePtr = pimpl.lock();
			ARC_ASSERT_MSG(sharePtr != nullptr, NULL_PTR_WARNING);
			return (*pimpl)--;
		}
	}

	ReturnType operator--()
	{
		return m_preImplCall();
	}

	ReturnType operator--(int)
	{
		return m_postImplCall();
	}
private:
	std::function<ReturnType(Params...)> m_preImplCall;
	std::function<ReturnType(Params...)> m_postImplCall;
};

#define DefineMethod(Name, ReturnValue, ...)												\
METHOD_WRAPPER(Name)																		\
typedef t_##Name<ReturnValue, __VA_ARGS__> Name												\

// Note: This does not correctly handle overload such as pre and post increment!
#define DefineOperator(OperatorName, OperatorType, ReturnValue, ...)						\
OPERATOR_WRAPPER(OperatorName, OperatorType)												\
typedef t_ ## OperatorName<ReturnValue, __VA_ARGS__> OperatorName							\

template<typename... MethodWrappers>
class InterfaceWrapper : public MethodWrappers...
{
public:
	template<typename Pimpl>
	InterfaceWrapper(Pimpl pimpl)
		: MethodWrappers(pimpl)...
	{

	}
private:
};

}

#endif