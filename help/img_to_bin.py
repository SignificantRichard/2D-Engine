import os
from PIL import Image

def toChars(value: int): # split 16 bit ints into 2 raw bytes
    return bytes([(value >> 8) & 0xff, value & 0xff])

def toChar(value: int): # 8 bit int into 1 raw byte
    return bytes([value & 0xff])

def main():
    launchDir = os.getcwd()
    os.chdir("./source/res")

    imageFileNames = os.listdir("./raw_images")
    total = 0

    for fileName in imageFileNames:
        if not fileName.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp')):
            continue
            
        pallet = []
        colorTable = []
        image = Image.open("./raw_images/" + fileName).convert("RGB")
        width, height = image.size
        print(f"Processing {fileName}: {width}x{height}")

        for y in range(height):
            for x in range(width):
                color = image.getpixel((x, y))
                if color in pallet:
                    colorTable.append(pallet.index(color))
                else:
                    pallet.append(color)
                    colorTable.append(len(pallet) - 1)

        if len(pallet) * 3 > 0xffff:
            print("Image is too complicated, will not attempt")
            continue

        out_filename = "./images/" + fileName.split(".")[0]
        
        # Open file in binary write mode ('wb')
        with open(out_filename, "wb") as f:
            writtenWidth = toChars(width)
            writtenHeight = toChars(height)

            header_size = len(pallet) * 3 + len(writtenWidth) + len(writtenHeight) + 2
            f.write(toChars(header_size))

            f.write(toChar(len(writtenWidth)))
            f.write(toChar(len(writtenHeight)))
            
            f.write(writtenWidth)
            f.write(writtenHeight)

            for color in pallet:
                f.write(toChar(color[0]))
                f.write(toChar(color[1]))
                f.write(toChar(color[2]))

            for element in colorTable:
                f.write(toChar(element))
                
        total += 1
        
    print(f"Completed {total} of {len(imageFileNames)} images")

if __name__ == "__main__":
    main()