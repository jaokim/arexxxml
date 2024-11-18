ARexxXml

ArexxXml can be download on [os4depot/utility/text/misc/arexxxml.lha](https://os4depot.net/?function=showfile&file=utility/text/misc/arexxxml.lha)

Version:        1.1
Author:         Joakim Nordström
Date:           2009-12-03
Requirements:   OS4, expat.library, uni.library, [http-handler]
Download:       https://os4depot.net/?function=showfile&file=utility/text/misc/arexxxml.lha


What is it?
-----------
ARexxXml is an ARexx interface for reading XML files.

How does it work?
-----------------
When started, ARexxXml creates an ARexx-port, AREXXXML.

Assume a XML-file like below:
```
    <test>
        <mojo temp="foo">Pop<mojo/>
        <mojo temp="bar">Funk<mojo/>
    </test>
```
Now, with one simple ARexx-command:
``` 
GETTAG "test.mojo(temp)"
```
we would get the value "`foo`" which is the attribute "`temp`"
of element "`mojo`".

Instructions:
-------------
```
ArexxXml takes one optional argument:
NEWPORT/S               Should the ARexx-port create a new
    ARexx-port, like AREXXXML.1, AREXXXML.2 etc, or should
    AREXXXML always (tried to) be opened. The default is
    to always use AREXXXML.


The ARexx-port support these commands:
LOADXML	FILE/F			
    Loads an XML-file into memory. Relative to ARexxXml home-drawer.
GETTAG TAG/F			
    Tries to retrieve a tag-structure from the XML-file.
    Each element is one string, child elements are preceded by
    a point. The content of <info> in test.xml would be returned
    with the command tag-string "test.info".
    Attributes are given in parentheses. the "temp" attribute of
    tag <mojo> would be "test.mojo(temp)"
    Multiple tags can be iterated with a []-notation and an index,
    starting at 0. The <mojo>-tag with content "FUNK" could be
    reached with "test.mojo[1]", and its temp-attribute with
    "test.mojo[1](temp)".
    The number of multiple tags can be returned with the special
    "length"-index. "test.mojo[length]" would in this case return 2.
    If an element can't be found, the result code 5 is returned.

CLOSE					
    Closes the XML file and removes it from memory.
	After a CLOSE, only the file and its resources are freed, the
    ARexx-port and program are still in memory.
QUIT					
    Quits ArexxXml and removes the ARexx-port.
	This quits and removes the entire ARexx-port and program.

Each command can return these codes:
    Code Error
    5  	Element not found
    10	You forgot to supply tag in GETTAG-command
    16 	The XMl isnt valid
    17  No file was loaded when trying to call "GETTAG"
    18 	File couldn't be found
    19 	File couldn't be read
    20 	Couldn't allocate memory
```

Examples:
---------

A few examples are provided in the ExampleScripts drawer:
GMail.rexx
    Gets the first 20 email in your gmail inbox, using the
    GMail ATOM feed.
    This script requires http.handler from os4depot.net
    (network/misc/httphandler.lha) by Chris Young.

ViewASX.rexx
    Reads asx-files and starta mplayer for viewing.
    Requires MPlayer.

GetYouTube.rexx
    Retrieves titles from YouTube.
    Also requires http-handler. (See GMail.rexx)

GetApps.rexx
    Lists all programs you have started.

Installation:
-------------
Copy ARexxXML to path.

Requirments:
------------
Make sure you have expat.library in Libs
(http://os4depot.net/share/development/library/misc/expat.lha).

For UTF-8 decoding, you must have uni.library installed
(http://os4depot.net/share/library/unilibdev.lha).

To handle URLs you need HTTP-handler (really not needed
by ARexxXML, but nice to use in scripts)
(http://os4depot.net/share/library/unilibdev.lha).

Release notes:
--------------
v 1.1 2009-12-03

* Added support for Unicode
* Fixed so files larger than 8k can be read (doh!)
* URL-support can be achieved by using HTTP-handler
  (see examples)
* Added NEWPORT argument.

v 1.0 2009-11-11

* Initial release

Todo:
-----

* Make it possible to traverse the XML-file tag by tag in ARexx

Bugs:
-----

Files have to be fully resolved, ie, DH1:Temp/test.xml, or relative
to the actual ArexxXml-binaries startpath.



