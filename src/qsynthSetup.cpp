// qsynthSetup.cpp
//
/****************************************************************************
   Copyright (C) 2003, rncbc aka Rui Nuno Capela. All rights reserved.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*****************************************************************************/

#include "qsynthSetup.h"
#include "qsynthAbout.h"

#include "config.h"


//-------------------------------------------------------------------------
// qsynthSetup - Prototype settings structure.
//

// Constructor.
qsynthSetup::qsynthSetup (void)
{
    m_pFluidSettings = ::new_fluid_settings();
    m_iSoundfontOverride = 0;

    m_settings.beginGroup("/qsynth");

    // Load previous/default fluidsynth settings...
    m_settings.beginGroup("/Settings");
    bMidiIn          = m_settings.readBoolEntry    ("/MidiIn",          true);
    sMidiDriver      = m_settings.readEntry        ("/MidiDriver",      "alsa_seq");
    iMidiChannels    = m_settings.readNumEntry     ("/MidiChannels",    0);
    sAudioDriver     = m_settings.readEntry        ("/AudioDriver",     "jack");
    sJackName        = m_settings.readEntry        ("/JackName",        "qsynth");
    bJackAutoConnect = m_settings.readBoolEntry    ("/JackAutoConnect", true);
    bJackMulti       = m_settings.readBoolEntry    ("/JackMulti",       false);
    iAudioChannels   = m_settings.readNumEntry     ("/AudioChannels",   0);
    iAudioGroups     = m_settings.readNumEntry     ("/AudioGroups",     0);
    iAudioBufSize    = m_settings.readNumEntry     ("/AudioBufSize",    0);
    iAudioBufCount   = m_settings.readNumEntry     ("/AudioBufCount",   0);
    sSampleFormat    = m_settings.readEntry        ("/SampleFormat",    "16bits");
    fSampleRate      = m_settings.readDoubleEntry  ("/SampleRate",      0.0);
    iPolyphony       = m_settings.readNumEntry     ("/Polyphony",       0);
    iAudioBufCount   = m_settings.readNumEntry     ("/AudioBufCount",   0);
    bReverbActive    = m_settings.readBoolEntry    ("/ReverbActive",    true);
    bChorusActive    = m_settings.readBoolEntry    ("/ChorusActive",    true);
    bLadspaActive    = m_settings.readBoolEntry    ("/LadspaActive",    false);
    fGain            = m_settings.readDoubleEntry  ("/Gain",            0.0);
    bServer          = m_settings.readBoolEntry    ("/Server",          false);
    bMidiDump        = m_settings.readBoolEntry    ("/MidiDump",        false);
    bVerbose         = m_settings.readBoolEntry    ("/Verbose",         false);
    m_settings.endGroup();

    // Load soundfont list...
    m_settings.beginGroup("/SoundFonts");
    int i = 0;
    for (;;) {
        QString sSoundFont = m_settings.readEntry("/SoundFont" + QString::number(++i), QString::null);
        if (sSoundFont.isEmpty())
            break;
        soundfonts.append(sSoundFont);
    }
    m_settings.endGroup();

    // Load display options...
    m_settings.beginGroup("/Options");
    sMessagesFont = m_settings.readEntry("/MessagesFont", QString::null);
    m_settings.endGroup();
}


// Default Destructor.
qsynthSetup::~qsynthSetup (void)
{
    // Make program version available in the future.
    m_settings.beginGroup("/Program");
    m_settings.writeEntry("/Version", QSYNTH_VERSION);
    m_settings.endGroup();

    // Save last display options.
    m_settings.beginGroup("/Options");
    m_settings.writeEntry("/MessagesFont", sMessagesFont);
    m_settings.endGroup();

    // Save last soundfont list.
    m_settings.beginGroup("/SoundFonts");
    int i = 0;
    for (QStringList::Iterator iter = soundfonts.begin(); iter != soundfonts.end(); iter++)
        m_settings.writeEntry("/SoundFont" + QString::number(++i), *iter);
    m_settings.endGroup();

    // Save last fluidsynth settings.
    m_settings.beginGroup("/Settings");
    m_settings.writeEntry("/MidiIn",           bMidiIn);
    m_settings.writeEntry("/MidiDriver",       sMidiDriver);
    m_settings.writeEntry("/MidiChannels",     iMidiChannels);
    m_settings.writeEntry("/AudioDriver",      sAudioDriver);
    m_settings.writeEntry("/JackName",         sJackName);
    m_settings.writeEntry("/JackAutoConnect",  bJackAutoConnect);
    m_settings.writeEntry("/JackMulti",        bJackMulti);
    m_settings.writeEntry("/AudioChannels",    iAudioChannels);
    m_settings.writeEntry("/AudioGroups",      iAudioGroups);
    m_settings.writeEntry("/AudioBufSize",     iAudioBufSize);
    m_settings.writeEntry("/AudioBufCount",    iAudioBufCount);
    m_settings.writeEntry("/SampleFormat",     sSampleFormat);
    m_settings.writeEntry("/SampleRate",       fSampleRate);
    m_settings.writeEntry("/Polyphony",        iPolyphony);
    m_settings.writeEntry("/ReverbActive",     bReverbActive);
    m_settings.writeEntry("/ChorusActive",     bChorusActive);
    m_settings.writeEntry("/LadspaActive",     bLadspaActive);
    m_settings.writeEntry("/Gain",             fGain);
    m_settings.writeEntry("/Server",           bServer);
    m_settings.writeEntry("/MidiDump",         bMidiDump);
    m_settings.writeEntry("/Verbose",          bVerbose);
    m_settings.endGroup();

    m_settings.endGroup();

    ::delete_fluid_settings(m_pFluidSettings);
    m_pFluidSettings = NULL;
}


// Fluidsynth settings accessor.
fluid_settings_t *qsynthSetup::fluid_settings (void)
{
    return m_pFluidSettings;
}


//-------------------------------------------------------------------------
// Command-line argument stuff. Mostly to mimic fluidsynth CLI.
//

// Help about command line options.
void qsynthSetup::print_usage ( const char *arg0 )
{
    const QString sEot = "\n\t";
    const QString sEol = "\n\n";

    fprintf(stderr, QObject::tr("Usage") + ": %s"
        " [" + QObject::tr("options")    + "]"
        " [" + QObject::tr("soundfonts") + "]"
        " [" + QObject::tr("midifiles")  + "]" + sEol, arg0);
    fprintf(stderr, "%s - %s\n\n", QSYNTH_TITLE, QSYNTH_SUBTITLE);
    fprintf(stderr, QObject::tr("Options") + ":" + sEol);
    fprintf(stderr, "  -n, --no-midi-in" + sEot +
	   QObject::tr("Don't create a midi driver to read MIDI input events [default = yes]") + sEol);
    fprintf(stderr, "  -m, --midi-driver=[label]" + sEot +
       QObject::tr("The name of the midi driver to use [oss,alsa,alsa_seq,...]") + sEol);
    fprintf(stderr, "  -K, --midi-channels=[num]" + sEot +
       QObject::tr("The number of midi channels [default = 16]") + sEol);
    fprintf(stderr, "  -a, --audio-driver=[label]" + sEot +
       QObject::tr("The audio driver [alsa,jack,oss,dsound,...]") + sEol);
    fprintf(stderr, "  -j, --connect-jack-outputs" + sEot +
       QObject::tr("Attempt to connect the jack outputs to the physical ports") + sEol);
    fprintf(stderr, "  -L, --audio-channels=[num]" + sEot +
       QObject::tr("The number of stereo audio channels [default = 1]") + sEol);
    fprintf(stderr, "  -G, --audio-groups=[num]" + sEot +
       QObject::tr("The number of audio groups [default = 1]") + sEol);
    fprintf(stderr, "  -z, --audio-bufsize=[size]" + sEot +
       QObject::tr("Size of each audio buffer") + sEol);
    fprintf(stderr, "  -c, --audio-bufcount=[count]" + sEot +
       QObject::tr("Number of audio buffers") + sEol);
    fprintf(stderr, "  -r, --sample-rate=[rate]" + sEot +
       QObject::tr("Set the sample rate") + sEol);
    fprintf(stderr, "  -R, --reverb=[flag]" + sEot +
       QObject::tr("Turn the reverb on or off [1|0|yes|no|on|off, default = on]") + sEol);
    fprintf(stderr, "  -C, --chorus=[flag]" + sEot +
       QObject::tr("Turn the chorus on or off [1|0|yes|no|on|off, default = on]") + sEol);
    fprintf(stderr, "  -g, --gain=[gain]" + sEot +
       QObject::tr("Set the master gain [0 < gain < 10, default = 0.2]") + sEol);
    fprintf(stderr, "  -o, --option [name=value]" + sEot +
       QObject::tr("Define a setting name=value") + sEol);
    fprintf(stderr, "  -s, --server" + sEot +
	   QObject::tr("Create and start server [default = no]") + sEol);
    fprintf(stderr, "  -i, --no-shell" + sEot +
       QObject::tr("Don't read commands from the shell [ignored]") + sEol);
    fprintf(stderr, "  -d, --dump" + sEot +
       QObject::tr("Dump midi router events") + sEol);
    fprintf(stderr, "  -v, --verbose" + sEot +
       QObject::tr("Print out verbose messages about midi events") + sEol);
    fprintf(stderr, "  -h, --help" + sEot + +
       QObject::tr("Show help about command line options") + sEol);
    fprintf(stderr, "  -V, --version" + sEot +
       QObject::tr("Show version information") + sEol);
}


// Special parsing of '-o' command-line option into fluidsynth settings.
bool qsynthSetup::parse_option ( char *optarg )
{
    char *val;

    for (val = optarg; *val; val++) {
        if (*val == '=') {
            *val++ = (char) 0;
            break;
        }
    }

    switch (::fluid_settings_get_type(m_pFluidSettings, optarg)) {
    case FLUID_NUM_TYPE:
        if (::fluid_settings_setnum(m_pFluidSettings, optarg, ::atof(val)))
            break;
    case FLUID_INT_TYPE:
        if (::fluid_settings_setint(m_pFluidSettings, optarg, ::atoi(val)))
            break;
    case FLUID_STR_TYPE:
        if (::fluid_settings_setstr(m_pFluidSettings, optarg, val))
            break;
    default:
        return false;
    }

    return true;
}


// Parse command line arguments into fluidsynth settings.
bool qsynthSetup::parse_args ( int argc, char **argv )
{
    const QString sEol = "\n\n";

    for (int i = 1; i < argc; i++) {

        QString sArg = argv[i];
        QString sVal = QString::null;
        int iEqual = sArg.find("=");
        if (iEqual >= 0) {
            sVal = sArg.right(sArg.length() - iEqual - 1);
            sArg = sArg.left(iEqual);
        }
        else if (i < argc)
            sVal = argv[i + 1];

        if (sArg == "-n" || sArg == "--no-midi-in") {
            bMidiIn = false;
        }
        else if (sArg == "-m" || sArg == "--midi-driver") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -m requires an argument (midi-driver).") + sEol);
                return false;
            }
            sMidiDriver = sVal;
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-K" || sArg == "--midi-channels") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -K requires an argument (midi-channels).") + sEol);
                return false;
            }
            iMidiChannels = sVal.toInt();
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-a" || sArg == "--audio-driver") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -a requires an argument (audio-driver).") + sEol);
                return false;
            }
            sAudioDriver = sVal;
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-j" || sArg == "--connect-jack-outputs") {
            bJackAutoConnect = true;
        }
        else if (sArg == "-L" || sArg == "--audio-channels") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -L requires an argument (audio-channels).") + sEol);
                return false;
            }
            iAudioChannels = sVal.toInt();
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-G" || sArg == "--audio-groups") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -G requires an argument (audio-groups).") + sEol);
                return false;
         }
            iAudioGroups = sVal.toInt();
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-z" || sArg == "--audio-bufsize") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -z requires an argument (audio-bufsize).") + sEol);
                return false;
            }
            iAudioBufSize = sVal.toInt();
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-c" || sArg == "--audio-bufcount") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -c requires an argument (audio-bufcount).") + sEol);
                return false;
            }
            iAudioBufCount = sVal.toInt();
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-r" || sArg == "--sample-rate") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -r requires an argument (sample-rate).") + sEol);
                return false;
            }
            fSampleRate = sVal.toFloat();
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-R" || sArg == "--reverb") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -R requires an argument (reverb).") + sEol);
                return false;
            }
            bReverbActive = !(sVal == "0" || sVal == "no" || sVal == "off");
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-C" || sArg == "--chorus") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -C requires an argument (chorus).") + sEol);
                return false;
            }
            bChorusActive = !(sVal == "0" || sVal == "no" || sVal == "off");
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-g" || sArg == "--gain") {
        	if (sVal.isNull()) {
                fprintf(stderr, QObject::tr("Option -g requires an argument (gain).") + sEol);
                return false;
            }
            fGain = sVal.toFloat();
            if (iEqual < 0)
                i++;
        }
        else if (sArg == "-o" || sArg == "--option") {
        	if (++i >= argc) {
                fprintf(stderr, QObject::tr("Option -o requires an argument.") + sEol);
                return false;
            }
            if (!parse_option(argv[i])) {
                fprintf(stderr, QObject::tr("Option -o failed to set") + " \"%s\"." + sEol, argv[i]);
                return false;
            }
        }
        else if (sArg == "-s" || sArg == "--server") {
            bServer = true;
        }
        else if (sArg == "-i" || sArg == "--no-shell") {
            // Just ignore this...
        }
        else if (sArg == "-d" || sArg == "--dump") {
            bMidiDump = true;
        }
        else if (sArg == "-v" || sArg == "--verbose") {
            bVerbose = true;
        }
        else if (sArg == "-h" || sArg == "--help") {
            print_usage(argv[0]);
            return false;
        }
        else if (sArg == "-V" || sArg == "--version") {
            fprintf(stderr, "Qt: %s\n", qVersion());
            fprintf(stderr, "qsynth: %s\n", QSYNTH_VERSION);
            return false;
        }
        else if (::fluid_is_soundfont(argv[i])) {
            if (++m_iSoundfontOverride == 1)
                soundfonts.clear();
            soundfonts.append(argv[i]);
        }
        else if (::fluid_is_midifile(argv[i])) {
            midifiles.append(argv[i]);
        }
        else {
            fprintf(stderr, QObject::tr("Unknown option") + " %s" + sEol, argv[i]);
            print_usage(argv[0]);
            return false;
        }
    }

    // Alright with argument parsing.
    return true;
}


//-------------------------------------------------------------------------
// Settings cache realization.
//

void qsynthSetup::realize (void)
{
    // The 'groups' setting is only relevant for LADSPA operation
    // If not given, set number groups to number of audio channels, because
    // they are the same (there is nothing between synth output and 'sound card')
    if ((iAudioGroups == 0) && (iAudioChannels != 0))
        iAudioGroups = iAudioChannels;

    if (!sMidiDriver.isEmpty())
        ::fluid_settings_setstr(m_pFluidSettings, "midi.driver", (char *) sMidiDriver.latin1());
    if (iMidiChannels > 0)
        ::fluid_settings_setint(m_pFluidSettings, "synth.midi-channels", iMidiChannels);
    if (!sAudioDriver.isEmpty())
        ::fluid_settings_setstr(m_pFluidSettings, "audio.driver", (char *) sAudioDriver.latin1());
    if (!sJackName.isEmpty())
        ::fluid_settings_setstr(m_pFluidSettings, "audio.jack.id", (char *) sJackName.latin1());
        
    ::fluid_settings_setint(m_pFluidSettings, "audio.jack.autoconnect", (int) bJackAutoConnect);
    ::fluid_settings_setstr(m_pFluidSettings, "audio.jack.multi", (char *) (bJackMulti ? "yes" : "no"));
    ::fluid_settings_setstr(m_pFluidSettings, "audio.sample-format", (char *) sSampleFormat.latin1());

    if (iAudioChannels > 0)
        ::fluid_settings_setint(m_pFluidSettings, "synth.audio-channels", iAudioChannels);
    if (iAudioGroups > 0)
        ::fluid_settings_setint(m_pFluidSettings, "synth.audio-groups", iAudioGroups);
    if (iAudioBufSize > 0)
        ::fluid_settings_setint(m_pFluidSettings, "audio.period-size", iAudioBufSize);
    if (iAudioBufCount > 0)
        ::fluid_settings_setint(m_pFluidSettings, "audio.periods", iAudioBufCount);
    if (fSampleRate > 0.0)
        ::fluid_settings_setnum(m_pFluidSettings, "synth.sample-rate", fSampleRate);
    if (iPolyphony > 0)
        ::fluid_settings_setint(m_pFluidSettings, "synth.polyphony", iPolyphony);
    if (fGain > 0.0)
        ::fluid_settings_setnum(m_pFluidSettings, "synth.gain", fGain);

    ::fluid_settings_setstr(m_pFluidSettings, "synth.reverb.active", (char *) (bReverbActive ? "yes" : "no"));
    ::fluid_settings_setstr(m_pFluidSettings, "synth.chorus.active", (char *) (bChorusActive ? "yes" : "no"));
    ::fluid_settings_setstr(m_pFluidSettings, "synth.ladspa.active", (char *) (bLadspaActive ? "yes" : "no"));
    ::fluid_settings_setstr(m_pFluidSettings, "synth.dump",          (char *) (bMidiDump     ? "yes" : "no"));
    ::fluid_settings_setstr(m_pFluidSettings, "synth.verbose",       (char *) (bVerbose      ? "yes" : "no"));
}


//---------------------------------------------------------------------------
// Combo box history persistence helper implementation.

void qsynthSetup::add2ComboBoxHistory ( QComboBox *pComboBox, const QString& sNewText, int iLimit, int iIndex )
{
    int iCount = pComboBox->count();
    for (int i = 0; i < iCount; i++) {
        QString sText = pComboBox->text(i);
        if (sText == sNewText) {
            pComboBox->removeItem(i);
            iCount--;
            break;
        }
    }
    while (iCount >= iLimit)
        pComboBox->removeItem(--iCount);
    pComboBox->insertItem(sNewText, iIndex);
}


void qsynthSetup::loadComboBoxHistory ( QComboBox *pComboBox, int iLimit )
{
    pComboBox->setUpdatesEnabled(false);
    pComboBox->setDuplicatesEnabled(false);

    m_settings.beginGroup("/" + QString(pComboBox->name()));
    for (int i = 0; i < iLimit; i++) {
        QString sText = m_settings.readEntry("/Item" + QString::number(i + 1), QString::null);
        if (sText.isEmpty())
            break;
        add2ComboBoxHistory(pComboBox, sText, iLimit);
    }
    m_settings.endGroup();

    pComboBox->setUpdatesEnabled(true);
}


void qsynthSetup::saveComboBoxHistory ( QComboBox *pComboBox, int iLimit )
{
    add2ComboBoxHistory(pComboBox, pComboBox->currentText(), iLimit, 0);

    m_settings.beginGroup("/" + QString(pComboBox->name()));
    for (int i = 0; i < iLimit && i < pComboBox->count(); i++) {
        QString sText = pComboBox->text(i);
        if (sText.isEmpty())
            break;
        m_settings.writeEntry("/Item" + QString::number(i + 1), sText);
    }
    m_settings.endGroup();
}


//---------------------------------------------------------------------------
// Widget geometry persistence helper methods.

void qsynthSetup::loadWidgetGeometry ( QWidget *pWidget )
{
    // Try to restore old form window positioning.
    if (pWidget) {
        QPoint fpos;
        QSize  fsize;
        bool bVisible;
        m_settings.beginGroup("/Geometry/" + QString(pWidget->name()));
        fpos.setX(m_settings.readNumEntry("/x", -1));
        fpos.setY(m_settings.readNumEntry("/y", -1));
        fsize.setWidth(m_settings.readNumEntry("/width", -1));
        fsize.setHeight(m_settings.readNumEntry("/height", -1));
        bVisible = m_settings.readBoolEntry("/visible", false);
        m_settings.endGroup();
        if (fpos.x() > 0 && fpos.y() > 0)
            pWidget->move(fpos);
        if (fsize.width() > 0 && fsize.height() > 0)
            pWidget->resize(fsize);
        else
            pWidget->adjustSize();
        if (bVisible)
            pWidget->show();
    }
}


void qsynthSetup::saveWidgetGeometry ( QWidget *pWidget )
{
    // Try to save form window position...
    // (due to X11 window managers ideossincrasies, we better
    // only save the form geometry while its up and visible)
    if (pWidget) {
        m_settings.beginGroup("/Geometry/" + QString(pWidget->name()));
        bool bVisible = pWidget->isVisible();
        if (bVisible) {
            QPoint fpos  = pWidget->pos();
            QSize  fsize = pWidget->size();
            m_settings.writeEntry("/x", fpos.x());
            m_settings.writeEntry("/y", fpos.y());
            m_settings.writeEntry("/width", fsize.width());
            m_settings.writeEntry("/height", fsize.height());
        }
        m_settings.writeEntry("/visible", bVisible);
        m_settings.endGroup();
    }
}


// end of qsynthSetup.cpp