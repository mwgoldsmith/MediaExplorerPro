#ifndef LIBRARYWORKFLOW_H
#define LIBRARYWORKFLOW_H

#include "Media/media.h"
#include "Media/mediafolder.h"
#include <QObject>

#include "mediaexplorer/IMediaExplorerCb.h"

class MediaListModel;

/**
 * SIGNALS:
 * void LibraryFoldersDock::contentsChanged(const MediaFolderList& media)
 * void LibraryFoldersDock::selectionChanged(const MediaFolderList& folders)
 * void LibraryWorkflow::foldersLoaded(const MediaFolderList& folders)
 * void LibraryWorkflow::mediaLoaded(const MediaList& media)
 * void LibraryPanel::contentsChanged(const MediaList& media)
 * void LibraryPanel::selectionChanged(const MediaList& media)
 * SLOTS:
 * void LibraryFoldersDock::itemSelectionChanged()
 * void LibraryFoldersDock::setContents(const MediaFolderList& folders)
 * void LibraryWorkflow::loadFolders()
 * void LibraryWorkflow::selectDefaultFolder(const MediaFolderList& folders)
 * void LibraryWorkflow::loadMedia(const MediaFolderList& folders)
 * void LibraryPanel::setContents(const MediaList& media)
 * void MediaPropertiesDock::setMedia(const MediaList& media)
 * void MediaViewDock::setMedia(const MediaList& media)
 */
 /**
  * {INITIALIZE}
  *   [LibraryWorkflow::loadFolders]
  *   create MediaFolderList as results
  *   get all LibraryFolder objects
  *   for each LibraryFolder
  *     if it has no parent
  *       get all child LibraryFolder objects recursively
  *       create a MediaFolder
  *       add to results
  *   emit foldersLoaded(const MediaFolderList& folders)
  *    -> LibraryFoldersDock::setContents
  *
  *   [LibraryFoldersDock::setContents]
  *   reset current contents
  *   for each MediaFolder to set
  *     create a QTreeItem
  *     set the text of the item to the MediaFolder name
  *     set the data of the item to the MediaFolder
  *     if the MediaFolder has children
  *       add each to the item recursively
  *     add the item to the tree
  *   emit contentsChanged(const MediaFolderList& media)
  *    -> LibraryWorkflow::selectDefaultFolder
  */
/**
* {SELECT LIBRARY FOLDERS}
*   [LibraryFoldersDock::itemSelectionChanged]
*   create MediaFolderList as results
*   get selected QTreeItems
*   for each selected item
*     get MediaFolder from item data
*     append to results
*   emit selectionChanged(const MediaFolderList& folders)
*    -> LibraryWorkflow::loadMedia
*
*   [LibraryWorkflow::loadMedia]
*   create MediaList as results
*   for each MediaFolder
*     get all Media objects in the folder
*     for each Media object
*       if it doesn't exist in results
*         append to results
*   emit mediaLoaded(const MediaList& media)
*    -> LibraryPanel::setContents
*
*   [LibraryPanel::setContents]
*   create MediaList of current panel selection
*   create selection changed flag initialized to false
*   for each Media in the panel contents
*     if it does not exist in the media to set
*       if it exists in current selection
*         set selection changed flag to true
*       remove from contents
*   for each Media in media to set
*     if it does not exist in contents
*       add to contents
*   emit contentsChanged(const MediaList& media)
*   emit selectionChanged(const MediaList& media)
*    -> MediaPropertiesDock::setMedia
*    -> MediaViewDock::setMedia
*/
class LibraryWorkflow : public QObject, public mxp::IMediaExplorerCb {
  Q_OBJECT

public:
  explicit LibraryWorkflow(mxp::IMediaExplorer& ml, QObject *parent = 0);

  MediaListModel* getModel() const;

private:
  void setMedia(const MediaFolderList& folders);

  MediaFolderList getFolderChildren(const mxp::LibraryFolderPtr folder, const std::vector<mxp::LibraryFolderPtr>& folders) const;

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

signals:
  void foldersLoaded(const MediaFolderList& folders);
  void mediaLoaded(const MediaList& media);

public slots:
  void loadFolders();
  void loadMedia(const MediaFolderList& folders);
  void selectDefaultFolder(const MediaFolderList& folders);

  void initialize();

private:
  mxp::IMediaExplorer& m_ml;
  MediaListModel*      m_model;

};

#endif // LIBRARYWORKFLOW_H
