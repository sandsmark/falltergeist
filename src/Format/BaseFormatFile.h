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

#ifndef FALLTERGEIST_FORMAT_BASEFORMATFILE_H
#define FALLTERGEIST_FORMAT_BASEFORMATFILE_H

// C++ standard includes
#include <sstream>

// Falltergeist includes

// Third party includes
#include <ttvfs/ttvfs.h>

namespace Falltergeist
{
    namespace Format
    {
        class BaseFormatFile
        {
            public:
                explicit BaseFormatFile(ttvfs::CountedPtr<ttvfs::File> file);
                virtual ~BaseFormatFile();

                BaseFormatFile& operator>>(int32_t &value);
                BaseFormatFile& operator>>(uint32_t &value);
                BaseFormatFile& operator>>(int16_t &value);
                BaseFormatFile& operator>>(uint16_t &value);
                BaseFormatFile& operator>>(int8_t &value);
                BaseFormatFile& operator>>(uint8_t &value);

                uint32_t uint32();
                int32_t int32();
                uint16_t uint16();
                int16_t int16();
                uint8_t uint8();
                int8_t int8();

                std::string filename();
                std::istringstream getStream();

            protected:
                ttvfs::CountedPtr<ttvfs::File> _file;

                bool _changeEndianness = false;
        };
    }
}


#endif //FALLTERGEIST_FORMAT_BASEFORMATFILE_H
