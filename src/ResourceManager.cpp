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

// C++ standard includes
#include <fstream>
#include <sstream>
#include <iomanip>
#include <locale>
#include <memory>
#include <utility>

// Falltergeist includes
#include "CrossPlatform.h"
#include "Exception.h"
#include "Format/Acm/File.h"
#include "Format/Bio/File.h"
#include "Format/Fon/File.h"
#include "Format/Frm/File.h"
#include "Format/Gam/File.h"
#include "Format/Gcd/File.h"
#include "Format/Int/File.h"
#include "Format/Lip/File.h"
#include "Format/Lst/File.h"
#include "Format/Map/File.h"
#include "Format/Msg/File.h"
#include "Format/Mve/File.h"
#include "Format/Pal/File.h"
#include "Format/Pro/File.h"
#include "Format/Rix/File.h"
#include "Format/Sve/File.h"
#include "Format/Txt/CityFile.h"
#include "Format/Txt/CSVBasedFile.h"
#include "Format/Txt/MapsFile.h"
#include "Format/Txt/WorldmapFile.h"
#include "Game/Location.h"
#include "Graphics/Font.h"
#include "Graphics/Font/AAF.h"
#include "Graphics/Font/FON.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Helpers/CritterAnimationHelper.h"
#include "Logger.h"
#include "ResourceManager.h"
#include "Ini/File.h"

// Third party includes
#include <SDL_image.h>

namespace Falltergeist
{
    using namespace std;
    using namespace Format;
    using Helpers::CritterAnimationHelper;

    ResourceManager::ResourceManager()
    {
        // TODO: mount DAT archives to VFS
        // TODO: mount custom paths to VFS
        /*
        for (auto filename : CrossPlatform::findFalloutDataFiles())
        {
            string path = CrossPlatform::findFalloutDataPath() + "/" + filename;
            _datFiles.push_back(std::make_unique<Dat::File>(path));
        }
        */
    }

    // static
    ResourceManager* ResourceManager::getInstance()
    {
        return Base::Singleton<ResourceManager>::get();
    }

    template <class T>
    std::shared_ptr<T> ResourceManager::get(const std::string& filename)
    {
        // TODO seems to be unnecessary since all filenames already should be in lowercase
        //std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

        // Return item from cache
        auto itemIt = _cachedFiles.find(filename);
        if (itemIt != _cachedFiles.end()) {
            auto itemPtr = std::dynamic_pointer_cast<T>(itemIt->second);
            if (!itemPtr) {
                Logger::error("RESOURCE MANAGER") << "Requested file type does not match type in the cache: " << filename << endl;
            }
            return itemPtr;
        }

        /*
         * TODO load file from VFS and create it instance
        T* itemPtr = nullptr;
        _loadStreamForFile(filename, [this, &filename, &itemPtr](Dat::Stream&& stream) {
            auto item = std::make_unique<T>(std::move(stream));
            itemPtr = item.get();
            item->setFilename(filename);
            _datItems.emplace(filename, std::move(item));
        });
         */

        return nullptr;
    }

    Graphics::Texture* ResourceManager::texture(const string& filename)
    {
        if (_textures.count(filename)) {
            return _textures.at(filename).get();
        }

        string ext = filename.substr(filename.length() - 4);

        Graphics::Texture* texture = nullptr;

        if (ext == ".png") {
            // @fixme: this section looks quite ugly. we should try to do something with it someday
            SDL_Surface* tempSurface = IMG_Load(string(CrossPlatform::findFalltergeistDataPath() + "/" +filename).c_str());
            if (tempSurface == NULL) {
                throw Exception("ResourceManager::texture(name) - cannot load texture from file " + filename + ": " + IMG_GetError());
            }

            SDL_PixelFormat* pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
            SDL_Surface* tempSurface2 = SDL_ConvertSurface(tempSurface, pixelFormat, 0);
            texture = new Graphics::Texture(tempSurface2);

            SDL_FreeFormat(pixelFormat);
            SDL_FreeSurface(tempSurface);
            SDL_FreeSurface(tempSurface2);

        } else if (ext == ".rix") {
            auto rix = get<Format::Rix::File>(filename);
            if (!rix) {
                return nullptr;
            }
            texture = new Graphics::Texture(rix->width(), rix->height());
            texture->loadFromRGBA(rix->rgba());
        } else if (ext == ".frm") {
            auto frm = get<Format::Frm::File>(filename);
            if (!frm) {
                return nullptr;
            }
            auto pal = get<Format::Pal::File>("color.pal");
            texture = new Graphics::Texture(frm->width(), frm->height());
            texture->loadFromRGBA(frm->rgba(pal));
            texture->setMask(frm->mask(pal));
        } else {
            throw Exception("ResourceManager::surface() - unknown image type:" + filename);
        }

        _textures.insert(make_pair(filename, unique_ptr<Graphics::Texture>(texture)));
        return texture;
    }

    Graphics::Font* ResourceManager::font(const string& filename)
    {

        if (_fonts.count(filename)) {
            return _fonts.at(filename).get();
        }

        std::string ext = filename.substr(filename.length() - 4);
        Graphics::Font* fontPtr = nullptr;

        if (ext == ".aaf") {
            fontPtr = new Graphics::AAF(filename);
        } else if (ext == ".fon") {
            fontPtr = new Graphics::FON(filename);
        }
        _fonts.emplace(filename, std::unique_ptr<Graphics::Font>(fontPtr));
        return fontPtr;
    }


    Graphics::Shader* ResourceManager::shader(const string& filename)
    {
        if (_shaders.count(filename)) {
            return _shaders.at(filename).get();
        }

        Graphics::Shader* shader = new Graphics::Shader(filename);

        _shaders.emplace(filename, unique_ptr<Graphics::Shader>(shader));
        return shader;
    }


    std::shared_ptr<Format::Pro::File> ResourceManager::proFileType(unsigned int PID)
    {
        unsigned int typeId = PID >> 24;
        string listFile;
        switch ((OBJECT_TYPE)typeId) {
            case OBJECT_TYPE::ITEM:
                listFile += "proto/items/items.lst";
                break;
            case OBJECT_TYPE::CRITTER:
                listFile += "proto/critters/critters.lst";
                break;
            case OBJECT_TYPE::SCENERY:
                listFile += "proto/scenery/scenery.lst";
                break;
            case OBJECT_TYPE::WALL:
                listFile += "proto/walls/walls.lst";
                break;
            case OBJECT_TYPE::TILE:
                listFile += "proto/tiles/tiles.lst";
                break;
            case OBJECT_TYPE::MISC:
                listFile += "proto/misc/misc.lst";
                break;
            default:
                Logger::error() << "ResourceManager::proFileType(unsigned int) - wrong PID: " << PID << endl;
                return nullptr;
        }

        auto lst = get<Format::Lst::File>(listFile);

        unsigned int index = 0x00000FFF & PID;

        if (index > lst->strings()->size()) {
            Logger::error() << "ResourceManager::proFileType(unsigned int) - LST size < PID: " << PID << endl;
            return nullptr;
        }

        string protoName = lst->strings()->at(index-1);

        switch ((OBJECT_TYPE)typeId)
        {
            case OBJECT_TYPE::ITEM:
                return get<Format::Pro::File>("proto/items/" + protoName);
            case OBJECT_TYPE::CRITTER:
                return get<Format::Pro::File>("proto/critters/" + protoName);
            case OBJECT_TYPE::SCENERY:
                return get<Format::Pro::File>("proto/scenery/" + protoName);
            case OBJECT_TYPE::WALL:
                return get<Format::Pro::File>("proto/walls/" + protoName);
            case OBJECT_TYPE::TILE:
                return get<Format::Pro::File>("proto/tiles/" + protoName);
            case OBJECT_TYPE::MISC:
                return get<Format::Pro::File>("proto/misc/" + protoName);
        }
        return nullptr;
    }

    void ResourceManager::unloadResources()
    {
        _cachedFiles.clear();
    }

    std::shared_ptr<Format::Frm::File> ResourceManager::frmFileType(unsigned int FID)
    {
        const auto& frmName = FIDtoFrmName(FID);

        if (frmName.empty()) {
            return nullptr;
        }
        return get<Format::Frm::File>(frmName);
    }

    std::shared_ptr<Format::Int::File> ResourceManager::intFileType(unsigned int SID)
    {
        auto lst = get<Format::Lst::File>("scripts/scripts.lst");
        if (SID >= lst->strings()->size()) {
            throw Exception("ResourceManager::intFileType() - wrong SID: " + std::to_string(SID));
        }

        return get<Format::Int::File>("scripts/" + lst->strings()->at(SID));
    }

    string ResourceManager::FIDtoFrmName(unsigned int FID)
    {
        const auto baseId = FID & 0x00000FFF;
        const auto type = static_cast<FRM_TYPE>(FID >> 24);

        if (type == FRM_TYPE::CRITTER) {
            throw Exception("use CritterAnimationHelpers instead");
        }

        if (type == FRM_TYPE::MISC && baseId == 1) {
            static const std::string SCROLL_BLOCKERS_PATH("art/misc/scrblk.frm");
            // Map scroll blockers
            return SCROLL_BLOCKERS_PATH;
        }

        static struct TypeArtListDecription
        {
            const std::string prefixPath;
            const std::string lstFilePath;
        } const frmTypeDescription[] =
        {
            { "art/items/", "art/items/items.lst" },
            { "art/critters/", "art/critters/critters.lst" },
            { "art/scenery/", "art/scenery/scenery.lst" },
            { "art/walls/", "art/walls/walls.lst" },
            { "art/tiles/", "art/tiles/tiles.lst" },
            { "art/misc/", "art/misc/misc.lst" },
            { "art/intrface/", "art/intrface/intrface.lst" },
            { "art/inven/", "art/inven/inven.lst" },
        };

        if (type > FRM_TYPE::INVENTORY) {
            throw Exception("ResourceManager::FIDtoFrmName - wrong type");
        }

        const auto& typeArtDescription = frmTypeDescription[static_cast<size_t>(type)];
        auto lst = get<Format::Lst::File>(typeArtDescription.lstFilePath);
        if (baseId >= lst->strings()->size()) {
            Logger::error() << "ResourceManager::FIDtoFrmName(unsigned int) - LST size " << lst->strings()->size() << " <= frmID: " << baseId << " frmType: " << (unsigned)type << endl;
            return std::string();
        }

        string frmName = lst->strings()->at(baseId);
        return typeArtDescription.prefixPath + frmName;
    }

    void ResourceManager::shutdown()
    {
        unloadResources();
    }
}
