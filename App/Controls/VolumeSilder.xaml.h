#pragma once

#include "Controls\VolumeSilder.g.h"
#include "ViewModels\VolumeSilderVM.hpp"

namespace Arcusical {
  [Windows::Foundation::Metadata::WebHostHidden] public ref class VolumeSilder sealed {
   public:
    VolumeSilder();

   private:
    ViewModel::VolumeSliderVM ^ m_vm;
  };
}
