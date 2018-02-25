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

// Falltergeist includes
#include "../Format/BaseFormatFile.h"

// Third party includes

namespace Falltergeist
{
    namespace Format
    {
        BaseFormatFile::BaseFormatFile(ttvfs::CountedPtr<ttvfs::File> file)
        {
            _file = file;
        }

        BaseFormatFile::~BaseFormatFile()
        {
        }

        BaseFormatFile& BaseFormatFile::operator>>(int32_t &value)
        {
            _file->read(reinterpret_cast<char *>(&value), sizeof(value));
            return *this;
        }

        BaseFormatFile& BaseFormatFile::operator>>(uint32_t &value)
        {
            return *this >> (int32_t&) value;
        }

        BaseFormatFile& BaseFormatFile::operator>>(int16_t &value)
        {
            _file->read(reinterpret_cast<char *>(&value), sizeof(value));
            return *this;
        }

        BaseFormatFile& BaseFormatFile::operator>>(uint16_t &value)
        {
            return *this >> (int16_t&) value;
        }

        BaseFormatFile& BaseFormatFile::operator>>(int8_t &value)
        {
            _file->read(reinterpret_cast<char *>(&value), sizeof(value));
            return *this;
        }

        BaseFormatFile& BaseFormatFile::operator>>(uint8_t &value)
        {
            return *this >> (int8_t&) value;
        }

        uint32_t BaseFormatFile::uint32()
        {
            uint32_t value;
            _file->read(reinterpret_cast<char *>(&value), sizeof(value));
            return value;
        }

        int32_t BaseFormatFile::int32()
        {
            return (int32_t) uint32();
        }

        uint16_t BaseFormatFile::uint16()
        {
            uint16_t value;
            _file->read(reinterpret_cast<char *>(&value), sizeof(value));
            return value;
        }

        int16_t BaseFormatFile::int16()
        {
            return (int16_t) uint16();
        }

        uint8_t BaseFormatFile::uint8()
        {
            uint8_t value;
            _file->read(reinterpret_cast<char *>(&value), sizeof(value));
            return value;
        }

        int8_t BaseFormatFile::int8()
        {
            return (int8_t) uint8();
        }

        std::string BaseFormatFile::filename()
        {
            return _file->name();
        }

        std::istringstream BaseFormatFile::getStream()
        {
            _file->seek(0, SEEK_SET);
            std::string text;
            text.resize(_file->size());
            _file->read(&text, _file->size());
            return std::istringstream(text);
        }
    }
}
