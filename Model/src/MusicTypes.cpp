#include "pch.h"

#include "MusicTypes.hpp"

Arcusical::Model::SongCollectionChanges::SongCollectionChanges(SongCollectionChanges&& other)
{
	this->AllSongs = move(other.AllSongs);
	this->NewSongs = move(other.NewSongs);
	this->ModifiedSongs = move(other.ModifiedSongs);
	this->DeletedSongs = move(other.DeletedSongs);
}

Arcusical::Model::AlbumCollectionChanges::AlbumCollectionChanges(AlbumCollectionChanges&& other)
{
	this->AllAlbums = move(other.AllAlbums);
	this->NewAlbums = move(other.NewAlbums);
	this->ModifiedAlbums = move(other.ModifiedAlbums);
	this->DeletedAlbums = move(other.DeletedAlbums);
}
