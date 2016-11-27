/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_SETTINGSKEY_H
#define MXP_SETTINGSKEY_H

namespace mxp {

enum class SettingsKey {
  DbModelVersion,

  /**
  * @brief
  */
  ThumbnailPath,

  /**
  * @brief
  */
  ThumbnailWidth,

  /**
  * @brief
  */
  ThumbnailHeight,

  /**
  * @brief
  */
  ThumbnailTimestamp,

  /**
  * @brief
  */
  ThumbnailPreserveAspectRatio,

  /**
  * @brief
  */
  SmartThumbnailBegin,

  /**
  * @brief
  */
  SmartThumbnailEnd,

  /**
  * @brief
  */
  SmartThumbnailSamples,

  /**
   * @brief
   */
  CreateSmartThumbnails

};

} /* namespace mxp */

#endif /* MXP_SETTINGSKEY_H */
