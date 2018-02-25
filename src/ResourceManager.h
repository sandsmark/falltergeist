/*
 * Copyright 2012-2018 Falltergeist Developers.
 *
 * This file is part of Falltergeist.
 *
 * Falltergeist is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Falltergeist is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FALLTERGEIST_RESOURCEMANAGER_H
#define FALLTERGEIST_RESOURCEMANAGER_H

// C++ standard includes
#include <fstream>
#include <functional>
#include <string>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

// Falltergeist includes
#include "Base/Singleton.h"

// Third party includes
#include <ttvfs.h>
#include <ttvfs_dat2.h>

namespace Falltergeist
{
    namespace Format
    {
        namespace Dat
        {
            class File;
            class Item;
            class MiscFile;
            class Stream;
        }
        namespace Frm { class File; }
        namespace Pal { class File; }
        namespace Lst { class File; }
        namespace Pro { class File; }
        namespace Txt
        {
            class CityFile;
            class MapsFile;
            class WorldmapFile;

            template <typename T>
            class CSVBasedFile;

            struct EndDeath;
            struct EndGame;
            struct GenRep;
            struct Holodisk;
            struct KarmaVar;
            struct Quest;

            typedef CSVBasedFile<EndDeath> EndDeathFile;
            typedef CSVBasedFile<EndGame> EndGameFile;
            typedef CSVBasedFile<GenRep> GenRepFile;
            typedef CSVBasedFile<Holodisk> HolodiskFile;
            typedef CSVBasedFile<KarmaVar> KarmaVarFile;
            typedef CSVBasedFile<Quest> QuestsFile;
        }
        class BaseFormatFile;
    }
}

namespace Falltergeist
{
    namespace Game
    {
        class Location;
    }
    namespace Graphics
    {
        class Texture;
        class Font;
        class Shader;
    }

    class ResourceManager
    {
        public:
            static ResourceManager* getInstance();

            static std::shared_ptr<Format::BaseFormatFile> get(const std::string& filename);

            Format::Dat::Item* datFileItem(const std::string& filename);
            Format::Frm::File* frmFileType(const std::string& filename);
            Format::Frm::File* frmFileType(unsigned int FID);
            Format::Pal::File* palFileType(const std::string& filename);
            std::shared_ptr<Format::BaseFormatFile> intFileType(unsigned int SID);
            Format::Lst::File* lstFileType(const std::string& filename);
            Format::Pro::File* proFileType(const std::string& filename);
            Format::Pro::File* proFileType(unsigned int PID);

            Format::Txt::CityFile* cityTxt();
            Format::Txt::MapsFile* mapsTxt();
            Format::Txt::WorldmapFile* worldmapTxt();
            Format::Txt::EndDeathFile* endDeathTxt();
            Format::Txt::EndGameFile* endGameTxt();
            Format::Txt::GenRepFile* genRepTxt();
            Format::Txt::HolodiskFile* holodiskTxt();
            Format::Txt::KarmaVarFile* karmaVarTxt();
            Format::Txt::QuestsFile* questsTxt();

            Graphics::Texture* texture(const std::string& filename);
            Graphics::Font* font(const std::string& filename = "font1.aaf");
            Graphics::Shader* shader(const std::string& filename);
            void unloadResources();
            std::string FIDtoFrmName(unsigned int FID);
            Game::Location* gameLocation(unsigned int number);
            void shutdown();

        protected:
            friend class Base::Singleton<ResourceManager>;

            std::vector<std::unique_ptr<Format::Dat::File>> _datFiles;
            std::unordered_map<std::string, std::unique_ptr<Format::Dat::Item>> _datItems;
            std::unordered_map<std::string, std::unique_ptr<Graphics::Texture>> _textures;
            std::unordered_map<std::string, std::unique_ptr<Graphics::Font>> _fonts;
            std::unordered_map<std::string, std::unique_ptr<Graphics::Shader>> _shaders;

            ResourceManager();
            ResourceManager(const ResourceManager&) = delete;
            ResourceManager& operator=(const ResourceManager&) = delete;

            // Retrieves given file item from "virtual file system".
            // All items are cached after being requested for the first time.
            template <class T>
            T* _datFileItem(std::string filename);

            // Searches for a given file within virtual "file system" and calls the given callback with Dat::Stream created from that file.
            void _loadStreamForFile(std::string filename, std::function<void(Format::Dat::Stream&&)> callback);
    };
}

#endif // FALLTERGEIST_RESOURCEMANAGER_H
