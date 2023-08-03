Nameing of Variant level
cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_DISTINGUISHEDGENTLEMAN.html

The variant level contains object, library, or executable files specific to a particular variant of the processor.
We mostly use

a
	The image being built is an object library.
so
	The image being built is a shared object.
g
	Compile and link the source with the debugging flag set.
be, le
	Compile and link the source to generate big- (if be) or little- (if le) endian
	code.

Variant names can be placed in any order in the compound variant, For example:
g.le	A debugging version of a little-endian executable.
so.be	A big-endian version of a shared object.

