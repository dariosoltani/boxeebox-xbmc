/*
 *      Copyright (C) 2005-2012 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "MythProgramInfo.h"
#include "MythTimestamp.h"
#include "MythPointer.h"

MythProgramInfo::MythProgramInfo()
  : m_proginfo_t()
  , m_UID()
  , m_frameRate(-1)
  , m_coverart("")
  , m_fanart("")
{
}

MythProgramInfo::MythProgramInfo(cmyth_proginfo_t cmyth_proginfo)
  : m_proginfo_t(new MythPointer<cmyth_proginfo_t>())
  , m_UID()
  , m_frameRate(-1)
  , m_coverart("")
  , m_fanart("")
{
  *m_proginfo_t = cmyth_proginfo;
  // Make Unique Identifier
  if (cmyth_proginfo)
  {
    MythTimestamp ts = MythTimestamp(cmyth_proginfo_rec_start(cmyth_proginfo));
    m_UID = MakeUID(ChannelID(), ts);
  }
}

CStdString MythProgramInfo::MakeUID(unsigned int chanid, MythTimestamp &ts)
{
  char buf[50] = "";
  CStdString timestr;
  if (!ts.IsUTC())
  {
    MythTimestamp utc = ts.ToUTC();
    timestr = utc.String();
  }
  else
    timestr = ts.String();
  sprintf(buf, "%u %s", chanid, timestr.c_str());
  return CStdString(buf);
}

bool MythProgramInfo::IsNull() const
{
  if (m_proginfo_t == NULL)
    return true;
  return *m_proginfo_t == NULL;
}

bool MythProgramInfo::operator ==(MythProgramInfo &other)
{
  if (!this->IsNull() && !other.IsNull() && this->UID() == other.UID())
    return true;
  return false;
}

bool MythProgramInfo::operator !=(MythProgramInfo &other)
{
  return !(*this == other);
}

CStdString MythProgramInfo::UID() const
{
  return m_UID;
}

CStdString MythProgramInfo::ProgramID()
{
  char* progId = cmyth_proginfo_programid(*m_proginfo_t);
  CStdString retval(progId);
  ref_release(progId);
  return retval;
}

CStdString MythProgramInfo::Title()
{
  char* title = cmyth_proginfo_title(*m_proginfo_t);
  CStdString retval(title);
  ref_release(title);
  return retval;
}

CStdString MythProgramInfo::Subtitle()
{
  char* subtitle = cmyth_proginfo_subtitle(*m_proginfo_t);
  CStdString retval(subtitle);
  ref_release(subtitle);
  return retval;
}

CStdString MythProgramInfo::BaseName()
{
  char* path = cmyth_proginfo_pathname(*m_proginfo_t);
  CStdString retval(path);
  ref_release(path);
  return retval;
}

CStdString MythProgramInfo::Description()
{
  char* desc = cmyth_proginfo_description(*m_proginfo_t);
  CStdString retval(desc);
  ref_release(desc);
  return retval;
}

int MythProgramInfo::Duration()
{
  MythTimestamp end = cmyth_proginfo_rec_end(*m_proginfo_t);
  MythTimestamp start = cmyth_proginfo_rec_start(*m_proginfo_t);
  return end.UnixTime() - start.UnixTime();
  //return cmyth_proginfo_length_sec(*m_proginfo_t);
}

CStdString MythProgramInfo::Category()
{
  char* cat = cmyth_proginfo_category(*m_proginfo_t);
  CStdString retval(cat);
  ref_release(cat);
  return retval;
}

time_t MythProgramInfo::StartTime()
{
  MythTimestamp time = cmyth_proginfo_start(*m_proginfo_t);
  time_t retval(time.UnixTime());
  return retval;
}

time_t MythProgramInfo::EndTime()
{
  MythTimestamp time = cmyth_proginfo_end(*m_proginfo_t);
  time_t retval(time.UnixTime());
  return retval;
}

bool MythProgramInfo::IsWatched()
{
  unsigned long recording_flags = cmyth_proginfo_flags(*m_proginfo_t);
  return (recording_flags & 0x00000200) != 0; // FL_WATCHED
}

bool MythProgramInfo::IsDeletePending()
{
  unsigned long recording_flags = cmyth_proginfo_flags(*m_proginfo_t);
  return (recording_flags & 0x00000080) != 0; // FL_DELETEPENDING
}

bool MythProgramInfo::HasBookmark()
{
  unsigned long recording_flags = cmyth_proginfo_flags(*m_proginfo_t);
  return (recording_flags & 0x00000010) != 0; // FL_BOOKMARK
}

bool MythProgramInfo::IsVisible()
{
  // Filter out recording of special storage group Deleted
  // Filter out recording with duration less than 5 seconds
  // When  deleting a recording, it might not be deleted immediately but marked as 'pending delete'.
  // Depending on the protocol version the recording is moved to the group Deleted or
  // the 'delete pending' flag is set
  if (Duration() < 5 || RecordingGroup() == "Deleted" || IsDeletePending())
  {
    return false;
  }

  return true;
}

bool MythProgramInfo::IsLiveTV()
{
  return (RecordingGroup() == "LiveTV");
}

unsigned int MythProgramInfo::ChannelID()
{
  return cmyth_proginfo_chan_id(*m_proginfo_t);
}

CStdString MythProgramInfo::ChannelName()
{
  char* chan = cmyth_proginfo_channame(*m_proginfo_t);
  CStdString retval(chan);
  ref_release(chan);
  return retval;
}

MythProgramInfo::RecordStatus MythProgramInfo::Status()
{
  return cmyth_proginfo_rec_status(*m_proginfo_t);
}

CStdString MythProgramInfo::RecordingGroup()
{
  char* recgroup = cmyth_proginfo_recgroup(*m_proginfo_t);
  CStdString retval(recgroup);
  ref_release(recgroup);
  return retval;
}

unsigned int MythProgramInfo::RecordID()
{
  return cmyth_proginfo_recordid(*m_proginfo_t);
}

time_t MythProgramInfo::RecordingStartTime()
{
  MythTimestamp time = cmyth_proginfo_rec_start(*m_proginfo_t);
  time_t retval(time.UnixTime());
  return retval;
}

time_t MythProgramInfo::RecordingEndTime()
{
  MythTimestamp time = cmyth_proginfo_rec_end(*m_proginfo_t);
  time_t retval(time.UnixTime());
  return retval;
}

int MythProgramInfo::Priority()
{
  return cmyth_proginfo_priority(*m_proginfo_t); // Might want to use recpriority2 instead
}

CStdString MythProgramInfo::StorageGroup()
{
  char* storageGroup = cmyth_proginfo_storagegroup(*m_proginfo_t);
  CStdString retval(storageGroup);
  ref_release(storageGroup);
  return retval;
}

void MythProgramInfo::SetFrameRate(const long long frameRate)
{
  m_frameRate = frameRate;
}

long long MythProgramInfo::FrameRate() const
{
  return m_frameRate;
}

CStdString MythProgramInfo::IconPath()
{
  return (BaseName() + ".png");
}

CStdString MythProgramInfo::Coverart() const
{
  return m_coverart;
}

CStdString MythProgramInfo::Fanart() const
{
  return m_fanart;
}
