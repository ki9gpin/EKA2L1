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

#ifndef SCDV_SCALE_H_
#define SCDV_SCALE_H_

#include <e32def.h>

namespace Scdv {
    class MScalingSettings {
    public:
        virtual TInt Set(TInt aFactorX, TInt aFactorY, TInt aDivisorX, TInt aDivisorY) = 0;
        virtual void Get(TInt &aFactorX, TInt &aFactorY, TInt &aDivisorX, TInt &aDivisorY) = 0;
        virtual TBool IsScalingOff() = 0;
    };
}

#endif
