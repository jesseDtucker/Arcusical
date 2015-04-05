#ifndef XAML_MACROS_HPP
#define XAML_MACROS_HPP

#include "Arc_Assert.hpp"
#include "DispatcherHelper.hpp"

typedef Windows::UI::Xaml::Data::INotifyPropertyChanged INotifyPropertyChanged_t;

#define NOTIFY_PROPERTY_CHANGED_IMPL																\
virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;				\
void OnPropertyChanged(Platform::String^ info)														\
{																									\
	this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(info));	\
}																									\

#define VM(type)		\
private:				\
type m_vm;				\
public:					\
property type VM		\
{						\
	type get();			\
	void set(type val);	\
}						\

#define VM_IMPL(type, targetClass)				\
type targetClass ## ::VM::get()					\
{												\
	return m_vm;								\
}												\
void targetClass ## ::VM::set(type vm)			\
{												\
	ARC_ASSERT(Arcusical::HasThreadAccess());	\
	m_vm = vm;									\
	this->DataContext = m_vm;					\
}												\

#endif