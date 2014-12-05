LD31
====

My Ludum Dare 31 entry

This time I'm doing it as much in scripts as possible, the script interface documentation is available here: [Script Interface](https://gist.github.com/ace13/ae6db4b5c6fbcf41b7fd#file-script-interface-md).
Or you can dump a local copy by starting the application with the argument '-d'

It will automatically load all scripts directly situated in the `Scripts/` folder, and will also autmatically hot-reload any scripts if they change.

Dependencies
============

This entry's built on top of the SFML git and Angelscript SVN, so you might want to grab recent versions of both.
It also uses CMake for build system generation, so that's a plus too.

And because of quite extensive use of C++11 I recommend a recent compiler, the CMake script should stop you from using one that won't be able to compile it anyway.
