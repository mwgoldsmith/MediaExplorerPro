/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_IPLAYLIST_H
#define MXP_IPLAYLIST_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IPlaylist {
public:
  virtual ~IPlaylist() = default;

  /**
   * @return Identifier of the record for the playlist
   */
  virtual int64_t Id() const = 0;

  /**
   * @return Name of the playlist
   */
  virtual const mstring& GetName() const = 0;

  /**
   * @brief Sets the name of the playlist
   *
   * @param name Name of the playlist
   */
  virtual bool SetName(const mstring& name) = 0;

  /**
   * @brief Gets the creation date of the playlist
   */
  virtual time_t GetCreationDate() const = 0;

  /**
   * @brief Gets the media in the playlist
   *
   * @return A list of pointers to the Media objects in the playlist
   */
  virtual std::vector<MediaPtr> GetMedia() const = 0;

  /**
   *  @brief Appends a media to a playlist
   *
   *  @param media The media to add
   *
   *  @return true on success, false on failure.
   *
   *  The media will be the last element of a subsequent call to media()
   *  This is equivalent to calling add( media, 0 )
   */
  virtual bool Append(int64_t mediaId) = 0;

  /**
   *  @brief Add a media to the playlist at the given position.
   *
   *  @param media    The media to add
   *  @param position The position of this new media. Valid positions start
   *                  at 1. 0 means appending.
   *
   *  @return true on success, false on failure
   */
  virtual bool Add(int64_t mediaId, unsigned int position) = 0;

  /**
   *  @brief Change the position of a media
   *  @param mediaId The media to move reorder
   *  @param position The new position within the playlist.
   *  0 is an invalid value when moving.
   *  In case there is already a media at the given position, it will be placed after
   *  the media being moved. This will cascade to any media placed afterward. For
   *  instance, a playlist with <media,position> like [<1,1>, <2,2>, <3,3>] on which
   *  move(1, 2) is called will result in the playlist  being changed to
   *  [<1,2>, <2,3>, <3,4>]
   *  @return true on success, false on failure
   */
  virtual bool Move(int64_t mediaId, unsigned int position) = 0;

  /**
   *  @brief Removes a media from the playlist
   *  @param mediaId The media to remove.
   *  @return true on success, false on failure
   */
  virtual bool Remove(int64_t mediaId) = 0;
};

} /* namespace mxp */

#endif /* MXP_IPLAYLIST_H */
