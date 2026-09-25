# AltoidOS App (AAP) Format

## Overview

An .aap file contains an executable AltoidOS application.

## Layout

| Offset | Size | Field              |
|-------:|------|--------------------|
| 0x00   | 4    | Magic              |
| 0x04   | 2    | Format Version     |
| 0x06   | 4    | Entry Point Offset |
| 0x0A   | 4    | Program Size       |
| 0x0E   | ...  | Code               |