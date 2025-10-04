#!/usr/bin/env python3
# a small script to transmit simulated GPS samples via UHD
# (C) 2015 by Harald Welte <laforge@gnumonks.org>
# Licensed under the MIT License (see LICENSE)

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import time

class top_block(gr.top_block):
    def __init__(self, options):
        gr.top_block.__init__(self, "GPS-L1C-SDR-SIM")

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_sink = uhd.usrp_sink(options.args, uhd.stream_args(cpu_format="fc32"))
        self.uhd_usrp_sink.set_samp_rate(options.sample_rate)
        self.uhd_usrp_sink.set_center_freq(options.frequency, 0)
        self.uhd_usrp_sink.set_gain(options.gain, 0)
        self.uhd_usrp_sink.set_clock_source(options.clock_source)

        # L1C uses complex samples directly
        self.blocks_file_source = blocks.file_source(gr.sizeof_gr_complex*1, options.filename, True)

        # Scale complex values for L1C signal
        # L1C has different power levels for data and pilot components
        # Using a conservative scaling factor of 1.0/2^12 for headroom
        self.blocks_multiply_const = blocks.multiply_const_vcc((1.0/2**12, ))

        # establish the connections
        self.connect((self.blocks_file_source, 0), (self.blocks_multiply_const, 0))
        self.connect((self.blocks_multiply_const, 0), (self.uhd_usrp_sink, 0))

def get_options():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-x", "--gain", type="eng_float", default=0,
                      help="set transmitter gain [default=0]")
    parser.add_option("-f", "--frequency", type="eng_float", default=1575420000,
                      help="set transmit frequency [default=1575420000]")
    parser.add_option("-s", "--sample-rate", type="eng_float", default=5000000,
                      help="set sample rate [default=5000000]") 
    parser.add_option("-t", "--filename", type="string", default="gpssim_l1c.bin",
                      help="set output file name [default=gpssim_l1c.bin]")
    parser.add_option("-a", "--args", type="string", default="",
                      help="set UHD arguments [default='']")
    parser.add_option("-c", "--clock_source", type="string", default="internal",
                      help="set clock source [default='internal']")


    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        raise SystemExit(1)

    return (options)

if __name__ == '__main__':
    (options) = get_options()
    tb = top_block(options)
    tb.start()
    input('Press Enter to quit: ')
tb.stop()
tb.wait()
