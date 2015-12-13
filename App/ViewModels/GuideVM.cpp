#include "ViewModels/GuideVM.hpp"

using namespace Arcusical;
using namespace Arcusical::ViewModel;

GuideVM::GuideVM(Player::Playlist& playlist, Util::BackgroundWorker& worker)
	: m_selectedAlbum(nullptr)
	, m_playlist(playlist)
	, m_worker(worker) {

}

void GuideVM::SelectedAlbum::set(AlbumVM^ selectedAlbum) {
	m_selectedAlbum = selectedAlbum;
	auto songListVM = ref new ViewModel::SongListControlVM(m_playlist, m_worker);
	songListVM->SongList = selectedAlbum->Songs;
	this->SongListControlVM = songListVM;

	DispatchToUI([this]()
	{
		OnPropertyChanged("SelectedAlbum");
	});
}

AlbumVM^ GuideVM::SelectedAlbum::get() {
	return m_selectedAlbum;
}