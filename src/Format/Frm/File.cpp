/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2012-2018 Falltergeist Developers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// C++ standard includes
#include <algorithm>

// Falltergeist includes
#include "../Enums.h"
#include "../Frm/File.h"
#include "../Frm/Frame.h"
#include "../Pal/File.h"
#include "../Pal/Color.h"

// Third party includes

namespace Falltergeist
{
    namespace Format
    {
        namespace Frm
        {
            File::File(ttvfs::CountedPtr<ttvfs::File> file) : BaseFormatFile(file)
            {
                _file->open();
                _file->seek(0, SEEK_SET);

                *this >> _version >> _framesPerSecond >> _actionFrame >> _framesPerDirection;

                uint16_t shiftX[6];
                uint16_t shiftY[6];
                uint32_t dataOffset[6];
                for (unsigned int i = 0; i != 6; ++i) {
                    *this >> shiftX[i];
                }
                for (unsigned int i = 0; i != 6; ++i) {
                    *this >> shiftY[i];
                }
                for (unsigned int i = 0; i != 6; ++i) {
                    *this >> dataOffset[i];
                    if (i > 0 && dataOffset[i-1] == dataOffset[i]) {
                        continue;
                    }

                    _directions.emplace_back();
                    auto& direction = _directions.back();
                    direction.setDataOffset(dataOffset[i]);
                    direction.setShiftX(shiftX[i]);
                    direction.setShiftY(shiftY[i]);
                }

                // for each direction
                for (auto& direction : _directions) {
                    // jump to frames data at frames area
                    _file->seek(direction.dataOffset() + 62, SEEK_SET);

                    // read all frames
                    for (unsigned i = 0; i != _framesPerDirection; ++i) {
                        uint16_t width, height;
                        *this >> width >> height;

                        direction.frames().emplace_back(width, height);
                        auto& frame = direction.frames().back();

                        // Number of pixels for this frame
                        // We don't need this, because we already have width*height
                        _file->seek(_file->getpos() + 4, SEEK_SET);

                        int16_t offsetX, offsetY;
                        *this >> offsetX >> offsetY;
                        frame.setOffsetX(offsetX);
                        frame.setOffsetY(offsetY);

                        // Pixels data
                        _file->read(frame.data(), frame.width() * frame.height());
                    }
                }
            }

            uint32_t File::version() const
            {
                return _version;
            }

            uint16_t File::framesPerSecond() const
            {
                return _framesPerSecond;
            }

            uint16_t File::framesPerDirection() const
            {
                return _framesPerDirection;
            }

            uint16_t File::actionFrame() const
            {
                return _actionFrame;
            }

            const std::vector<Direction>& File::directions() const
            {
                return _directions;
            }

            uint16_t File::width() const
            {
                return std::max_element(_directions.begin(), _directions.end(), [](const Direction& a, const Direction& b) {
                    return a.width() < b.width();
                })->width();
            }

            uint16_t File::height() const
            {
                uint16_t height = 0;
                for (auto& direction : _directions) {
                    height += direction.height();
                }
                return height;
            }

            uint32_t* File::rgba(std::shared_ptr<Pal::File> palFile)
            {
                // TODO: this looks like a getter, which in fact creates _rgba.
                // Moreover, the content of _rgba depends on the specific palFile that was provided the first time
                // This is clearly bad semantics
                if (!_rgba.empty()) {
                    return _rgba.data();
                }

                _rgba.resize(width()*height());

                uint16_t w = width();

                size_t positionY = 1;
                for (auto& direction : _directions) {
                    size_t positionX = 1;
                    for (auto& frame : direction.frames()) {
                        // TODO: more efficient way to generate texture?
                        for (uint16_t y = 0; y != frame.height(); ++y) {
                            for (uint16_t x = 0; x != frame.width(); ++x) {
                                _rgba[((y + positionY)*w) + x + positionX] = *palFile->color(frame.index(x, y));
                            }
                        }
                        positionX += frame.width() + 2;
                    }
                    positionY += direction.height();
                }
                return _rgba.data();
            }

            std::vector<bool>& File::mask(std::shared_ptr<Pal::File> palFile)
            {
                if (!_mask.empty()) {
                    return _mask;
                }

                uint16_t w = width();
                uint16_t h = height();

                _mask.resize(w*h, true);

                unsigned positionY = 1;
                for (auto& direction : _directions) {
                    unsigned positionX = 1;
                    for (auto& frame : direction.frames()) {
                        // TODO: optimize
                        for (unsigned y = 0; y != frame.height(); ++y) {
                            for (unsigned x = 0; x != frame.width(); ++x) {
                                _mask[((y + positionY)*w) + x + positionX] = (palFile->color(frame.index(x, y))->alpha() > 0);
                            }
                        }
                        positionX += frame.width() + 2;
                    }
                    positionY += direction.height();
                }
                return _mask;
            }

            int16_t File::offsetX(unsigned int direction, unsigned int frame) const
            {
                if (direction >= _directions.size()) {
                    direction = 0;
                }
                return _directions.at(direction).frames().at(frame).offsetX();
            }

            int16_t File::offsetY(unsigned int direction, unsigned int frame) const
            {
                if (direction >= _directions.size()) {
                    direction = 0;
                }
                return _directions.at(direction).frames().at(frame).offsetY();
            }
        }
    }
}
