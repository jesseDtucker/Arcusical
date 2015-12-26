#include "pch.h"

#include "IPlayer.hpp"
#include "ViewModels/VolumeSilderVM.hpp"

using namespace Arcusical::Player;
using namespace Arcusical::ViewModel;

VolumeSliderVM::VolumeSliderVM(IPlayer& player) : m_player(player) {}

double VolumeSliderVM::Volume::get() { return m_player.GetVolume() * 100.0; }

void VolumeSliderVM::Volume::set(double val) { return m_player.SetVolume(val / 100.0); }
