#pragma once

#include "../State/State.h"
#include "../UI/IResourceManager.h"

namespace Falltergeist
{
    namespace Format {
        namespace Txt {
            class WorldmapFile;
        }
    }
    namespace UI
    {
        namespace Factory
        {
            class ImageButtonFactory;
        }
        class Image;
        class ImageButton;
        class ImageList;
    }
    namespace State
    {
        class WorldMap final : public State
        {
            public:

                WorldMap(std::shared_ptr<UI::IResourceManager> resourceManager);

                // Need out of line to get unique_ptr to work with gcc's STL
                virtual ~WorldMap();

                void init() override;
                void render() override;
                void handle(Event::Event* event) override;
                void onKeyDown(Event::Keyboard* event) override;
                void onStateActivate(Event::State* event) override;
                void onStateDeactivate(Event::State* event) override;

            private:
                std::shared_ptr<UI::IResourceManager> resourceManager;
                std::unique_ptr<UI::Factory::ImageButtonFactory> imageButtonFactory;
                Format::Txt::WorldmapFile* _worldmapFile = nullptr;

                UI::Image* _panel = nullptr;
                UI::ImageList* _tiles = nullptr;
                UI::ImageButton* _hotspot = nullptr;

                // temporary!
                // @todo: move it to other place!
                // coordinates of the player on world map
                unsigned int worldMapX = 0;
                unsigned int worldMapY = 0;

                // delta (shift of map to fit to screen)
                signed int deltaX = 0;
                signed int deltaY = 0;

                // fallout2 map size: 4x5 tiles, so texture size is 1400x1500
                unsigned int tilesNumberX = 4; // fallout 2 tiles by X
                unsigned int tilesNumberY = 5; // fallout 2 tiles by Y
                unsigned int tileWidth = 350;  // fallout 2 map tile width
                unsigned int tileHeight = 300; // fallout 2 map tile height

                // default size data
                unsigned int mapWidth = 450;   // fallout 2 map screen width
                unsigned int mapHeight = 442;  // fallout 2 map screen height
                unsigned int mapMinX = 0;      // start X point of map screen
                unsigned int mapMinY = 0;      // start Y point of map screen
        };
    }
}
