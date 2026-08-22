# kurz vor acht – German Pebble Watchface

This is is a watchface for the Pebble smartwatch that shows the current time in German.
The watchface is based on [n3v3rstextone](https://github.com/n3v3r001/n3v3rstextone) by [@n3v3r001](https://github.com/n3v3r001) and [Deutsch](https://github.com/wolframroesler/Deutsch) by [@wolframroesler](https://github.com/wolframroesler), adjusted for modern pebbles while keeping compatibility with the originals.

![banner](pics/banner.jpg)

The time is shown in colloquial, "fuzzy" mode: We say "kurz vor acht" ("just before eight") e.g. when it's 7:58, but an exact mode is also available.

Several color themes are available (selected with the configuration dialog in your phone's Pebble app).

## Screenshots

| | | | | | | |
| --- | :---: | :---: | :---: | :---: | :---: | :---: | 
| **Pebble Classic/ Steel**<br>Aplite | ![Black theme right aligned](pics/aplite_black.png "Black theme right aligned") | ![White theme centered charging](pics/aplite_white_disconnected.png "White theme centered charging and disconnected") | ![Black theme with status symbols](pics/aplite_black_status.png "Black theme with status symbols") |
| **Pebble Time / Time Steel**<br>Basalt | ![Black theme right aligned](pics/basalt_black.png "Black theme right aligned") | ![Blue theme centered with low battery and disconnected bluetooth](pics/basalt_blue.png "Blue theme centered with low battery and disconnected bluetooth") | ![Green theme](pics/basalt_green.png "Green theme") | ![Red theme charging](pics/basalt_red.png "Red theme charging") | ![Gray theme with quick view](pics/basalt_gray_quick_view.png "Gray theme with quick view") | ![White theme](pics/basalt_white.png "White theme") |
| **Pebble Time Round**<br>Chalk | ![Black theme](pics/chalk_black.png "Black theme") | ![Blue theme in exact mode with warnings](pics/chalk_blue_status.png "Blue theme in exact mode with warnings") | ![Green theme](pics/chalk_green.png "Green theme") | ![Red theme charging](pics/chalk_red_charging.png "Red theme charging") | ![White theme](pics/chalk_white.png "White theme") |
| **Pebble&nbsp;2**<br>Diorite | ![Black theme right aligned](pics/diorite_black.png "Black theme right aligned") | ![White theme centered charging](pics/diorite_white_center.png "White theme centered charging") | ![White theme charging](pics/diorite_white_left.png "White theme charging") | ![Black theme with quick view](pics/diorite_black_quick_view.png "Black theme with quick view") | ![White theme with exact time](pics/diorite_white_exact.png "White theme with exact time") |
| **Pebble Time&nbsp;2**<br>Emery | ![Black theme right aligned](pics/emery_black.png "Black theme right aligned") | ![Blue theme with low battery](pics/emery_blue.png "Blue theme with low battery") | ![Green  theme](pics/emery_green.png "Green  theme") | ![Red theme with quick view](pics/emery_red_quick_view.png "Red theme with quick view") | ![Gray theme charging](pics/emery_gray_charging.png "Gray theme charging")| ![White theme](pics/emery_white.png "White theme") |
| **Pebble Round&nbsp;2**<br>Gabbro | ![Black theme](pics/gabbro_black.png "Black theme") | ![Blue theme with low battery](pics/gabbro_blue.png "Blue theme with low battery") | ![Green theme](pics/gabbro_green.png "Green theme") | ![Red theme charging with quick view](pics/gabbro_red_quick_view.png "Red theme charging with quick view") | ![Gray theme charging with Timeline Quick View](pics/gabbro_gray_quick_view_charging.png "Gray theme charging with Timeline Quick View")| ![White theme](pics/gabbro_white.png "White theme") |

## Installation

### [Pebble Appstore](https://apps.repebble.com/4151feede8864a94905355d0)

### Sideloading
To install the watchface on your Pebble, download the latest release from the GitHub Releases page and upload the generated `kurz-vor-acht.pbw` file using the Pebble app on your smartphone. If you prefer building it yourself, you can also build and install the watchface with the [Pebble SDK](https://developer.repebble.com/sdk/). Also check out the devcontainer and GitHub Actions setup in this repo to see how to install the SDK and build the watchface.

Releases are built automatically in GitHub Actions, so the latest packaged build is always available from the repository's Releases page.

## Changes

In addition to the [changes](https://github.com/wolframroesler/Deutsch) by [@wolframroesler](https://github.com/wolframroesler) I have made these further changes:

### Technical

* Update to Pebble SDK 4 to support Pebble 2, Pebble 2 Duo, Pebble Time 2 and Pebble Round 2
* Refactor layout logic to support an array of watch resolutions
* Recalculate the layout when Timeline events are obstructing the screen
* Added setup for DevContainer and GitHub Actions for automatic builds
* ~~Use GitHub Pages for the settings~~ Obsolete by move to Clay
* make the configuration page work in the cloudpebble and sdk emulator
* Update Configuration Page to use the clay library
* make bluetooth and battery icons work with color themes
* Make it available in the Pebble store

### Layout

* On larger displays the text is padded to the bottom right with some margin to the screen borders
* Added configuration option for rectangle displays for text alignment
* Configuration options to for bluetooth and battery to only show when relevant

## Planned future changes

* fix three line text on flint with quick view
* make low battery threshold configurable
    * maybe even whether icon is displayed during charging
* Option to create custom theme with custom colours
* reduce options for incapable watches (e.g. colours for b/w watches)
* reuse same battery assets with pallette inversion instead of having separate resources
