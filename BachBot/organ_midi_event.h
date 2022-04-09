/**
 * @file organ_midi_event.h
 * @brief Individual MIDI event sent to organ.
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
 * This represents the storage for an individual MIDI event to be relayed to
 * the organ console.  These events will usually be either note-on or note-off
 * events, however these may also be created as bank-change commands.
 */

#pragma once


//  system includes
#include <cstdint>
#include <optional>  //  std::optional
#include <utility>  //  std::pair
#include <wx/wx.h>  //  wxLongLong
#include <RtMidi.h>  //  RtMidiOut

#include "MidiEvent.h"  //  smf::MidiEvent

#include "common_defs.h"  //  SyndineKeyboards


/**
 * @brief Type for setting/getting the desired bank configuration.
 * @note
 * The organ represents banks as numbers 1-8 (as shown on the screen).  Banks
 * roll over to the next "piston mode".  IE 1,1 - 1,2 ... 1,8, 2,1, 2,2 ... etc
 * "Cancel" always sets the bank back to the first bank of the current piston
 * mode.  This represents the only way of controlling stop settings from the
 * MIDI interface.
 *  - `first` current bank (0-7)
 *  - `second` current mode (0-100)
 */
using BankConfig = std::pair<uint8_t, uint32_t>;

/**
 * @brief Organ MIDI event storage class.
 */
class OrganMidiEvent
{
    //  Allow the note deduplication logic to mess with the internals.
    friend class MidiNoteTracker;

public:
    /**
     * @brief Construct from a MIDI event and map to a specific keyboard.
     * @param midi_event Midi event to take timing and note information from.
     * @param channel Route MIDI event to this keyboard.
     */
    OrganMidiEvent(const smf::MidiEvent& midi_event, 
                   const SyndineKeyboards channel);

    /**
     * @brief Construct a bank change event.
     * @param midi_event Midi event to take timing information from.
     * @param cfg Generate new bank configuration.
     */
    OrganMidiEvent(const smf::MidiEvent &midi_event, const BankConfig& cfg);
    OrganMidiEvent(const MidiCommands command, 
                   const SyndineKeyboards channel,
                   const int8_t byte1=-1,
                   const int8_t byte2=-1);

    /**
     * @brief Allow default copy construction.
     */
    OrganMidiEvent(const OrganMidiEvent&) = default;

    /**
     * @brief Subtract the event timing of one MIDI event from this one.
     * @param rhs Midi event timing.
     * @return `this`
     * @note This is intended to set a 0-delay gap between "songs" or introduce
     *       a large gap for intra-song manual triggering.
     */
    OrganMidiEvent& operator-=(const OrganMidiEvent &rhs);

    /**
     * @brief Send this event to the organ.
     * @param player MIDI device Output reference.
     */
    void send_event(RtMidiOut &player) const;
    
    /**
     * @brief Get the event timing in microseconds.
     * @return Event time relative to the start of song.
     */
    wxLongLong get_us() const;
    
    /**
     * @brief Set the desired bank configuration that this note should be 
     *        played at.
     * @param cfg Bank configuration
     */
    void set_bank_config(const BankConfig &cfg);
    
    /**
     * @brief Get the desired bank configuration that this note should be 
     *        played at.
     * @return Bank configuration
     */
    BankConfig get_bank_config() const;
    
    /**
     * @brief Test if this event is a bank change event.
     * @retval `true` Event represents a bank change event
     * @retval `false` Event represents a note event
     */
    bool is_mode_change_event() const;
    
    /**
     * @brief Calculate the real time and midi time delta between this event
     *        and another event.  Internal deltas updated.
     * @param rhs Midi event of reference.
    */
    void calculate_delta(const OrganMidiEvent& rhs);

    /**
     * @brief Operator used for sorting events by event time.
     * @param rhs Event to test in sort function.
     * @return sort order
    */
    constexpr bool operator< (const OrganMidiEvent& rhs) const
    {
        return (m_seconds < rhs.m_seconds);
    }

private:
    uint8_t m_event_code;  ///<  This event command
    bool m_mode_change_event; ///< Was this constructed as a mode change event?
    uint8_t m_desired_bank_number;  ///<  Store the desired bank number
    uint32_t m_desired_mode_number;  ///<  Store the desired piston mode number
    double m_seconds;  ///<  Event time in seconds.
    double m_delta_time;  ///<  Delta seconds since last event.
    std::optional<uint8_t> m_byte1;  ///<  MIDI event payload first byte
    std::optional<uint8_t> m_byte2;  ///<  MIDI event payload second byte
    int m_midi_time;  ///<  Midi event MIDI ticks time.
    int m_delta;  ///<  Midi ticks since last event.
};
