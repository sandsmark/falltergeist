#pragma once

#include <string>
#include "../Graphics/Sprite.h"
#include "../UI/Base.h"

namespace Falltergeist
{
    namespace Graphics
    {
        class Sprite;
    }
    namespace UI
    {
        class Image final : public Base
        {
            public:
                Image(std::unique_ptr<Graphics::Sprite> sprite);
                virtual ~Image() = default;

                virtual void render(bool eggTransparency = false) override;

                virtual void render(const Size &size, bool eggTransparency = false) override;

                void renderCropped(const Point offset, const Size size);

                bool opaque(unsigned int x, unsigned int y);
                virtual bool opaque(const Point &position) override;

                virtual Size size() const override;

            private:
                std::unique_ptr<Graphics::Sprite> sprite;
        };
    }
}
