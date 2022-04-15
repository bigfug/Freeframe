# FFGL
This repo contains all the bigfug Freeframe and FFGL plugins, including the popular (and previously commerical) fugFeedbackGL and fugGlitchGL.

The FF10 ones are compatible with Resolume 2 (and 3?) and many old bits of software like VisualJockey Gold.

The FFGL ones are compatible with Resolume up to version 6, as far as I know, but not for version 7.

When I started writing Freeframe plugins back in the early 2000's, I was excited to support an open format that could be used in many different creative software packages.  As time went on, the amount of active software using Freeframe diminished and my interest in writing plugins waned as I was moving away from VJ'ing and into other areas.

These days, Freeframe is largely synonymous with the mighty Resolume, and they made a necessary decision to move from OpenGL 2.x to 4.x in Resolume 7.  This broke my plugins and since then I have received many requests to update them to work with it.

Unfortunately, I just don't have the time to do this, but I also don't want to kill the project completely as I know the plugins, especially fugFeedbackGL, are still popular.

So, in the spirit of the original Freeframe project, I have released the source code here under a GPL licence with hope that other developers will take up the challenge of updating them.  The GPL licence means that the code, and any modifications to it, must remain open source and cannot be incorporated into any closed source binaries.

As I hadn't imagined I would show the source code to anyone, I have to apologise profusely for the state of the code base, which almost certainly won't even compile at this point.  Also, if you're looking for helpful comments or documentation then you will be sadly disappointed.  However, I am very happy to answer questions and offer what assistance I can to anyone who will take on the task.

The code was designed to compile on Windows, macOS, and Linux and was my attempt to unify a Freeframe v1.0 and FFGL framework.  There are some bits that will require Qt5 to compile (such as fugScreenCapture and the plugin unit tests) and there are a couple of other dependencies for certain plugins such as TurboJPEG for fugGlitchGL.

I did some work moving from a (Qt) qmake build to CMake, but I think this wasn't done 100% so you'll see some old .pro files laying around, and probably some even older Visual Studio files for the FF10 ones.
