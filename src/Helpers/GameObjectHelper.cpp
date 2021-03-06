// C++ standard includes
#include <memory>

// Falltergeist includes
#include "../Exception.h"
#include "../Format/Map/Object.h"
#include "../Format/Map/Script.h"
#include "../Format/Msg/File.h"
#include "../Game/ContainerItemObject.h"
#include "../Game/CritterObject.h"
#include "../Game/DoorSceneryObject.h"
#include "../Game/ExitMiscObject.h"
#include "../Game/ObjectFactory.h"
#include "../Game/SpatialObject.h"
#include "../Helpers/GameObjectHelper.h"
#include "../Logger.h"
#include "../ResourceManager.h"
#include "../VM/Script.h"

namespace Falltergeist
{
    namespace Helpers
    {
        std::shared_ptr<Game::Object> GameObjectHelper::createFromMapSpatialScript(const Format::Map::Script& mapScript) const
        {
            auto tile = mapScript.spatialTile();
            auto hex = tile & 0xFFFF;
            auto elev = ((tile >> 28) & 0xf) >> 1;

            auto object = std::make_shared<Game::SpatialObject>(mapScript.spatialRadius());
            object->setElevation(elev);
            object->setPosition(hex);

            auto intFile = ResourceManager::getInstance()->intFileType(mapScript.scriptId());
            if (intFile) {
                object->setScript(new VM::Script(intFile, object));
            }
            return object;
        }

        std::shared_ptr<Game::Object> GameObjectHelper::createFromMapObject(const std::unique_ptr<Format::Map::Object> &mapObject) const
        {
            std::shared_ptr<Game::Object> object = Game::ObjectFactory::getInstance()->createObject(mapObject->PID());
            if (!object) {
                Logger::error() << "Location::setLocation() - can't create object with PID: " << mapObject->PID() << std::endl;
                return nullptr;
            }

            object->setFID(mapObject->FID());
            object->setElevation(mapObject->elevation());
            object->setOrientation(mapObject->orientation());
            object->setPosition(mapObject->hexPosition());
            object->setLightRadius(mapObject->lightRadius());
            object->setLightIntensity(mapObject->lightIntensity());
            object->setFlags(mapObject->flags());
            object->setDefaultFrame(mapObject->frameNumber());

            if (auto exitGrid = std::dynamic_pointer_cast<Game::ExitMiscObject>(object)) {
                exitGrid->setExitMapNumber(mapObject->exitMap());
                exitGrid->setExitElevationNumber(mapObject->exitElevation());
                exitGrid->setExitHexagonNumber(mapObject->exitPosition());
                exitGrid->setExitDirection(mapObject->exitOrientation());
            }

            if (auto door = std::dynamic_pointer_cast<Game::DoorSceneryObject>(object)) {
                door->setOpened(mapObject->opened());
            }

            if (auto container = std::dynamic_pointer_cast<Game::ContainerItemObject>(object)) {
                for (auto &child : mapObject->children()) {
                    auto item = std::dynamic_pointer_cast<Game::ItemObject>(Game::ObjectFactory::getInstance()->createObject(child->PID()));
                    if (!item) {
                        Logger::error() << "Location::setLocation() - can't create object with PID: " << child->PID() << std::endl;
                        return nullptr;
                    }
                    item->setAmount(child->ammount());
                    container->inventory()->push_back(item);
                }
            }

            // TODO: use common interface...
            if (auto critter = std::dynamic_pointer_cast<Game::CritterObject>(object)) {
                for (auto &child : mapObject->children()) {
                    auto item = std::dynamic_pointer_cast<Game::ItemObject>(Game::ObjectFactory::getInstance()->createObject(
                            child->PID()));
                    if (!item) {
                        Logger::error() << "Location::setLocation() - can't create object with PID: "
                                        << child->PID() << std::endl;
                        return nullptr;
                    }
                    item->setAmount(child->ammount());
                    critter->inventory()->push_back(item);
                }
            }

            if (mapObject->scriptId() > 0) {
                auto intFile = ResourceManager::getInstance()->intFileType(mapObject->scriptId());
                if (intFile) {
                    object->setScript(new VM::Script(intFile, object));
                    object->setSID(mapObject->scriptId());
                }
            }
            if (mapObject->mapScriptId() > 0 && mapObject->mapScriptId() != mapObject->scriptId()) {
                auto intFile = ResourceManager::getInstance()->intFileType(mapObject->mapScriptId());
                if (intFile) {
                    object->setScript(new VM::Script(intFile, object));
                    object->setSID(mapObject->mapScriptId());
                }
            }

            if (object->SID() > 0) {
                auto msg = ResourceManager::getInstance()->msgFileType("text/english/game/scrname.msg");
                try {
                    object->setScrName(msg->message(object->SID() + 101)->text());
                }
                catch (const Exception &) {
                }
            }
            return object;
        }
    }
}
