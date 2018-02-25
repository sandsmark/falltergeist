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
#include "../../Format/Int/File.h"
#include "../../Format/Int/Procedure.h"
#include "../../Exception.h"

// Third party includes

namespace Falltergeist
{
    namespace Format
    {
        namespace Int
        {
            File::File(ttvfs::CountedPtr<ttvfs::File> file) : BaseFormatFile(file)
            {
                _file->seek(0, SEEK_SET);

                // Initialization code goes here
                _file->seek(42, SEEK_SET);

                // Procedures table
                uint32_t proceduresCount;
                *this >> proceduresCount;

                std::vector<uint32_t> procedureNameOffsets;

                for (unsigned i = 0; i != proceduresCount; ++i) {
                    _procedures.emplace_back();
                    auto& procedure = _procedures.back();

                    uint32_t nameOffset, flags, delay, conditionOffset, bodyOffset, argumentsCounter;
                    *this >> nameOffset >> flags >> delay >> conditionOffset >> bodyOffset >> argumentsCounter;

                    procedureNameOffsets.push_back(nameOffset);
                    procedure.setFlags(flags);
                    procedure.setDelay(delay);
                    procedure.setConditionOffset(conditionOffset);
                    procedure.setBodyOffset(bodyOffset);
                    procedure.setArgumentsCounter(argumentsCounter);
                }

                // Identifiers table
                uint32_t tableSize;
                *this >> tableSize;
                unsigned j = 0;
                while (j < tableSize) {
                    uint16_t nameLength;
                    *this >> nameLength;
                    j += 2;

                    uint32_t nameOffset = j + 4;
                    std::string name;
                    for (unsigned i = 0; i != nameLength; ++i, ++j) {
                        uint8_t ch;
                        *this >> ch;
                        if (ch != 0) {
                            name.push_back(ch);
                        }
                    }

                    _identifiers.insert(std::make_pair(nameOffset, name)); // names of functions and variables
                }

                _file->seek(file->getpos() + 4, SEEK_SET);  // signature 0xFFFFFFFF

                for (unsigned i = 0; i != procedureNameOffsets.size(); ++i) {
                    _procedures.at(i).setName(_identifiers.at(procedureNameOffsets.at(i)));
                }

                // STRINGS TABLE
                uint32_t stringsTable;
                *this >> stringsTable;

                if (stringsTable != 0xFFFFFFFF) {
                    uint32_t j = 0;
                    while (j < stringsTable) {
                        uint16_t length;
                        *this >> length;
                        j += 2;
                        uint32_t nameOffset = j + 4;
                        std::string name;
                        for (unsigned i = 0; i != length; ++i, ++j)
                        {
                            uint8_t ch;
                            *this >> ch;
                            if (ch != 0) {
                                name.push_back(ch);
                            }
                        }
                        _strings.insert(std::make_pair(nameOffset, name));
                    }
                }
            }

            const std::map<unsigned int, std::string>& File::identifiers() const
            {
                return _identifiers;
            }

            const std::map<unsigned int, std::string>& File::strings() const
            {
                return _strings;
            }

            size_t File::position() const
            {
                return _file->getpos();
            }

            void File::setPosition(size_t pos)
            {
                _file->seek(pos, SEEK_SET);
            }

            size_t File::size()
            {
                return _file->size();
            }

            uint16_t File::readOpcode()
            {
                uint16_t opcode;
                *this >> opcode;
                return opcode;
            }

            uint32_t File::readValue()
            {
                uint32_t value;
                *this >> value;
                return value;
            }

            const std::vector<Procedure>& File::procedures() const
            {
                return _procedures;
            }

            const Procedure* File::procedure(const std::string& name) const
            {
                for (auto& procedure : _procedures) {
                    if (procedure.name() == name) {
                        return &procedure;
                    }
                }
                return nullptr;
            }
        }
    }
}
