#include <future>
#include <random>

#include "DefaultAlbumArt.hpp"

using namespace std;

static const vector<wstring> DEFAULT_IMAGES =
{
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_black.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_cyan.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_dark_blue.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_dark_red.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_fuchsia.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_green.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_light_cyan.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_light_green.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_lime.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_orange.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_red.png",
	L"ms-appx:///Assets/DefaultAlbumBackgrounds/default_violet.png"
};

static mutex s_lock;

bool Arcusical::Model::isDefaultAlbumArt(const wstring& path)
{
	return any_of(begin(DEFAULT_IMAGES), end(DEFAULT_IMAGES), [&path](const wstring& a)
	{
		return a == path;
	});
}

wstring Arcusical::Model::GetDefaultArt()
{
	unique_lock<mutex> lock(s_lock);

	static vector<wstring> artBag;
	static random_device rd;
	static default_random_engine rand(rd());

	// then we pull one out of the bag
	// images are selected using a Tetris algorithm
	// the images are selected until none are available then
	// the bag is refilled and selection continues. This
	// guarantees a mostly uniform selection regardless of 
	// probability and the ensuing chaos of occasionally unlikely odds
	if (artBag.size() == 0) // make sure to refill the bag when it gets empty
	{
		artBag = DEFAULT_IMAGES;
		shuffle(begin(artBag), end(artBag), rand);
	}

	auto path = artBag.back();
	artBag.pop_back();
	return path;
}
