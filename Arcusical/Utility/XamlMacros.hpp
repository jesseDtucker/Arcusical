#ifndef XAML_MACROS_HPP
#define XAML_MACROS_HPP

typedef Windows::UI::Xaml::Data::INotifyPropertyChanged INotifyPropertyChanged_t;

#define NOTIFY_PROPERTY_CHANGED_IMPL																\
virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;				\
void OnPropertyChanged(Platform::String^ info)														\
{																									\
	this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(info));	\
}																									\


#endif