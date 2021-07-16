/*
 * Copyright (c) 2020 EKA2L1 Team.
 * 
 * This file is part of EKA2L1 project.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SCDV_SV_H_
#define SCDV_SV_H_

#include <e32std.h>

#define HLE_DISPATCH_FUNC(ret, name, id, ARGS...) \
    ret name(const TUint32 func_id, ##ARGS)

struct TFastBlitInfo {
    TUint8 *iDestBase;
    const TUint8 *iSrcBase;
    TPoint iDestPoint;
    TUint32 iDestStride;
    TUint32 iSourceStride;
    TSize iSrcSize;
    TRect iSrcRect;
};

extern "C" {
    HLE_DISPATCH_FUNC(void, UpdateScreen, 1, const TUint32 aScreenNumber, const TUint32 aNumberOfRect, const TRect *aRectangles);
    HLE_DISPATCH_FUNC(void, FastBlit, 2, const TFastBlitInfo *aInfo);
}

#endif
