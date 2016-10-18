/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "MediaExplorer.h"
#include "ModificationsNotifier.h"

namespace mxp {

ModificationNotifier::ModificationNotifier(MediaExplorerPtr ml)
  : m_ml(ml)
  , m_cb(ml->GetCallbacks())
  , m_stop(false) {
}

ModificationNotifier::~ModificationNotifier() {
  if(m_notifierThread.joinable() == true) {
    m_stop = true;
    m_cond.notify_all();
    m_notifierThread.join();
  }
}

void mxp::ModificationNotifier::Start() {
  assert(m_notifierThread.get_id() == compat::Thread::id{});
  m_notifierThread = compat::Thread{ &ModificationNotifier::run, this };
}

void mxp::ModificationNotifier::NotifyMediaCreation(MediaPtr media) {
  NotifyCreation(std::move(media), m_media);
}

void mxp::ModificationNotifier::NotifyMediaModification(MediaPtr media) {
  NotifyModification(std::move(media), m_media);
}

void mxp::ModificationNotifier::NotifyMediaRemoval(int64_t mediaId) {
  NotifyRemoval(mediaId, m_media);
}

void mxp::ModificationNotifier::NotifyArtistCreation(ArtistPtr artist) {
  NotifyCreation(std::move(artist), m_artists);
}

void mxp::ModificationNotifier::NotifyArtistModification(ArtistPtr artist) {
  NotifyModification(std::move(artist), m_artists);
}

void mxp::ModificationNotifier::NotifyArtistRemoval(int64_t artist) {
  NotifyRemoval(std::move(artist), m_artists);
}

void mxp::ModificationNotifier::NotifyAlbumCreation(AlbumPtr album) {
  NotifyCreation(std::move(album), m_albums);
}

void mxp::ModificationNotifier::NotifyAlbumModification(AlbumPtr album) {
  NotifyModification(std::move(album), m_albums);
}

void mxp::ModificationNotifier::NotifyAlbumRemoval(int64_t albumId) {
  NotifyRemoval(albumId, m_albums);
}

void mxp::ModificationNotifier::NotifyAlbumTrackCreation(AlbumTrackPtr track) {
  NotifyCreation(std::move(track), m_tracks);
}

void mxp::ModificationNotifier::NotifyAlbumTrackModification(AlbumTrackPtr track) {
  NotifyModification(std::move(track), m_tracks);
}

void mxp::ModificationNotifier::NotifyAlbumTrackRemoval(int64_t trackId) {
  NotifyRemoval(trackId, m_tracks);
}

void mxp::ModificationNotifier::run() {
#if !defined(_LIBCPP_STD_VER) || (_LIBCPP_STD_VER > 11 && !defined(_LIBCPP_HAS_NO_CXX14_CONSTEXPR))
  constexpr auto ZeroTimeout = std::chrono::time_point<std::chrono::steady_clock>{};
#else
  const auto ZeroTimeout = std::chrono::time_point<std::chrono::steady_clock>{};
#endif

  // Create some other queue to swap with the ones that are used
  // by other threads. That way we can release those early and allow
  // more insertions to proceed
  Queue<IMedia> media;
  Queue<IArtist> artists;
  Queue<IAlbum> albums;
  Queue<IAlbumTrack> tracks;

  while(m_stop == false) {
    {
      std::unique_lock<compat::Mutex> lock(m_lock);
      if(m_timeout == ZeroTimeout)
        m_cond.wait(lock, [this, ZeroTimeout]() {
        return m_timeout != ZeroTimeout || m_stop == true;
      });
      m_cond.wait_until(lock, m_timeout, [this]() {
        return m_stop == true;
      });
      if(m_stop == true)
        break;
      auto now = std::chrono::steady_clock::now();
      auto nextTimeout = ZeroTimeout;
      checkQueue(m_media, media, nextTimeout, now);
      checkQueue(m_artists, artists, nextTimeout, now);
      checkQueue(m_albums, albums, nextTimeout, now);
      checkQueue(m_tracks, tracks, nextTimeout, now);
      m_timeout = nextTimeout;
    }
    Notify(std::move(media), &IMediaExplorerCb::onMediaAdded, &IMediaExplorerCb::onMediaUpdated, &IMediaExplorerCb::onMediaDeleted);
    Notify(std::move(artists), &IMediaExplorerCb::onArtistsAdded, &IMediaExplorerCb::onArtistsModified, &IMediaExplorerCb::onArtistsDeleted);
    Notify(std::move(albums), &IMediaExplorerCb::onAlbumsAdded, &IMediaExplorerCb::onAlbumsModified, &IMediaExplorerCb::onAlbumsDeleted);
    // We pass the onTrackAdded callback twice, to avoid having to do some nifty templates specialization
     //for nullptr callbacks. There is no onTracksModified callback, as tracks are never modified.
    Notify(std::move(tracks), &IMediaExplorerCb::onTracksAdded, &IMediaExplorerCb::onTracksAdded, &IMediaExplorerCb::onTracksDeleted);
  }
}

}
