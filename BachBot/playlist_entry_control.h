/**
 * @file playlist_entry_control.h
 * @brief Playlist song entry display panel
 * @copyright
 * 2022 Andrew Buettner (ABi)
 *
 * @section LICENSE
 *
 * BachBot - A hymn Midi player for Schlicker organs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * Because playlists can cause the loading of several "large" songs, this
 * process can potentially take several seconds on an under-powered machine.
 * Rather than bottling up the UI, do most of the loading in a separate thread
 * and force a top-level modal dialog box during the process.
 */

#pragma once

//  system includes
#include <cstdint>  //  uint32_t
#include <functional>  //  std::function
#include <wx/wx.h>  //  wxString

 //  module includes
 // -none-

 //  local includes
#include "main_window.h"  //  PlaylistEntryPanel
#include "play_list.h"  //  PlayListEntry

namespace bach_bot {
namespace ui {


class PlaylistEntryControl : public PlaylistEntryPanel
{
    /** Callback function format for events generated by this class */
    using CallBack = std::function<void(uint32_t /* song_id */,
                                        PlaylistEntryControl* /* `this` */,
                                        bool /* Function specific */)>;

public:
    PlaylistEntryControl(wxWindow *const parent, const PlayListEntry &song);

    const wxString& get_filename() const;
    void set_next();
    void set_playing();
    void reset_status();
    void set_autoplay(const bool autoplay_enabled);

    //  Allow the player window to set these directly:
    /** Configure button clicked bool: unused (always false) */
    CallBack configure_event;

    /** Checkbox changed event bool: current checkbox state */
    CallBack checkbox_event;

    /** Muve up/down request bool: `true` - up, `false` down */
    CallBack move_event;

    /** Set next song bool: unused (always true) */
    CallBack set_next_event;

protected:
    virtual void on_configure_clicked(wxCommandEvent& event) override final;
    virtual void on_checkbox_checked(wxCommandEvent &event) override final;
    virtual void on_set_next(wxCommandEvent &event) override final;
    virtual void on_move_up(wxCommandEvent &event) override final;
    virtual void on_move_down(wxCommandEvent &event) override final;

private:
    void setup_widgets();
    static void dummy_event(uint32_t, PlaylistEntryControl*, bool);

    wxWindow *const m_parent;
    const uint32_t m_song_id;
    const wxString m_filename;
    bool m_autoplay;
    bool m_up_next;
    bool m_playing;
};

}  //  end ui
}  //  end bach_bot
