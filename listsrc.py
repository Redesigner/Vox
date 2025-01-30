import os.path

currentDir = os.path.dirname(os.path.realpath(__file__))
dirIndent = len(currentDir.split('\\'))
for root, dirs, files in os.walk(currentDir + '/src'):
    path = root.split(os.sep)
    for file in files:
        if (file.endswith('.cpp')):
            print("\"src/" + '/'.join(path[dirIndent:]) + '/' + file + '"')