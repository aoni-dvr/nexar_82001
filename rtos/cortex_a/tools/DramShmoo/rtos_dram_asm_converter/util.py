import bitarray
import bitarray.util

from itertools import groupby
from operator import itemgetter

class Bitfield:
    def __init__(self, name, hi_lo_bit, mapping):
        self.name = name
        if (hi_lo_bit[0] < hi_lo_bit[1]):
            print("hi/lo bit sequence wrong! Bitfield init failed!")
            raise
        self.hi = hi_lo_bit[0]
        self.lo = hi_lo_bit[1]
        self.mapping = mapping
        self.val = None

    def value(self):
        return self.val

    def mapped_value(self):
        if self.val is not None:
            try:
                return self.mapping(self.val)
            except:
                return "UNDEFINED"

class Register:
    def __init__(self, name, raw, out, debug, *bitfield):
        self.name = name
        self.raw = raw
        self.bitfield = {}
        for bf in bitfield:
            self.bitfield.update({bf.name: bf})
        self.update_bitfield(out, debug)
        if debug:
            self.show_unused_bitfield()

    def value(self):
        return bitarray.util.ba2int(self.raw)

    def update_bitfield(self, out, debug):
        for bf in sorted(self.bitfield.values(), key=lambda x: x.lo):
            v = bitarray.util.ba2int(self.raw[bf.lo:bf.hi+1])    # This will get bit[hi:lo]
            if out:
                if bf.mapping:
                    out.update({bf.name: bf.mapping(v)})
                else:
                    out.update({bf.name: v})
            bf.val = v
            if debug:
                print("[Debug] Collect from %s - bit[%d:%d], value:0x%x" % (self.name, bf.hi, bf.lo, bf.val))

    def show_unused_bitfield(self):
        bits = list(range(32))
        for bf in sorted(self.bitfield.values(), key=lambda x: x.lo, reverse=True):
            for b in range(bf.lo, bf.hi+1):
                if b in bits:
                    bits.remove(b)

        print("[Warning] %s - " % self.name, end='')
        ranges = []
        for k, g in groupby(enumerate(bits), lambda x: x[0]-x[1]):
            group = map(itemgetter(1), g)
            group = list(map(int,group))
            ranges.append((group[0], group[-1]))
        bits_diff = []
        for lo, hi in ranges[::-1]:
            bits_diff.append("bit[%d:%d]" % (hi, lo))
        print("%s unused! (Info could be lost in translations...)" % "/".join(bits_diff))

