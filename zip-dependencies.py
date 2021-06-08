#!/usr/bin/python3


import os
import zipfile
import shutil


def make_archive(source, destination):
    destination = os.path.join(destination, source)
    destination = os.path.abspath(destination)
    if not os.path.isdir(destination):
        os.makedirs(destination)

    source = os.path.abspath(source)

    for folder in os.listdir(source):
        if os.path.isfile(os.path.join(source, folder)):
            continue

        zip_file = zipfile.ZipFile(os.path.join(destination, folder) + ".zip", 'w', zipfile.ZIP_DEFLATED)

        for root, dirs, files in os.walk(os.path.join(source, folder)):
            for filename in files:
                zip_file.write(os.path.abspath(os.path.join(root, filename)), arcname=filename)
        zip_file.close()


if os.path.isdir("tmp"):
    shutil.rmtree("tmp")
make_archive("programs", "tmp")
make_archive("assets", "tmp")
