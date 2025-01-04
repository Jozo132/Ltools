# Transfer *.ttf files into a single CPP header file named "precompiled_fonts.h" in the include directory.
# Usage: python compile_fonts.py

import os
import sys

# Font directory
FONT_DIR = "./resources/"

# Output header file
HEADER_FILE = "./resources/precompiled_fonts.h"


def main():
    # Load all *.ttf files in the font directory
    fonts = []
    for root, _, files in os.walk(FONT_DIR):
        for file in files:
            if file.lower().endswith(".ttf"):
                with open(os.path.join(root, file), "rb") as f:
                    data = f.read()
                    fonts.append((file, len(data), data))
                
    # Write to the header file
    with open(HEADER_FILE, "w") as f:
        for i in range(0, len(fonts)):
            name, length, data = fonts[i]
            # if i != 1: continue
            name_without_postfix = name.split(".")[0]
            f.write("    { .name = \"%s\", .length = %d, " % (name_without_postfix, length))
            f.write(".data = (const uint8_t[]) {")
            for i in range(0, length):
                f.write("0x%02X, " % data[i])
            f.write("} },\n")
    print("Fonts compiled successfully!")
    return 0

if __name__ == "__main__":
    sys.exit(main())