# bitarray

This is a implementation of a bitarray using a fixed length array of one byte long words.
The operations treat the words as one contigious whole and get and set bits at a given index.

Big endian order is used. This means that we start from the most significant bit for the
operations. For example, for the bits `1000`, we start from `1`.

Little endian order is assumed to be on the actual computer. This might require some changes,
if used on a cpu using little endian order.
