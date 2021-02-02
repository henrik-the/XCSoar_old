/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2016 The XCSoar Project
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

#include "TaskActionsPanel.hpp"
#include "TaskMiscPanel.hpp"
#include "TaskListPanel.hpp"
#include "Internal.hpp"
#include "../dlgTaskHelpers.hpp"
#include "Dialogs/Message.hpp"
#include "Components.hpp"
#include "Logger/ExternalLogger.hpp"
#include "Simulator.hpp"
#include "Language/Language.hpp"
#include "Interface.hpp"
#include "Device/Declaration.hpp"
#include "Task/ValidationErrorStrings.hpp"
#include "Engine/Task/Ordered/OrderedTask.hpp"
#include "Engine/Task/Factory/AbstractTaskFactory.hpp"
#include "Engine/Waypoint/Waypoints.hpp"

//henrik
#include "UIGlobals.hpp"
#include "Dialogs/JobDialog.hpp"
#include "Util/ConvertString.hpp"
#include "Net/HTTP/Session.hpp"
#include "Net/HTTP/ToFile.hpp"
#include "Net/HTTP/ToBuffer.hpp"
#include "Job/Runner.hpp"
#include "LocalPath.hpp"
#include "Dialogs/Error.hpp"
#include "LogFile.hpp"

//mit Download Manager
#include "Net/HTTP/DownloadManager.hpp"

TaskActionsPanel::TaskActionsPanel(TaskManagerDialog &_dialog,
                                   TaskMiscPanel &_parent,
                                   OrderedTask **_active_task,
                                   bool *_task_modified)
  :RowFormWidget(_dialog.GetLook()),
   dialog(_dialog), parent(_parent),
   active_task(_active_task), task_modified(_task_modified) {}

void
TaskActionsPanel::SaveTask()
{
  AbstractTaskFactory &factory = (*active_task)->GetFactory();
  factory.UpdateStatsGeometry();
  if (factory.CheckAddFinish())
    factory.UpdateGeometry();

  if ((*active_task)->CheckTask()) {
    if (!OrderedTaskSave(**active_task))
      return;

    *task_modified = true;
    dialog.UpdateCaption();
    DirtyTaskListPanel();
  } else {
    ShowMessageBox(getTaskValidationErrors(
        (*active_task)->GetFactory().GetValidationErrors()), _("Task not saved"),
        MB_ICONEXCLAMATION);
  }
}

inline void
TaskActionsPanel::OnBrowseClicked()
{
  parent.SetCurrent(1);
}

inline void
TaskActionsPanel::OnNewTaskClicked()
{
  if (((*active_task)->TaskSize() < 2) ||
      (ShowMessageBox(_("Create new task?"), _("Task New"),
                   MB_YESNO|MB_ICONQUESTION) == IDYES)) {
    (*active_task)->Clear();
    (*active_task)->SetFactory(CommonInterface::GetComputerSettings().task.task_type_default);
    *task_modified = true;
    dialog.SwitchToPropertiesPanel();
  }
}

inline void
TaskActionsPanel::OnDeclareClicked()
{
  if (!(*active_task)->CheckTask()) {
    const auto errors =
      (*active_task)->GetFactory().GetValidationErrors();
    ShowMessageBox(getTaskValidationErrors(errors), _("Declare task"),
                MB_ICONEXCLAMATION);
    return;
  }

  const ComputerSettings &settings = CommonInterface::GetComputerSettings();
  Declaration decl(settings.logger, settings.plane, *active_task);
  ExternalLogger::Declare(decl, way_points.GetHome().get());
}

inline void
TaskActionsPanel::OnDownloadClicked()
{
  LogFormat("start\n");
  //DialogJobRunner runner42(UIGlobals::GetMainWindow(),UIGlobals::GetDialogLook(),_("Download"), true);
  //DialogJobRunner runner(dialog.GetMainWindow(), dialog.GetLook(),_("Download"), true);
  //Net::Session session42;

  char url[256];
  snprintf(url, sizeof(url),"https://api.weglide.org/v1/task/declaration/67?cup=false&tsk=true");
  const auto cache_path = MakeLocalPath(_T("weglide"));
  //const auto path = AllocatedPath::Build(cache_path, UTF8ToWideConverter("weglide_declared.tsk"));
  Net::DownloadManager::Enqueue(url, Path("weglide/weglide_declared.tsk"));
}

void
TaskActionsPanel::ReClick()
{
  dialog.TaskViewClicked();
}

void
TaskActionsPanel::Prepare(ContainerWindow &parent, const PixelRect &rc)
{
  AddButton(_("New Task"), *this, NEW_TASK);
  AddButton(_("Declare"), *this, DECLARE);
  AddButton(_("Browse"), *this, BROWSE);
  AddButton(_("Save"), *this, SAVE);
  AddButton(_("Download WeGlide"),*this, DOWNLOAD);

  if (is_simulator())
    /* cannot communicate with real devices in simulator mode */
    SetRowEnabled(DECLARE, false);
}

void
TaskActionsPanel::OnAction(int id) noexcept
{
  switch (id) {
  case NEW_TASK:
    OnNewTaskClicked();
    break;

  case DECLARE:
    OnDeclareClicked();
    break;

  case BROWSE:
    OnBrowseClicked();
    break;

  case SAVE:
    SaveTask();
    break;

  case DOWNLOAD:
    OnDownloadClicked();
    break;
  }
}
