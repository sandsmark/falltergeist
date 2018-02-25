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
        namespace Frm { class File; }
        namespace Int { class File; }
        namespace Pro { class File; }
        namespace Txt
        {
            class MapsFile;
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

            std::shared_ptr<Format::Frm::File> frmFileType(unsigned int FID);
            std::shared_ptr<Format::Int::File> intFileType(unsigned int SID);
            std::shared_ptr<Format::Pro::File> proFileType(unsigned int PID);

            Graphics::Texture* texture(const std::string& filename);
            Graphics::Font* font(const std::string& filename = "font1.aaf");
            Graphics::Shader* shader(const std::string& filename);
            void unloadResources();
            std::string FIDtoFrmName(unsigned int FID);
            void shutdown();

            template <class T>
            static std::shared_ptr<T> get(const std::string& filename);

        protected:
            friend class Base::Singleton<ResourceManager>;

            static std::unordered_map<std::string, std::shared_ptr<Format::BaseFormatFile>> _cachedFiles;
            std::unordered_map<std::string, std::unique_ptr<Graphics::Texture>> _textures;
            std::unordered_map<std::string, std::unique_ptr<Graphics::Font>> _fonts;
            std::unordered_map<std::string, std::unique_ptr<Graphics::Shader>> _shaders;

            ResourceManager();
            ResourceManager(const ResourceManager&) = delete;
            ResourceManager& operator=(const ResourceManager&) = delete;
    };
}

#endif // FALLTERGEIST_RESOURCEMANAGER_H
