/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include <stdio.h>
#include "mxp.h"

std::mutex m;
std::condition_variable cv;
bool processed = false;

class MxpCallbacks : public mxp::IMediaExplorerCb {

public:
  virtual void onMediaAdded(std::vector<mxp::MediaPtr> media) override;
  virtual void onMediaUpdated(std::vector<mxp::MediaPtr> media) override;
  virtual void onMediaDeleted(std::vector<int64_t> ids) override;
  virtual void OnParsingStatsUpdated(size_t done, size_t todo) override;
  virtual void onDiscoveryStarted(const std::string& entryPoint) override;
  virtual void onDiscoveryProgress(const std::string& entryPoint) override;
  virtual void onDiscoveryCompleted(const std::string& entryPoint) override;
  virtual void onArtistsAdded(std::vector<mxp::ArtistPtr> artists) override;
  virtual void onArtistsModified(std::vector<mxp::ArtistPtr> artist) override;
  virtual void onArtistsDeleted(std::vector<int64_t> ids) override;
  virtual void onAlbumsAdded(std::vector<mxp::AlbumPtr> albums) override;
  virtual void onAlbumsModified(std::vector<mxp::AlbumPtr> albums) override;
  virtual void onAlbumsDeleted(std::vector<int64_t> ids) override;
  virtual void onTracksAdded(std::vector<mxp::AlbumTrackPtr> tracks) override;
  virtual void onTracksDeleted(std::vector<int64_t> trackIds) override;
};

void MxpCallbacks::onMediaAdded(std::vector<mxp::MediaPtr> media) {}

void MxpCallbacks::onMediaUpdated(std::vector<mxp::MediaPtr> media) {}

void MxpCallbacks::onMediaDeleted(std::vector<int64_t> ids) {}

void MxpCallbacks::OnParsingStatsUpdated(size_t done, size_t todo) {

  printf("Status: %u of %u\n", done, todo);
  if (done == todo) {
    processed = true;
    cv.notify_one();
  }
}

void MxpCallbacks::onDiscoveryStarted(const std::string& entryPoint) {}

void MxpCallbacks::onDiscoveryProgress(const std::string& entryPoint) {}

void MxpCallbacks::onDiscoveryCompleted(const std::string& entryPoint) {
  auto e = entryPoint.c_str();
  printf(e);
}

void MxpCallbacks::onArtistsAdded(std::vector<mxp::ArtistPtr> artists) {}

void MxpCallbacks::onArtistsModified(std::vector<mxp::ArtistPtr> artist) {}

void MxpCallbacks::onArtistsDeleted(std::vector<int64_t> ids) {}

void MxpCallbacks::onAlbumsAdded(std::vector<mxp::AlbumPtr> albums) {}

void MxpCallbacks::onAlbumsModified(std::vector<mxp::AlbumPtr> albums) {}

void MxpCallbacks::onAlbumsDeleted(std::vector<int64_t> ids) {}

void MxpCallbacks::onTracksAdded(std::vector<mxp::AlbumTrackPtr> tracks) {}

void MxpCallbacks::onTracksDeleted(std::vector<int64_t> trackIds) {}

int main (int argc, char *argv[]){
  MxpCallbacks cb;

  printf("Creating new media explorer\n");
  auto mxp = mxp_newInstance();

  printf("Initializing\n");
  mxp->Initialize("mxp.db", &cb);

  printf("Discovering\n");
  mxp->Discover("C:\\Users\\mgoldsmith\\Videos\\");

  {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [] {return processed; });
  }
  printf("Shutting down\n");
  mxp->Shutdown();

  return 0;
}