# Analysis Questions:

1. Both methods make the same calls in this case: one `opentat`, two `read`, and one `close`
2. `fopen()` is buffered so data can be written in a memory buffer before being written to disk, so only need to write once. `open()` meanwhile writes everytime data is received, which is slower.
3. The library method utilizes a buffer in order to use a smaller amount of calls, although here it does not matter as the file being read is smaller than the buffer.