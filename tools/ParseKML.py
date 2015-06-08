#
# Parses a KML file from Google Earth to generate a text file of Waypoints in the format
# that is read by MeagBotics::Route class.
#
# The output file consists of lines with LAT LON pairs separated by a space
#
# A single command line argument can be passed that is the name of the input KML file
# The output file is generated in the same directory with 'WP-' prefix and '.TXT' suffix
#

__author__ = 'sraj'
from pykml import parser
import sys
import os

if __name__ == "__main__":
    if len(sys.argv) < 2:
        kmlFile = 'Waypoint Files/SMOKY.KML'
    else:
        kmlFile = sys.argv[1]

    dirName = os.path.dirname(kmlFile)
    baseName = os.path.basename(kmlFile)

    outFile = dirName + '/WP-' + baseName.replace(".KML", ".TXT")
    f = open(outFile, 'w')
    root = parser.fromstring(open(kmlFile, 'r').read())
    for place in root.Document.Folder.Placemark:
        mylist = place.Point.coordinates.text.split(",")
        f.write("%s %s\n" % (mylist[1], mylist[0]))
    f.close()
