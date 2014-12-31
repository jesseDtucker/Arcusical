#include "pch.h"

#include "IPlayer.hpp"
#include "ViewModels/VolumeSilderVM.hpp"

using namespace Arcusical::Player;
using namespace Arcusical::ViewModel;

VolumeSliderVM::VolumeSliderVM()
{

}

double VolumeSliderVM::Volume::get()
{
	auto player = PlayerLocator::ResolveService().lock();
	ARC_ASSERT(player != nullptr);
	return player->GetVolume() * 100.0;
}


void VolumeSliderVM::Volume::set(double val)
{
	auto player = PlayerLocator::ResolveService().lock();
	ARC_ASSERT(player != nullptr);
	return player->SetVolume(val/100.0);
}

