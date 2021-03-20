#!/usr/bin/python3

import os
import shutil
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


def make_archive(source, destination):
    base = os.path.basename(destination)
    name = base.split('.')[0]
    format = base.split('.')[1]
    archive_from = os.path.dirname(source)
    archive_to = os.path.basename(source.strip(os.sep))
    print(source, destination, archive_from, archive_to)
    shutil.make_archive(name, format, archive_from, archive_to)
    shutil.move('%s.%s' % (name, format), destination)


for folder in os.listdir(path):
    if os.path.isfile(os.path.join(path, folder)):
        continue

    make_archive(os.path.join(path, folder), os.path.join(out, folder) + ".zip")
