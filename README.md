# German Pebble Watchface

This is is a watchface for the Pebble smartwatch that shows the current time in German.

![Pebble Time mockup](pics/mockup1.jpg)
![Pebble Time Steel mockup](pics/mockup2.jpg)
![Pebble Steel mockup](pics/mockup3.jpg)

The time is shown in colloquial, "fuzzy" mode: We say "Viertel nach 10" when it's only 10:12 or already 10:17.

Several color themes are available (selected with the configuration dialog in your phone's Pebble app). More to be added on request, or when I get more shirts.

![Blue theme](pics/blue.jpg)
![Red theme](pics/red.jpg)
![Gray theme](pics/gray.jpg)

A battery icon is displayed in the upper left corner when charge drops to 10 % or below. A Bluetooth icon is displayed in the upper right corner when the Bluetooth connection is lost. To give the watchface a clean, minimalistic look, both icons are shown only when necessary.

![Battery and Bluetooth icons](pics/icons.png)

The watchface is based on https://github.com/n3v3r001/n3v3rstextone by [@n3v3r001](https://github.com/n3v3r001) and the [fork](https://github.com/wolframroesler/Deutsch) by [@wolframroesler](https://github.com/wolframroesler) adjusted for modern pebbles.

To install the watchface on your Pebble, download the latest release from the GitHub Releases page and upload the generated `Deutsch.pbw` file using the Pebble app on your smartphone. If you prefer building it yourself, you can also build and install the watchface with the Pebble SDK as described on https://developer.rebble.io/developer.pebble.com/tutorials/watchface-tutorial/part1/index.html. Check out the devcontainer and Actions setup in this repo to see how to install the SDK and build the watchface.

Releases are built automatically in GitHub Actions, so the latest packaged build is always available from the repository's Releases page.

## Changes

In addition to the [changes](https://github.com/wolframroesler/Deutsch) by [@wolframroesler](https://github.com/wolframroesler) I have made these additions:

### Technical

* Update to Pebble SDK 4 to support Pebble 2, Pebble 2 Duo, Pebble Time 2 and Pebble Round 2
* Refactor layout logic to support an array of watch resolutions
* Recaclulate the layout when Timeline events are obstructing the screen
* Added setup for DevContainer and GitHub Actions for automatic builds
* Use GitHub Pages for the settings

### Layout

* On larger displays the text is padded to the bottom right with some margin to the screen borders

## Planned future changes

* Configuration options to enable/disable the changes [@wolframroesler](https://github.com/wolframroesler) made to the original
* make the configuration work in the cloudpebble emulator
* update the configuration design to pebbles default template and improve saving ux
* Make it available in the Pebble store
