# signalso
signalso library, a simple way to manage detached threads and deal with the most common signals

The folder structure comes from eclipse. Release has no dependencies, Debug depends on https://github.com/bombela/backward-cpp it links, dynamically, against it, the name I choose for it as a dynamic library is backwardSTso.

There are some fixed paths on the make files, "-L/usr/local/lib" and "-L/home/jouven/mylibs/debug" might need to be edited/removed depending on the circumstances.
