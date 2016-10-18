/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_SORTINGCRITERIA_H
#define MXP_SORTINGCRITERIA_H



namespace mxp {

enum class SortingCriteria {
  /*
   * Default depends on the entity type:
   * - By track number (and disc number) for album tracks
   * - Alphabetical order for others
   */
  Default,
  Alpha,
  Duration,
  InsertionDate,
  LastModificationDate,
  ReleaseDate,
};

}

#endif /* MXP_SORTINGCRITERIA_H */
