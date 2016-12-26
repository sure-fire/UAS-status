# UAS-status
Pebble smartwatch app to query the status of unallocatedspace.org

This is a Pebble app, written in a combination of C and JavaScript, which is largely based off the One-Click Action / Lockitron tutorial provided generously by the Pebble SDK devs.  It queries the [Unallocated Space](www.unallocatedspace.org/uas) website to find out whether the occupancy sensor has been triggered.

The app is my first, and was written for the older Pebble Steel platform (aka Basalt).  I have not tested it on other platforms, but it should work roughly the same way:

<kbd>
 ![Glance](docs/screenshot1-glance.png)
</kbd>

If the app hasn't been run recently, the glance will show "UNKNOWN" and the select button can be used to quickly check and report back the time of the last status change:

<kbd>
![Details](docs/screenshot2-details.png)
</kbd>
