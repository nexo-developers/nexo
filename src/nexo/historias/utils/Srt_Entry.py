
from __future__ import division
import datetime

class Srt_entry(object):
    def __init__(self, lines):
        def parsetime(string):
            hours, minutes, seconds = string.split(u':')
            hours = int(hours)
            minutes = int(minutes)
            seconds = float(u'.'.join(seconds.split(u',')))
            return datetime.timedelta(0, seconds, 0, 0, minutes, hours)
        self.index = int(lines[0])
        start, arrow, end = lines[1].split()
        self.start = parsetime(start)
        if arrow != u"-->":
            raise ValueError
        self.end = parsetime(end)
        self.lines = lines[2:]
        if not self.lines[-1]:
            del self.lines[-1]
    def __unicode__(self):
        def delta_to_string(d):
            hours = (d.days * 24) \
                    + (d.seconds // (60 * 60))
            minutes = (d.seconds // 60) % 60
            seconds = d.seconds % 60 + d.microseconds / 1000000
            return u','.join((u"%02d:%02d:%06.3f"
                              % (hours, minutes, seconds)).split(u'.'))
        return (unicode(self.index) + u'\n'
                + delta_to_string(self.start)
                + ' --> '
                + delta_to_string(self.end) + u'\n'
                + u''.join(self.lines))


srt_file = open("../14Blades.srt")
entries = []
entry = []
for line in srt_file:
   # if options.decode:
    #    line = line.decode(options.decode)
    #if line == u'\n':
    entries.append(Srt_entry(entry))
    entry = []
   # else:
    #    entry.append(line)
srt_file.close()

        