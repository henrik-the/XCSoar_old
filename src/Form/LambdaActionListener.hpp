/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2021 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#ifndef XCSOAR_FORM_LAMBDA_ACTION_LISTENER_HPP
#define XCSOAR_FORM_LAMBDA_ACTION_LISTENER_HPP

#include "ActionListener.hpp"
#include "util/Compiler.h"

#include <utility>

/**
 * An adapter that forwards ActionListener::OnAction() calls to a
 * lambda expression.
 */
template<typename C>
class LambdaActionListener : public ActionListener, private C {
public:
  LambdaActionListener(C &&c):C(std::move(c)) {}

  void OnAction(int id) noexcept override {
    C::operator()(id);
  }
};

/**
 * Convert a lambda expression (a closure object) to an
 * ActionListener.  This is a convenience function.
 */
template<typename C>
LambdaActionListener<C>
MakeLambdaActionListener(C &&c)
{
  return LambdaActionListener<C>(std::move(c));
}

#endif
