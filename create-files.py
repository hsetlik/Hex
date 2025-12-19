#!/usr/bin/python3
import os
import argparse

#========================================================
# get the existing project's name
def cmake_get_project_name(path_name):
    f = open(path_name, "r")
    lines = f.readlines()
    f.close()
    for line in lines:
        pos = line.find("project(")
        if(pos != -1):
            last_pos = line.find(" ", pos + 8)
            return line[pos + 8:last_pos]
    return "null"

# this should be called before adding the .cpp file
def add_cpp_header(cmake_path, src_path):
    plugin_name = cmake_get_project_name(cmake_path)
    plugin_dir = "plugin/include/" + plugin_name + "/"
    include_line = "\t\t\t\t${INCLUDE_DIR}/" + src_path + "\n";
    # load the cmake file as an array of lines
    f_read = open(cmake_path, "r")
    lines = f_read.readlines()
    f_read.close()
    include_added = False
    src_list_found = False
    idx = 0
    while idx < len(lines) and not include_added:
        if not src_list_found:
            pos = lines[idx].find("PluginProcessor.h")
            src_list_found = (pos != -1)
        else:
            lines.insert(idx - 1, include_line)
            include_added = True
        idx += 1
    # write it back to the cmake file
    f_write = open(cmake_path, "w")
    for l in lines:
        f_write.write(l)
    f_write.close()
    # create the source file and add its boilerplate
    f_src = open(plugin_dir + src_path, "w");
    f_src.write("# pragma once")
    f_src.close()

#note: src_name be just the file name
# argument without extensions or any pathname stuff
def add_cpp_source(cmake_path, src_name):
    include_line = "\t\t\t\tsource/" + src_name + ".cpp\n"
    # load the cmake file as an array of lines
    f_read = open(cmake_path, "r")
    lines = f_read.readlines()
    f_read.close()
    include_added = False
    header_inc_found = False
    idx = 0
    while idx < len(lines) and not include_added:
        if not header_inc_found:
            pos = lines[idx].find(src_name)
            header_inc_found = (pos != -1)
        else:
            lines.insert(idx - 1, include_line)
            include_added = True
        idx += 1
    # write it back to the cmake file
    f_write = open(cmake_path, "w")
    for l in lines:
        f_write.write(l)
    f_write.close()
    #create the file
    f_src = "plugin/source/" + src_name + ".cpp"
    file = open(f_src, "w")
    file.write("//===================================================")
    file.close()



#========================================================

parser = argparse.ArgumentParser(description="Create new header/source files")
parser.add_argument("src_path", help="Location and name for the new header file")
parser.add_argument('-ho', '--header_only', action='store_true')
args = parser.parse_args()

parent_path = args.src_path
path_to_create = ""
pos = parent_path.find("/")
while pos != -1:
    # grip the new path segment
    path_to_create += parent_path[0:pos + 1]
    # trim the parent path
    parent_path = parent_path[pos + 1:]
    # find the next delimiter
    pos = parent_path.find("/")

# make sure any relevant subdirectories exist
cwd = os.getcwd()
inc_path = os.path.join(cwd, "plugin/include/" + cmake_get_project_name("plugin/CMakeLists.txt"))
dir_path = os.path.join(inc_path, path_to_create)
print(f"Enclosing folder is: {dir_path}")
os.makedirs(dir_path, exist_ok=True)

# create the header file
header_name = args.src_path + ".h"
print(f"Creating header file: {header_name}")
add_cpp_header("plugin/CMakeLists.txt", header_name)

# create the source file if needed
if not args.header_only:
    add_cpp_source("plugin/CMakeLists.txt", parent_path)

print("File creation finished")




