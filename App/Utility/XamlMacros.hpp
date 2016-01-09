#ifndef XAML_MACROS_HPP
#define XAML_MACROS_HPP

#include "Arc_Assert.hpp"
#include "DispatcherHelper.hpp"

typedef Windows::UI::Xaml::Data::INotifyPropertyChanged INotifyPropertyChanged_t;

#define NOTIFY_PROPERTY_CHANGED_IMPL                                                              \
  \
virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^                              \
      PropertyChanged;                                                                            \
  \
void OnPropertyChanged(Platform::String ^ info) \
{                                              \
    this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(info)); \
  \
}

#define VM_DECLARATION(type) \
  \
private:                     \
  \
type m_viewModel;            \
  \
public:                      \
  \
property type VM \
{        \
    type get();              \
    void set(type val);      \
  \
}

#define VM_IMPL(type, targetClass)                         \
  \
type targetClass## ::VM::get() \
{                        \
    if (m_viewModel != this->DataContext) {                \
      m_viewModel = dynamic_cast<type>(this->DataContext); \
    }                                                      \
    return m_viewModel;                                    \
  \
}                                                     \
  \
void targetClass## ::VM::set(type vm) \
{                 \
    ARC_ASSERT(Arcusical::HasThreadAccess());              \
    m_viewModel = vm;                                      \
    this->DataContext = m_viewModel;                       \
  \
}

#endif