#pragma once
#ifndef VOLUME_SLIDER_HPP

#include "Subscription.hpp"
#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical
{
	namespace ViewModel
	{
		[Windows::UI::Xaml::Data::Bindable]
		public ref class VolumeSliderVM sealed : INotifyPropertyChanged_t
		{
		public:

			VolumeSliderVM();

			NOTIFY_PROPERTY_CHANGED_IMPL;

			property double Volume
			{
				double get();
				void set(double val);
			}

			
		private:
		};
}
}

#endif