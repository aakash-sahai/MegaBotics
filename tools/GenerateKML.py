#
# Reads the LOG file generated by the MegaBotics SmartRover family of classes and generates a KML file that
# can be read by Google Earth to display the route taken by the Rover as it navigates from one waypoint to
# the next.
#
# This program takes two command line arguments. First is the name assigned to a set of runs whose logs are
# stored in the input LOG file - it is used as the name of the sub-directory under which the generated KML
# files are stored.  The second argument is the name of the input LOG file.
#
# If the LOG file contains multiple runs, it is broken up into multiple LOG files and corresponding  KML file.
#

__author__ = 'sraj'
from lxml import etree
from pykml.parser import Schema
from pykml.factory import KML_ElementMaker as KML
from pykml.factory import GX_ElementMaker as GX
import re
import sys
import shutil
import os


def split_file(run_name, file_name):
    dest = None
    no_of_runs = 0
    dirName = os.path.dirname(file_name) + "/" + run_name

    if not os.path.exists(dirName):
        os.makedirs(dirName)

    input = open(file_name, 'r')
    for line in input:
        if re.search('OPEN', line):
            if dest:
                dest.close()
            dest = open(dirName + "/LOG_" + str(no_of_runs) + '.TXT', 'w')
            no_of_runs += 1
        if None != dest:
            dest.write(line)
    return (no_of_runs, dirName)


def create_placemark(lat, lon, lines, style):
    desc = ''.join(lines)
    if style == '#pushpin' or style == '#waypoint':
        desc = desc.replace(",", "\n")

    pm = KML.Placemark(
        # KML.name("{0}".format(i)),
        KML.styleUrl(style),
        KML.description(desc),
        KML.LookAt(
            KML.longitude(lon),
            KML.latitude(lat),
            KML.altitude(10),
            KML.heading(0),
            KML.tilt(0),
            KML.roll(0),
            KML.altitudeMode("relativeToGround"),
            KML.range(50),
        ),
        KML.Point(
            KML.extrude(True),
            KML.altitudeMode('relativeToGround'),
            KML.coordinates("{0},{1},{2}".format(lon, lat, 0))
        ),
    )
    return pm


def create_KML(dir_name, run_number):
    lat = 0
    lon = 0
    first = True
    steer = ''
    lines = []
    kml_name = dir_name + '/KML_' + str(run_number)

    doc = KML.kml(
        KML.Document(
            KML.name(kml_name),
            KML.Style(
                KML.IconStyle(
                    KML.scale(1.0),
                    KML.Icon(
                        KML.href("http://maps.google.com/mapfiles/kml/shapes/placemark_circle.png"),
                    ),
                    id="mystyle"
                ),
                id="pushpin"
            ),
            KML.Style(
                KML.IconStyle(
                    KML.scale(1.5),
                    KML.Icon(
                        KML.href("http://maps.google.com/mapfiles/kml/paddle/red-circle.png"),
                    ),
                    id="mystyle"
                ),
                id="waypoint"
            ),
            KML.Style(
                KML.IconStyle(
                    KML.scale(1.5),
                    KML.Icon(
                        KML.href("http://maps.google.com/mapfiles/kml/paddle/S.png"),
                    ),
                    id="mystyle"
                ),
                id="start"
            ),
            KML.Style(
                KML.IconStyle(
                    KML.scale(1.5),
                    KML.Icon(
                        KML.href("http://maps.google.com/mapfiles/kml/paddle/E.png"),
                    ),
                    id="mystyle"
                ),
                id="end"
            )
        )
    )

    for line in open(dir_name + "/LOG_" + str(run_number) + '.TXT', 'r'):
        if re.search('RUN-STEER', line):
            if lat:
                if first:
                    pm = create_placemark(lat, lon, lines, '#start')
                    doc.Document.append(pm)
                    first = False
                else:
                    pm = create_placemark(lat, lon, lines, '#pushpin')
                    doc.Document.append(pm)

                del lines[:]
                lines.append(line)
        else:
            if re.search('RUN-THROTTLE', line):
                m = re.search('.*LAT:([^\s,]+).*LONG:([^\s,]+).*', line)
                lat = m.group(1)
                lon = m.group(2)
            lines.append(line)

        # reached waypoint
        if re.search('Reached Waypoint', line):
            m = re.search('.*Reached Waypoint[^:]+:([0-9]+).*', line)
            wpnum = m.group(1)
            pm = create_placemark(lat, lon, lines, '#waypoint')
            doc.Document.append(pm)

    pm = create_placemark(lat, lon, lines, '#end')
    doc.Document.append(pm)

    print etree.tostring(doc, pretty_print=True)

    # output a KML file (named based on the Python script)
    outfile = file(kml_name + '.KML', 'w')
    outfile.write(etree.tostring(doc, pretty_print=True))

    return


if __name__ == "__main__":
    if len(sys.argv) < 2:
        runName = 'TestRun'
    else:
        runName = sys.argv[1]

    if len(sys.argv) < 3:
        logFile = 'LOG Files/LOG.TXT'
    else:
        logFile = sys.argv[2]

    no_of_runs, dirName = split_file(runName, logFile)
    for run_number in range(0, no_of_runs, 1):
        create_KML(dirName, run_number)
