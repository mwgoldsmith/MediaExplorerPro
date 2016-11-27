/*****************************************************************************
 * Media Library
 *****************************************************************************
 * Copyright (C) 2015 Hugo Beauzée-Luyssen, Videolabs
 *
 * Authors: Hugo Beauzée-Luyssen<hugo@beauzee.fr>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#pragma once

#include "MediaExplorer.h"
#include "parser/ParserService.h"

namespace mxp {

class AlbumTrack;

class MetadataService : public ParserService {
protected:
  virtual bool Initialize() override;
  virtual parser::Task::Status Run(parser::Task& task) override;
  virtual const char* Name() const override;
  virtual uint8_t nbThreads() const override;

  std::shared_ptr<Album> FindAlbum(parser::Task& task, Artist* albumArtist) const;
  bool ParseAudioFile(parser::Task& task) const;
  bool ParseVideoFile(parser::Task& task) const;
  std::pair<std::shared_ptr<Artist>, std::shared_ptr<Artist>> HandleArtists(parser::Task& vlcMedia) const;
  std::shared_ptr<AlbumTrack> HandleTrack(std::shared_ptr<Album> album, parser::Task& task, std::shared_ptr<Artist> artist) const;
  bool Link(Media& media, std::shared_ptr<Album> album, std::shared_ptr<Artist> albumArtist, std::shared_ptr<Artist> artist) const;
  std::shared_ptr<Album> HandleAlbum(parser::Task& task, std::shared_ptr<Artist> albumArtist, std::shared_ptr<Artist> artist) const;

private:
  std::shared_ptr<Artist> m_unknownArtist;
};

}
