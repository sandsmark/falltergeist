/*
 * Copyright 2012-2014 Falltergeist Developers.
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

// Related headers
#include "../../VM/Handler/Opcode8115Handler.h"

// C++ standard includes

// Falltergeist includes
#include "../../Logger.h"
#include "../../Game/Game.h"
#include "../../State/Movie.h"
#include "../../VM/Script.h"

// Third party includes

namespace Falltergeist {
    namespace VM {
        namespace Handler {
            Opcode8115::Opcode8115(VM::Script *script) : OpcodeHandler(script) {
            }

            void Opcode8115::_run() {
                Logger::debug("SCRIPT") << "[8115] [*] void playMovie(int movie)" << std::endl;
                int movie = _script->dataStack()->popInteger();
                auto state = std::make_shared<State::Movie>(movie);
                Game::Game::getInstance()->pushState(std::move(state));
            }
        }
    }
}
