#include "MusicTypes.hpp"

Arcusical::Model::SongCollectionChanges::SongCollectionChanges(SongCollectionChanges&& other)
{
	this->AllSongs = std::move(other.AllSongs);
	this->NewSongs = std::move(other.NewSongs);
	this->ModifiedSongs = std::move(other.ModifiedSongs);
	this->DeletedSongs = std::move(other.DeletedSongs);
}

Arcusical::Model::AlbumCollectionChanges::AlbumCollectionChanges(AlbumCollectionChanges&& other)
{
	this->AllAlbums = std::move(other.AllAlbums);
	this->NewAlbums = std::move(other.NewAlbums);
	this->ModifiedAlbums = std::move(other.ModifiedAlbums);
	this->DeletedAlbums = std::move(other.DeletedAlbums);
}
