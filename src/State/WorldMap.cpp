#include "../State/WorldMap.h"
#include "../Game/Game.h"
#include "../Graphics/Renderer.h"
#include "../Input/Mouse.h"
#include "../ResourceManager.h"
#include "../Settings.h"
#include "../State/Location.h"
#include "../State/MainMenu.h"
#include "../UI/Factory/ImageButtonFactory.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/ImageList.h"
#include "../UI/TextArea.h"
#include "../Format/Txt/WorldmapFile.h"

#include <iostream>

namespace Falltergeist
{
    using ImageButtonType = UI::Factory::ImageButtonFactory::Type;

    namespace State
    {
        WorldMap::WorldMap(std::shared_ptr<UI::IResourceManager> resourceManager) : State()
        {
            this->resourceManager = resourceManager;
            imageButtonFactory = std::make_unique<UI::Factory::ImageButtonFactory>(resourceManager);
        }

        void WorldMap::init()
        {
            if (_initialized) return;
            State::init();

            setModal(true);
            setFullscreen(true);

            _worldmapFile = ResourceManager::getInstance()->worldmapTxt();
            tilesNumberX = _worldmapFile->numHorizontalTiles;
            tilesNumberY = _worldmapFile->tiles.size() / _worldmapFile->numHorizontalTiles;

            for (int x=0; x<tilesNumberX; x++) {
                for (int y=0; y<tilesNumberY; y++) {
                    std::cout << _worldmapFile->tiles[x + y * tilesNumberX].artIdx << std::endl;
                    std::cout << ResourceManager::getInstance()->FIDtoFrmName(_worldmapFile->tiles[x + y * tilesNumberX].artIdx) << std::endl;
                }
            }

            unsigned int renderWidth = Game::getInstance()->renderer()->width();
            unsigned int renderHeight = Game::getInstance()->renderer()->height();

            // loading map tiles
            _tiles = new UI::ImageList({0, 0}, {
                resourceManager->getImage("art/intrface/wrldmp00.frm"),
                resourceManager->getImage("art/intrface/wrldmp01.frm"),
                resourceManager->getImage("art/intrface/wrldmp02.frm"),
                resourceManager->getImage("art/intrface/wrldmp03.frm"),
                resourceManager->getImage("art/intrface/wrldmp04.frm"),
                resourceManager->getImage("art/intrface/wrldmp05.frm"),
                resourceManager->getImage("art/intrface/wrldmp06.frm"),
                resourceManager->getImage("art/intrface/wrldmp07.frm"),
                resourceManager->getImage("art/intrface/wrldmp08.frm"),
                resourceManager->getImage("art/intrface/wrldmp09.frm"),
                resourceManager->getImage("art/intrface/wrldmp10.frm"),
                resourceManager->getImage("art/intrface/wrldmp11.frm"),
                resourceManager->getImage("art/intrface/wrldmp12.frm"),
                resourceManager->getImage("art/intrface/wrldmp13.frm"),
                resourceManager->getImage("art/intrface/wrldmp14.frm"),
                resourceManager->getImage("art/intrface/wrldmp15.frm"),
                resourceManager->getImage("art/intrface/wrldmp16.frm"),
                resourceManager->getImage("art/intrface/wrldmp17.frm"),
                resourceManager->getImage("art/intrface/wrldmp18.frm"),
                resourceManager->getImage("art/intrface/wrldmp19.frm")
            });

            //auto cross = new Image("art/intrface/wmaploc.frm");
            _hotspot = imageButtonFactory->getByType(ImageButtonType::MAP_HOTSPOT, {0, 0});
            //addUI(_hotspot);

            // creating screen
            if (Game::getInstance()->settings()->worldMapFullscreen())
            {
                _panel = resourceManager->getImage("art/intrface/wminfce2.frm"); // panel small
                mapWidth = renderWidth - 168;
                mapHeight = renderHeight;
                mapMinX = 0;
                mapMinY = 0;
            }
            else
            {
                _panel = resourceManager->getImage("art/intrface/wmapbox.frm"); // panel full
                mapWidth = 450;   // fallout 2 map screen width
                mapHeight = 442;  // fallout 2 map screen height
                mapMinX = (renderWidth - 640)/2 + 22;
                mapMinY = (renderHeight - 480)/2 + 21;
            }
        }

        void WorldMap::render()
        {
            // calculating render size, screen size, etc
            unsigned int renderWidth = Game::getInstance()->renderer()->width();
            unsigned int renderHeight = Game::getInstance()->renderer()->height();

            // MAP SHOW
            // calculating delta (shift of map to fit to screen)
            deltaX = worldMapX - mapWidth/2;
            deltaY = worldMapY - mapHeight/2;

            unsigned int worldMapSizeX = tilesNumberX*tileWidth;
            unsigned int worldMapSizeY = tilesNumberY*tileHeight;

            // correcting delta
            if (deltaX<0)
            {
                deltaX = 0;
            }
            if (deltaY<0)
            {
                deltaY = 0;
            }
            if (worldMapSizeX-deltaX < mapWidth)
            {
                deltaX = worldMapSizeX - mapWidth;
            }
            if (worldMapSizeY-deltaY < mapHeight)
            {
                deltaY = worldMapSizeY - mapHeight;
            }

            // panel
            unsigned int panelX;
            unsigned int panelY;

            if (Game::getInstance()->settings()->worldMapFullscreen()) {
                panelX = renderWidth - 168; // only panel right
            } else {
                panelX = (renderWidth - _panel->size().width()) / 2;
            }
            panelY = (renderHeight - _panel->size().height()) / 2;

            signed int worldTileMinX; // start X coordinate of current tile on world map
            signed int worldTileMinY; // start Y coordinate of current tile on world map
            // NB: can be unsigned, but it compared with signed deltaX and deltaY, so...

//            std::cout << tilesNumberX << " x " << tilesNumberY << std::endl;
            // copy tiles to screen if needed
            for (unsigned int y=0; y<tilesNumberY; y++)
            {
                for (unsigned int x=0; x<tilesNumberX; x++)
                {
                    _tiles->setCurrentImage(y*tilesNumberX+x);
                    worldTileMinX = x*tileWidth;
                    worldTileMinY = y*tileHeight;

                    const int tileIndex = y*tilesNumberX+x;
                    const int tileX = x*tileWidth-deltaX;
                    const int tileY = y*tileHeight-deltaY;
//                    if (x * tileWidth < 100) {
//                        continue;
//                    }

                    // checking if tile is visible on screenMap
                    // checking current tile borders
                    // either xmin or xmax SHOULD belongs to map area AND
                    // either ymin or ymax SHOULD belongs to map area
                    if ((deltaX > worldTileMinX || worldTileMinX > deltaX+mapWidth) &&
                        (deltaX > worldTileMinX+tileWidth || worldTileMinX+tileWidth > deltaX+mapWidth)) {
                        continue;
                    }

                    if ((deltaY > worldTileMinY || worldTileMinY > deltaY+mapHeight) &&
                        (deltaY > worldTileMinY+tileHeight || worldTileMinY+tileHeight > deltaY+mapHeight))
                    {
                        continue;
                    }

//                    std::cout << "x " << x << " tilex " << tileX << " deltax " << deltaX << std::endl;
//                    std::cout << "y " << y << " tiley " << tileY << " deltay " << deltaY << std::endl;
                    _tiles->images().at(tileIndex)->setPosition(Point(tileX, tileY));
                    _tiles->images().at(tileIndex)->render();
                }
            }

            // hostpot show
            _hotspot->setPosition(Point(mapMinX + worldMapX - deltaX, mapMinY + worldMapY - deltaY));
            _hotspot->render();

            _panel->setPosition(Point(panelX, panelY));
            _panel->render();

        }

        void WorldMap::handle(Event::Event* event)
        {
            auto game = Game::getInstance();

            if (auto mouseEvent = dynamic_cast<Event::Mouse*>(event))
            {
                auto mouse = game->mouse();

                // Left button down
                if (mouseEvent->name() == "mousedown" && mouseEvent->leftButton())
                {
                    // check if point clicked belongs to the screen
                    if ((mapMinX<=(unsigned int)mouse->x()) && ((unsigned int)mouse->x()<=(mapMinX+mapWidth)) &&
                        (mapMinY<=(unsigned int)mouse->y()) && ((unsigned int)mouse->y()<=(mapMinY+mapHeight)))
                    {
                        // change destination point
                        worldMapX = mouse->x()+deltaX-mapMinX;
                        worldMapY = mouse->y()+deltaY-mapMinY;
                    }
                }
            }

            if (auto keyboardEvent = dynamic_cast<Event::Keyboard*>(event))
            {
                if (keyboardEvent->name() == "keydown")
                    onKeyDown(keyboardEvent);
            }
        }

        void WorldMap::onStateActivate(Event::State* event)
        {
            Game::getInstance()->mouse()->pushState(Input::Mouse::Cursor::BIG_ARROW);
        }

        void WorldMap::onStateDeactivate(Event::State* event)
        {
            Game::getInstance()->mouse()->popState();
        }

        void WorldMap::onKeyDown(Event::Keyboard* event)
        {
            switch (event->keyCode())
            {
                case SDLK_ESCAPE:
                {
                    Game::getInstance()->popState();
                }
            }
        }

        Falltergeist::State::WorldMap::~WorldMap() { }
    }
}
