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
#include <string>
#include <cstdlib>

// Falltergeist includes
#include "../../Exception.h"
#include "../../Format/Msg/File.h"
#include "../../Format/Msg/Message.h"

// Third party includes

namespace Falltergeist
{
    namespace Format
    {
        namespace Msg
        {
            File::File(ttvfs::CountedPtr<ttvfs::File> file) : BaseFormatFile(file)
            {
                _file->seek(0, SEEK_SET);

                /*
                 * Because of bug in CMBATAI2.MSG in messages #1382 and #32020 we need to explode each line with '{' symbol
                 * Any extra '}' symbols must be trimed from exploded parts
                 */

                while (_file->getpos() < _file->size())
                {
                    uint8_t chr;
                    *this >> chr;
                    if (chr == '{') {
                        std::string number;
                        std::string sound;
                        std::string text;
                        chr = 0;
                        // number
                        while (chr != '{') {
                            *this >> chr;
                            if (chr != '{' && chr != '}') {
                                number += chr;
                            }
                        }

                        // sound
                        chr = 0;
                        while (chr != '{') {
                            *this >> chr;
                            if (chr != '{' && chr != '}') {
                                sound += chr;
                            }
                        }

                        chr = 0;
                        // text
                        while (chr != '}' && chr != '{' && _file->getpos() < _file->size()) {
                            *this >> chr;
                            if (chr != '{' && chr != '}') {
                                text += chr;
                            }
                        }

                        // "put back" last character
                        _file->seek(_file->getpos() - 1, SEEK_SET);

                        while (text.find("\n") != std::string::npos) {
                            text.replace(text.find("\n"), 1, "");
                        }
                        while (text.find("\r") != std::string::npos) {
                            text.replace(text.find("\r"), 1, "");
                        }

                        Message message;
                        message.setNumber(std::stoi(number));
                        message.setSound(sound);
                        message.setText(text);
                        _messages.push_back(message);
                    }
                }
            }

            Message* File::message(unsigned int number)
            {
                for (auto& message : _messages) {
                    if (message.number() == number) {
                        return &message;
                    }
                }
                throw Exception("File::message() - number is out of range: " + std::to_string(number));
            }
        }
    }
}
