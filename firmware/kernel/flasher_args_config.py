import json
import os

Import("env")

build_dir = env.subst("$BUILD_DIR")

flasher_args = {
    "write_flash_args": [
        "--flash-mode", "dio",
        "--flash-size", "4MB",
        "--flash-freq", "40m"
    ],
    "flash_settings": {
        "flash_mode": "dio",
        "flash_size": "4MB",
        "flash_freq": "40m"
    },
    "flash_files": {
        "0x1000": "bootloader.bin",
        "0x8000": "partitions.bin",
        "0x10000": "firmware.bin"
    },
    "bootloader": {
        "offset": "0x1000",
        "file": "bootloader.bin",
        "encrypted": "false"
    },
    "partition-table": {
        "offset": "0x8000",
        "file": "partitions.bin",
        "encrypted": "false"
    },
    "app": {
        "offset": "0x10000",
        "file": "firmware.bin",
        "encrypted": "false"
    },
    "extra_esptool_args": {
        "after": "hard-reset",
        "before": "default-reset",
        "stub": True,
        "chip": "esp32"
    }
}

with open(os.path.join(build_dir, "flasher_args.json"), "w") as f:
    json.dump(flasher_args, f, indent=2)

print("Fixed flasher_args.json")