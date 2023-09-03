# DisplayCal-HDR-Toggler
Toggle DisplayCal based on Windows HDR status

DisplayCal works wonders in SDR mode, but when switching a display to HDR mode - the same profile gets applied, which should not be default behaviour.
This causes issues with colors and messes up the PQ curve.

This app periodically checks for which mode the main display is in, and switches DisplayCal on if SDR mode is on, or off if HDR mode is detected.

Currently, 'simple mode' is enabled by default, which uses screen coordinates to click on the DisplayCal Profile Loader and switch it on or off.
This is the case because DisplayCal's API/Scripting functions tend to malfunction often and not expose the required methods to do this via code.
To set the coordinates for your setup, change these definitions:
DISPCAL_ICON_LOC - pixel location of the DisplayCal icon
DISPCAL_LOAD_CAL_LOC - pixel location of the DisplayCal right-click menu "Load calibration" function
DISPCAL_RESET_TABLE_LOC - pixel location of the DisplayCal right-click menu "reset VCGT" function
