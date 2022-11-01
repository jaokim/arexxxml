/**
 * ARexxXml.cpp
 * @date 2015-03-03
 * @author Joakim Nordstrom
 *
 * MIT License
 *
 * Copyright (c) 2016 Joakim Nordstrom
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ARexxXml.h"

STRPTR USED ver = (STRPTR)VERSTAG;

int main(int argc, char **argv) {
    LONG opts[OPT_COUNT];
    struct RDArgs *rdargs;

    rdargs = IDOS->ReadArgs(TEMPLATE, opts, NULL);
    if(rdargs == NULL)
    {
        IDOS->Printf("Couldn't read arguments.\n");
        exit(20) ;
    }



    ARexxXml *arexxXml;
    try {
        arexxXml = new ARexxXml();
        if(opts[OPT_MULTIPLEINSTANCES]) {
            arexxXml->SetNoSlot(FALSE);
            IDOS->Printf("Opening port AREXXXML.X\n");
        } else {
            arexxXml->SetNoSlot(TRUE);
            IDOS->Printf("Opening port AREXXXML\n");
        }

        arexxXml->StartMainLoop();
    } catch(...) {
      IDOS->Printf("Caught exception, and throwing it again.\n");
      delete arexxXml;
      throw;
    }
    delete arexxXml;
    IDOS->FreeArgs(rdargs);

    return 0;
}
