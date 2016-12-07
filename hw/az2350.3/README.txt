To see my tests, run `make test`

My program is implemented as follows:

If there is no configuration file specified on the command line (i.e. the program is just run as "./hwcopy"), the program will look for a file called "default_config.txt".

I assume that the input and output filenames passed to stdin do not contain any kinds of whitespace besides standard spaces (i.e. I don't account for tabs or carriage returns in my stdin parsing).

I assume that the configuration file is well-formed syntactically, so I don't do input checking on that.

I limit the number of aliases allowed in the configuration file to 100. I implement aliases by creating symlinks (relative to the document root), which are later used whenever the alias is used in an input filename.

Input file checking: I check the path "depth" to make sure the input filepath doesn't go outside of the document root. As long as the input filepath ends in a location that is inside the document root, it is valid - for example, if the filepath is something like `../docRoot/etc`, it works even though it goes outside the document root temporarily. However, if the input filepath depth is negative, that means it goes outside of the document root and is invalid. I make the assumption that the same thinga applies to aliases - the destination of the alias will act as the new document root, so the filepath still shouldn't go outside of that.

Output file checking: I start a child process and chroot into the output area directory to write the output file, which restricts the output file to that directory.

At the end, I unlink all the alias symlinks that were created during execution of the program.