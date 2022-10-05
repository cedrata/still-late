# This script will automatically generate the file Source/ directory files list to add in the CMake file.
# To run this script simply point inside this directory ```cd /path/to/pamplejuce/Scripts```.
# Type then ```$ python3 cmake_lists_init.py``` to generate the CMakeFile with the Source directory files list.
from glob import iglob
from os.path import abspath, isdir
from re import sub

# Read all files inside "Source" directory.
source_dir: str = abspath("../Source")

source_list: list = []
source_files: str = "\n\t"

path_to_remove = f"{abspath('..')}/"

for filename in iglob(source_dir + "/**", recursive=True):

    # The requirement is to list all the files (and not the directories) recursively in the source_dir.
    if isdir(filename):
        continue

    # If is a file add it to the string.
    # Keep the relative name.
    source_list.append(sub(path_to_remove, "", filename))

source_list.sort()
source_files = "\n\t" + "\n\t".join(source_list)
# source_files = "\n\n" + source_files

# Read CMakeLists.txt to replace.
# Search for all of the regex pattern and replace them with the correct values.

# Source files pattern.
cmake_input = abspath("../CMakeLists.txt.source")

with open(cmake_input, "r") as f:
    cmake_content = f.read()

cmake_content = sub("(%<\w+>)", source_files, cmake_content)

# Print the cmake.
cmake_output = abspath("../CmakeLists.txt")
with open(cmake_output, "w") as f:
    f.write(cmake_content)
