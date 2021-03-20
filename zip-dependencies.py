#!/usr/bin/python3

import os
import zipfile
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-i", "--in", dest="input", help="Folder to zip into ./tmp")
parser.add_option("-o", "--out", dest="output", help="Output directory")
(options, args) = parser.parse_args()

path = options.input
out = options.output

out = os.path.join(out, path)
out = os.path.abspath(out)
if not os.path.isdir(out):
    os.makedirs(out)

path = os.path.abspath(path)


for folder in os.listdir(path):
    if os.path.isfile(os.path.join(path, folder)):
        continue

    zipf = zipfile.ZipFile(os.path.join(out, folder)+".zip", 'w', zipfile.ZIP_DEFLATED)

    for root, dirs, files in os.walk(os.path.join(path, folder)):
        for filename in files:
            zipf.write(os.path.abspath(os.path.join(root, filename)), arcname=filename)
    zipf.close()
