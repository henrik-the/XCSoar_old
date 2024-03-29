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

#include "AudioConfigPanel.hpp"
#include "Audio/Features.hpp"

#ifdef HAVE_VOLUME_CONTROLLER

#include "Interface.hpp"
#include "UIGlobals.hpp"
#include "Audio/GlobalVolumeController.hpp"
#include "Audio/VolumeController.hpp"
#include "Language/Language.hpp"
#include "Profile/ProfileKeys.hpp"
#include "Widget/RowFormWidget.hpp"

enum ControlIndex {
  MasterVolume,
};


class AudioConfigPanel final : public RowFormWidget {
public:
  AudioConfigPanel() : RowFormWidget(UIGlobals::GetDialogLook()) {
  }

public:
  virtual void Prepare(ContainerWindow &parent, const PixelRect &rc) override;
  virtual bool Save(bool &changed) override;
};

void
AudioConfigPanel::Prepare(ContainerWindow &parent, const PixelRect &rc)
{
  RowFormWidget::Prepare(parent, rc);

  const auto &settings = CommonInterface::GetUISettings().sound;

  AddInteger(_("Master Volume"), nullptr, _T("%d %%"), _T("%d"),
             0, VolumeController::GetMaxValue(), 1, settings.master_volume);
}

bool
AudioConfigPanel::Save(bool &changed)
{
  auto &settings = CommonInterface::SetUISettings().sound;

  unsigned volume = settings.master_volume;
  if (SaveValue(MasterVolume, ProfileKeys::MasterAudioVolume, volume)) {
    settings.master_volume = volume;
    changed = true;
  }

  return true;
}

Widget *
CreateAudioConfigPanel()
{
  return new AudioConfigPanel();
}

#endif /* HAVE_VOLUME_CONTROLLER */
