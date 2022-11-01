/**
 * ARexxXml.h
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

#ifndef AREXXML_H
#define AREXXML_H

#define ALL_REACTION_CLASSES
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "XmlHandler.h"
#include "ARexxXmlException.h"


#define VERSTAG "\0$VER: ARexxXml 1.3 (11.9.2016) Joakim Nordstrom"

#define TEMPLATE          "NEWPORT/S"
#define OPT_MULTIPLEINSTANCES    0
#define OPT_COUNT   1

#ifdef DEBUG
#define AREXXXML_H_DEBUG printf
#else
#define AREXXXML_H_DEBUG(x, y...) {}
#endif

/* Protos for the reply hook and ARexx command functions. */
extern void reply_callback(struct Hook *, Object *, struct RexxMsg *);
extern void rexx_LoadXml (struct ARexxCmd *, struct RexxMsg *);
extern void rexx_GetTag (struct ARexxCmd *, struct RexxMsg *);
extern void rexx_Quit (struct ARexxCmd *, struct RexxMsg *);
extern void rexx_Dispose (struct ARexxCmd *, struct RexxMsg *);

class ARexxXml {
  public:

    /* ARexx command IDs. */
    enum
    {
        REXX_LOADXML,
        REXX_TAG,
        REXX_DISPOSE,
        REXX_QUIT,
        REXX_DATE
    };

    ARexxXml();
    ~ARexxXml();
    void StartMainLoop();
    static BOOL running;
    static XmlHandler * xmlHandler;
    void SetNoSlot(BOOL noSlot) { _noSlot = noSlot; };
  protected:
    Object *arexx_obj;
    BOOL _noSlot;
  private:

};

#endif

