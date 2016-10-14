/*****************************************************************************
* Media Explorer
*****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstdlib>
#include "Album.h"
#include "AlbumTrack.h"
#include "Artist.h"
#include "MediaFile.h"
#include "Genre.h"
#include "Media.h"
#include "MetadataService.h"
#include "Show.h"
#include "utils/Filename.h"
#include "utils/ModificationsNotifier.h"

bool mxp::MetadataService::Initialize() {
  m_unknownArtist = Artist::Fetch(m_ml, UnknownArtistID);
  if (m_unknownArtist == nullptr)
  LOG_ERROR("Failed to cache unknown artist");
  return m_unknownArtist != nullptr;
}

const char* mxp::MetadataService::Name() const {
  return "Metadata";
}

uint8_t mxp::MetadataService::nbThreads() const {
  //    auto nbProcs = std::thread::hardware_concurrency();
  //    if (nbProcs == 0)
  //        return 1;
  //    return nbProcs;
  // Let's make this code thread-safe first :)
  return 1;
}

mxp::parser::Task::Status mxp::MetadataService::Run(mxp::parser::Task& task) {
  auto& media = task.Media;

  auto t = m_ml->GetConnection()->NewTransaction();
  auto isAudio = task.VideoTracks.empty();
  for (const auto& track : task.VideoTracks) {
    media->AddVideoTrack(track.Fcc, track.Width, track.Height, track.Fps, track.Language, track.Description);
  }
  for (const auto& track : task.AudioTracks) {
    media->AddAudioTrack(track.Fcc, track.Bitrate, track.Samplerate, track.NumChannels, track.Language, track.Description);
  }
  t->Commit();
  if (isAudio == true) {
    if (ParseAudioFile(task) == false)
      return parser::Task::Status::Fatal;
  } else {
    if (ParseVideoFile(task) == false)
      return parser::Task::Status::Fatal;
  }
  auto duration = task.Duration;
  media->SetDuration(duration);
  if (media->Save() == false)
    return parser::Task::Status::Error;
  return parser::Task::Status::Success;
}

/* Video files */

bool mxp::MetadataService::ParseVideoFile(mxp::parser::Task& task) const {
  auto media = task.Media.get();
  media->SetType(IMedia::Type::VideoType);
  if (task.Title.length() == 0)
    return true;

  if (task.ShowName.length() == 0) {
    auto show = m_ml->GetShow(task.ShowName);
    if (show == nullptr) {
      show = m_ml->CreateShow(task.ShowName);
      if (show == nullptr)
        return false;
    }

    if (task.Episode != 0) {
      std::shared_ptr<Show> s = std::static_pointer_cast<Show>(show);
      s->addEpisode(*media, task.Title, task.Episode);
    }
  } else {
    // How do we know if it's a movie or a random video?
  }
  return true;
}

/* Audio files */

bool mxp::MetadataService::ParseAudioFile(mxp::parser::Task& task) const {
  task.Media->SetType(IMedia::Type::AudioType);

  if(task.artworkMrl.empty() == false) {
    task.Media->SetThumbnail(task.artworkMrl);
  }
  auto artists = HandleArtists(task);
  auto album = HandleAlbum(task, artists.first, artists.second);
  if (album == nullptr) {
    LOG_WARN("Failed to get/create associated album");
    return false;
  }
  auto t = m_ml->GetConnection()->NewTransaction();
  auto res = Link(*task.Media, album, artists.first, artists.second);
  t->Commit();
  return res;
}

/* Album handling */
std::shared_ptr<mxp::Album> mxp::MetadataService::FindAlbum(parser::Task& task, Artist* albumArtist) const {
  static const auto req = "SELECT * FROM " + policy::AlbumTable::Name + " WHERE title = ?";

  auto albums = Album::FetchAll<Album>(m_ml, req, task.AlbumName);
  if(albums.size() == 0) {
    return nullptr;
  }

  // Even if we get only 1 album, we need to filter out invalid matches.
  // For instance, if we have already inserted an album "A" by an artist "john"
  // but we are now trying to Handle an album "A" by an artist "doe", not filtering
  // candidates would yield the only "A" album we know, while we should return
  // nullptr, so HandleAlbum can create a new one.
  for (auto it = begin(albums); it != end(albums);) {
    auto a = (*it).get();
    if (albumArtist != nullptr) {
      // We assume that an album without album artist is a positive match.
      // At the end of the day, without proper tags, there's only so much we can do.
      auto candidateAlbumArtist = a->AlbumArtist();
      if (candidateAlbumArtist != nullptr && candidateAlbumArtist->Id() != albumArtist->Id()) {
        it = albums.erase(it);
        continue;
      }
    }

    // If this is a multidisc album, assume it could be in a multiple amount of folders.
    // Since folders can come in any order, we can't assume the first album will be the
    // first media we see. If the discTotal or discNumber meta are provided, that's easy. If not,
    // we assume that another CD with the same name & artists, and a disc number > 1
    // denotes a multi disc album
    // Check the first case early to avoid fetching tracks if unrequired.
    if (task.discTotal > 1 || task.discNumber > 1) {
      ++it;
      continue;
    }

    const auto tracks = a->CachedTracks();
    assert(tracks.size() > 0);

    auto multiDisc = false;
    for (auto& t : tracks) {
      auto at = t->AlbumTrack();
      assert(at != nullptr);
      if (at != nullptr && at->DiscNumber() > 1) {
        multiDisc = true;
        break;
      }
    }
    if (multiDisc) {
      ++it;
      continue;
    }

    // Assume album files will be in the same folder.
    auto newFileFolder = utils::file::directory(task.MediaFile->mrl());
    auto trackFiles = tracks[0]->Files();
    auto excluded = false;
    for (auto& f : trackFiles) {
      auto candidateFolder = utils::file::directory(f->mrl());
      if (candidateFolder != newFileFolder) {
        excluded = true;
        break;
      }
    }

    if (excluded == true) {
      it = albums.erase(it);
      continue;
    }

    ++it;
  }

  if(albums.size() == 0) {
    return nullptr;
  }
  if (albums.size() > 1) {
    LOG_WARN("Multiple candidates for album ", task.AlbumName, ". Selecting first one out of luck");
  }

  return std::static_pointer_cast<Album>(albums[0]);
}

std::shared_ptr<mxp::Album> mxp::MetadataService::HandleAlbum(parser::Task& task, std::shared_ptr<Artist> albumArtist, std::shared_ptr<Artist> trackArtist) const {
  auto artist = albumArtist;
  if (artist == nullptr) {
    if (trackArtist != nullptr)
      artist = trackArtist;
    else {
      artist = m_unknownArtist;
    }
  }

  std::shared_ptr<Album> album;
  if (task.AlbumName.length() > 0) {
    // Album matching depends on the difference between artist & album artist.
    // Specificaly pass the albumArtist here.
    album = FindAlbum(task, albumArtist.get());

    if (album == nullptr) {
      album = m_ml->CreateAlbum(task.AlbumName);
      if (album != nullptr) {
        if(task.artworkMrl.length() != 0) {
          album->setArtworkMrl(task.artworkMrl);
        }
      }
      m_notifier->NotifyAlbumCreation(album);
    }
  } else {
    album = artist->unknownAlbum();
  }

  if(album == nullptr) {
    return nullptr;
  }

  // If we know a track artist, specify it, otherwise, fallback to the album/unknown artist
  auto track = HandleTrack(album, task, trackArtist ? trackArtist : artist);
  return album;
}

/**
 *  @briefmxp::MetadataService::handleArtists Returns Artist's involved on a track
 *  @param media The track to analyze
 *  @param vlcMedia VLC's media
 *  @return A pair containing:
 *  The album artist as a first element
 *  The track artist as a second element, or nullptr if it is the same as album artist
 */
std::pair<std::shared_ptr<mxp::Artist>, std::shared_ptr<mxp::Artist>> mxp::MetadataService::HandleArtists(parser::Task& task) const {
  std::shared_ptr<Artist> albumArtist;
  std::shared_ptr<Artist> artist;

  if (task.AlbumArtist.empty() == true && task.Artist.empty() == true) {
    return{nullptr, nullptr};
  }

  if (task.AlbumArtist.empty() == false) {
    albumArtist = std::static_pointer_cast<Artist>(m_ml->GetArtist(task.AlbumArtist));
    if (albumArtist == nullptr) {
      albumArtist = m_ml->CreateArtist(task.AlbumArtist);
      if (albumArtist == nullptr) {
        LOG_ERROR("Failed to create new artist ", task.AlbumArtist);
        return{nullptr, nullptr};
      }
      m_notifier->NotifyArtistCreation(albumArtist);
    }
  }

  if (task.Artist.empty() == false && task.Artist != task.AlbumArtist) {
    artist = std::static_pointer_cast<Artist>(m_ml->GetArtist(task.Artist));
    if (artist == nullptr) {
      artist = m_ml->CreateArtist(task.Artist);
      if (artist == nullptr) {
        LOG_ERROR("Failed to create new artist ", task.Artist);
        return{nullptr, nullptr};
      }
      m_notifier->NotifyArtistCreation(albumArtist);
    }
  }

  return{albumArtist, artist};
}

/* Tracks handling */

std::shared_ptr<mxp::AlbumTrack> mxp::MetadataService::HandleTrack(std::shared_ptr<Album> album, parser::Task& task, std::shared_ptr<Artist> artist) const {
  auto title = task.Title;
  if (title.empty() == true) {
    LOG_WARN("Failed to get track title");
    if (task.TrackNumber != 0) {
      title = "Track #";
      title += std::to_string(task.TrackNumber);
    }
  }
  if (title.empty() == false)
    task.Media->SetTitle(title);

  auto track = std::static_pointer_cast<AlbumTrack>(album->AddTrack(task.Media, task.TrackNumber, task.discNumber));
  if (track == nullptr) {
    LOG_ERROR("Failed to create album track");
    return nullptr;
  }
  if(artist != nullptr) {
    track->setArtist(artist);
  }

  if (task.Genre.length() != 0) {
    auto genre = Genre::FindByName(m_ml, task.Genre);
    if (genre == nullptr) {
      genre = Genre::Create(m_ml, task.Genre);
      if (genre == nullptr) {
        LOG_ERROR("Failed to create a genre in database");
        return nullptr;
      }
    }
    track->setGenre(genre);
  }
  if (task.releaseDate.empty() == false) {
    auto releaseYear = atoi(task.releaseDate.c_str());
    task.Media->SetReleaseDate(releaseYear);
    // Let the album Handle multiple dates. In order to do this properly, we need
    // to know if the date has been changed before, which can be known only by
    // using Album class internals.
    album->SetReleaseYear(releaseYear, false);
  }

  m_notifier->NotifyAlbumTrackCreation(track);

  return track;
}

/* Misc */

bool mxp::MetadataService::Link(Media& media, std::shared_ptr<Album> album,
                          std::shared_ptr<Artist> albumArtist, std::shared_ptr<Artist> artist) const {
  if (albumArtist == nullptr && artist == nullptr)
    albumArtist = m_unknownArtist;
  else {
    if (albumArtist == nullptr)
      albumArtist = artist;
  }

  // We might modify albumArtist later, hence Handle thumbnails before.
  // If we have an albumArtist (meaning the track was properly tagged, we
  // can assume this artist is a correct match. We can use the thumbnail from
  // the current album for the albumArtist, if none has been set before.
  if(albumArtist != nullptr && albumArtist->artworkMrl().empty() == true &&
    album != nullptr && album->artworkMrl().empty() == false) {
    albumArtist->setArtworkMrl(album->artworkMrl());
  }

  if(albumArtist != nullptr) {
    albumArtist->addMedia(media);
  }
  if(artist != nullptr && (albumArtist == nullptr || albumArtist->Id() != artist->Id())) {
    artist->addMedia(media);
  }

  auto currentAlbumArtist = album->AlbumArtist();

  // If we have no main artist yet, that's easy, we need to assign one.
  if (currentAlbumArtist == nullptr) {
    // We don't know if the artist was tagged as artist or albumartist, however, we simply add it
    // as the albumartist until proven we were wrong (ie. until one of the next tracks
    // has a different artist)
    album->SetAlbumArtist(albumArtist);
    // Always add the album artist as an artist
    album->AddArtist(albumArtist);
    if(artist != nullptr) {
      album->AddArtist(artist);
    }
  } else {
    if (albumArtist->Id() != currentAlbumArtist->Id()) {
      // We have more than a single artist on this album, fallback to various artists
      auto variousArtists = Artist::Fetch(m_ml, VariousArtistID);
      album->SetAlbumArtist(variousArtists);
      // Add those two artists as "featuring".
      album->AddArtist(albumArtist);
    }
    if (artist != nullptr && artist->Id() != albumArtist->Id()) {
      if(albumArtist->Id() != artist->Id()) {
        album->AddArtist(artist);
      }
    }
  }

  return true;
}
