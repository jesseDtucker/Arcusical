#pragma once

#include "PropertyHelper.hpp"
#include "Subscription.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical {
namespace Player {
class IPlayer;
}
}

namespace Arcusical {
namespace ViewModel {
[Windows::UI::Xaml::Data::Bindable] public ref class VolumeSliderVM sealed : INotifyPropertyChanged_t {
 public:
  NOTIFY_PROPERTY_CHANGED_IMPL;

  property double Volume {
    double get();
    void set(double val);
  }

  internal : VolumeSliderVM(Player::IPlayer& player);

 private:
  Player::IPlayer& m_player;
};
}
}
