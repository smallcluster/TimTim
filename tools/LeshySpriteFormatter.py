import sys
import json
from sys import argv

def remove_digits(string):
    s = string
    for d in "0123456789":
        s = s.replace(d, "")
    return s

if __name__ == "__main__":
    
    if len(argv) < 2:
        print("USAGE: py LeshySpriteFormatter.py path_to_data.txt ...")
        sys.exit(1)
    
    src_paths = [argv[i] for i in range(1, len(argv))]
    
    for path in src_paths:
        # parse txt file
        animations = {}
        lines = []

        with open(path, "r") as src:
            lines = src.readlines()
        
        for line in lines:
            if line.strip() == "":
                continue
            data = line.split(",")
            name = remove_digits(data[0])
            x = int(data[1])
            y = int(data[2])
            width = int(data[3])
            height = int(data[4].strip())
            if not name in animations.keys():
                animations[name] = []
            animations[name].append({"x":x, "y":y, "width":width, "height":height})
        
        dest = path
        if dest.endswith(".txt"):
            dest = ""
            s = path.split(".")[:-1]
            for p in s:
                dest += p +"."
            dest += "json"
        else:
            dest += ".json"

        with open(dest, "w") as out:
            json.dump(
            {"animations" : [
                {"name" : name,
                    "frames" : animations[name]
                } 
                 for name in animations.keys()
                ]}
                , out)