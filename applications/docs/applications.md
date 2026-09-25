# AltoidOS Applications

## Overview

AltoidOS Applications are external programs (and their metadata and required files) that can be installed and run on any Altoidometer.

## Metadata

The `app.json` file within an app's directory contains its metadata. It specifies useful user-facing information about the app, which is as follows:

| Field       | Purpose                                     | Specifications                 |
|-------------|---------------------------------------------|--------------------------------|
| name        | The application's title.                    | ...                            |
| version     | The application's software version.         | Uses standard Semantic Versioning: `<major>.<minor>.<patch>` |
| description | A brief summary of the app's functionality. | ...                            |
| author      | The person (or group) that created the application. | If the app was made by one person, this should be their GitHub username. If it was made by a group, use the group's common name. |
| icon        | The path to the app's icon file.            | The icon should be a 10x10 .bmp located somewhere within `<app-dir>/data/`. Path is relative to the app's directory. |
| permissions | A list of permissions required by the application. | See below               |

## Permissions

An app's permissions specify various aspects of what it is allowed to do and change on your Altoidometer. Default apps come with their permissions already granted for ease of use, but apps installed later must request certain more elevated permissions and have them granted by the user. The permissions listed in the app's metadata are what is **required** by the app, not what permissions have been **granted**.

| AltoidOS Permissions                                                                      ||
|--------------:|----------------------------------------------------------------------------|
| `disp`        | Allows the app to interact with the display. Granted by default to all apps. |
| `io`          | Allows the app to read the button keypad. Granted by default to all apps.  |
| `fs`          | Allows the app to read and write the device's SD card.                     |
| `scope`       | Allows the app to read the device's two oscilloscope ADCs.                 |
| `expio`       | Allows the app to read and write the device's expansion GPIO ports.        |