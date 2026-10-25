# Windows users can eat dirt with this script

import os, sys
from PIL import Image

def toChars(value: int): # split 16 bit ints to 8 bit chars
    conv = []
    conv.append(value >> 8 & 0x00ff)
    conv.append(value & 0x00ff)
    out = ""
    for v in conv:
        out += chr(v)
    return out

def toChar(value: int):
    conv = []
    conv.append(value & 0x00ff)
    out = ""
    for v in conv:
        out += chr(v)
    return out


def main():
    launchDir = os.getcwd()

    os.chdir("./source/res")

    # convert images to bin files

    # open file
    imageFileNames = os.listdir("./raw_images")

    total = 0

    for fileName in imageFileNames:
        pallet = []
        colorTable = []
        image = Image.open("./raw_images/" + fileName)
        width, height = image.size
        print(width, height)

        for x in range(width):
            for y in range(height):
                color = image.getpixel((x, y))
                if color in pallet:
                    colorTable.append([i for i in range(len(pallet)) if pallet[i] == color][0])
                    print("a", [i for i in range(len(pallet)) if pallet[i] == color][0])
                else:
                    pallet.append(color)
                    colorTable.append(len(pallet) - 1)
                    print("b", len(pallet) - 1)

        if len(pallet) * 3 > 0xffff:
            print("image is too complicated, will not attempt")
            continue

        # First part will be header size
        # Next will be size of size
        # Next will be size
        # Next will be the pallet
        # Next will be colors

        with open("./images/" + fileName.split(".")[0], "w") as f:
            f.write("")
        with open("./images/" + fileName.split(".")[0], "ab") as f:
            writtenWidth = toChars(width).encode("utf-8")
            writtenHeight = toChars(height).encode("utf-8")

            f.write(toChars(len(pallet) * 3 + writtenWidth.__len__() + writtenHeight.__len__() + 2).encode("utf-8"))

            f.write(toChar(writtenWidth.__len__()).encode("utf-8"))
            f.write(toChar(writtenHeight.__len__()).encode("utf-8"))
            
            f.write(writtenWidth)
            f.write(writtenHeight)

            for color in pallet:
                f.write(toChar(color[0]).encode("utf-8"))
                f.write(toChar(color[1]).encode("utf-8"))
                f.write(toChar(color[2]).encode("utf-8"))

            for element in colorTable:
                f.write(toChar(element).encode("utf-8"))
        total += 1
    print("Completed", total, "of", len(imageFileNames), "images")

main()