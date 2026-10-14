from PIL import Image
import os, os.path

imageSize = 0
imageFileName = 1

# root should be the pwd

def main():
    numImages = len(os.listdir("source/res/images"))
    totalSize = 0
    headData = []
    
    images = os.listdir("source/res/images")
    
    for imageFn in images:
        with Image.open("source/res/images/" + imageFn) as image:
            image.load()
            headData.append((totalSize, imageFn))
            totalSize += image.size[0] * image.size[1]

    with open("source/res/imageBlock.h", "w") as f:
        content = "// Image data is compiled into a bin here.\n// Nothing should be written directly as this is written to by a python script\n"
        content += "struct ImageData={\n"

        offset = 0
        # body data
        for data in headData:
            offset += data[imageSize]
            content += f"\timageData.{data[imageFileName].replace(".", "_")}={offset}\n"
        
        content += "};\n"
        content += "void _initFileData();\n"
        f.write(content)

    
    with open("source/res/imageBlock.c", "w") as f:
        content = "// Image data is compiled into a bin here.\n// Nothing should be written directly as this is written to by a python script\n"
        content += "void _initFileData(){\n"
        content += "ImageData imageData;\n"
        
        offset = 0
        # body data
        for data in headData:
            offset += data[imageSize]
            content += f"imageData.{data[imageFileName].replace(".", "_")}={offset}\n"

        content += "\n}\n"
        f.write(content)
    
    return 0

main()