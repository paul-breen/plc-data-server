###############################################################################
# PROJECT:  PDS Utilities
# MODULE:   pds-scope.py
# PURPOSE:  Oscilloscope to display the signal of the given tag
# AUTHOR:   Paul M. Breen
# DATE:     2018-07-21
###############################################################################

import argparse

from matplotlib import pyplot as plt
from matplotlib import animation

import pds.pds as pds

class PdsScope(object):
    def __init__(self, tagname, npoints=80, ylim=(), offset=0, scale_factor=1,
                 interval=200, style=None, show_grid=True):
        self.store_conf(npoints=npoints, ylim=ylim, offset=offset, scale_factor=scale_factor, interval=interval, style=style, show_grid=show_grid)
        self.tagname = tagname
        self.conn = None
        self.tag = None
        self.fig = None
        self.line = None
        self.data = {'x': [], 'y': []}

    def store_conf(self, **kwargs):
        self.conf = kwargs

    def init_plot(self):
        if self.conf['style']:
            plt.style.use(self.conf['style'])

        self.fig, self.ax = plt.subplots()
        self.ax.set_xlim(0, self.conf['npoints'])
        plt.title(self.tagname)

        # If not explicitly set, we autoscale the y-axis
        if self.conf['ylim']:
            self.ax.set_ylim(self.conf['ylim'])

        self.ax.grid(b=self.conf['show_grid'])
        self.line, = self.ax.plot(self.data['x'], self.data['y'])

        return self.line,

    def update(self, data):
        self.line.set_xdata(self.data['x'])
        self.line.set_ydata(self.data['y'])

        # canvas.draw is expensive, so we only call it if we have to autoscale
        if not self.conf['ylim']:
            self.ax.relim()
            self.ax.autoscale_view(scalex=False)
            self.fig.canvas.draw()
            self.fig.canvas.flush_events()

        return self.line,

    def process_tag(self):
        # Used to ensure the latest data scrolls in the plotting area
        fixed_x = [i for i in range(self.conf['npoints'])]
        i = 0

        self.conn = pds.PDSconnect(pds.PDS_IPCKEY)

        if self.conn.conn_status != pds.PDS_CONN_OK:
            raise ValueError('Error connecting to the PDS: {}'.format(self.conn.status))

        self.tag = pds.PDSget_tag_object(self.conn, self.tagname)

        while True:
            try:
                self.data['x'].append(i)
                i += 1

                y = int(self.tag.value) * self.conf['scale_factor'] + self.conf['offset']
                self.data['y'].append(y)

                # Once we've drawn to the RHS, we start scrolling
                if len(self.data['x']) > self.conf['npoints']:
                    self.data['x'] = fixed_x[:]
                    self.data['y'] = self.data['y'][1:]

                # We must yield an iterable, hence this tuple
                yield self.data,

            except KeyboardInterrupt:
                print('Keyboard Interrupt detected: exiting...')
                break

        pds.PDSdisconnect(self.conn)

    def run(self):
        self.anim = animation.FuncAnimation(self.fig, self.update, frames=self.process_tag, blit=True, interval=self.conf['interval'])
        plt.show()

def parse_cmdln():
    epilog = """Examples

Say we have a sine wave (in a tag called 'sine'), offset by 1, to make all
samples non-negative, and scaled by 1000 to make the samples integer.
Passing the offset (-o) and scale-factor (-f) options normalises the sine
wave to [-1,1].  We set fixed y-axis limits (-y), as autoscaling the y-axis
incurs overhead so it's best to avoid it if we know the function domain in
advance, as we do here:

python3 pds-scope.py -o -1 -f 0.001 -y -1.1 1.1 sine

The same, but with a higher sampling rate by setting a smaller interval
between samples (-i):

python3 pds-scope.py -o -1 -f 0.001 -y -1.1 1.1 -i 40 sine

Turn the grid off (-G) and set the style (-s) to a light signal on a dark
background:

python3 pds-scope.py -o -1 -f 0.001 -y -1.1 1.1 -i 40 sine -G -s dark_background
"""

    parser = argparse.ArgumentParser(description='oscilloscope to display the given tag', epilog=epilog, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('tagname', help='name of the tag to display')

    parser.add_argument('-p', '--npoints', help='number of points N to display', metavar='N', type=int, default=80)
    parser.add_argument('-y', '--ylim', help='limits of the y-axis MIN MAX', metavar='N', type=float, nargs=2)
    parser.add_argument('-o', '--offset', help='offset N to add to a displaced signal', metavar='N', type=float, default=0)
    parser.add_argument('-f', '--scale-factor', help='factor N to multiply a scaled signal by', metavar='N', type=float, default=1)
    parser.add_argument('-i', '--interval', help='interval N in ms between samples being taken of the signal', metavar='N', type=int, default=200)
    parser.add_argument('-s', '--style', help='stylesheet STYLE to apply to the plotting area, see the matplotlib stylesheets documentation for options', metavar='STYLE', type=str)

    group = parser.add_mutually_exclusive_group()
    group.add_argument('-g', '--show-grid', help='show a grid in the plotting area', action='store_true', default=True)
    group.add_argument('-G', '--no-show-grid', help='don\'t show a grid in the plotting area', action='store_false', default=False, dest='show_grid')

    args = parser.parse_args()

    return args

if __name__ == '__main__':
    args = parse_cmdln()

    scope = PdsScope(args.tagname, npoints=args.npoints, ylim=args.ylim, offset=args.offset, scale_factor=args.scale_factor, interval=args.interval, show_grid=args.show_grid, style=args.style)
    scope.init_plot()
    scope.run()

    exit(0)

