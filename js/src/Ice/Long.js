// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var Ice = (function(my, undefined)
{

    /**
     * The Long type represents a signed 64-bit integer. The constructor accepts an optional initializer.
     *
     * @param l Acceptable values are undefined or null (equivalent to 0), Long, Number, and String.
     * Number values are truncated to the integer portion.
     * @throws TypeError Raised if the initializer is an invalid type.
     * @throws RangeError Raised if the initializer is out of range.
     **/
    my.Long = function(l)
    {
        this._low = 0;
        this._high = 0;

        //
        // Returns true if the value is equivalent to zero, or false otherwise.
        //
        Object.defineProperty(this, 'isZero', {
            get: function()
            {
                return this._high === 0 && this._low === 0;
            }
        });

        //
        // Returns -1 if the value is negative, 0 if the value is zero, and 1 if the value is positive.
        //
        Object.defineProperty(this, 'sign', {
            get: function()
            {
                if(this._low === 0 && this._high === 0)
                {
                    return 0;
                }

                if((this._high & my.Long.SIGN_MASK) != 0)
                {
                    return -1;
                }

                return 1;
            }
        });

        //
        // Returns the high 32 bits as an unsigned integer.
        //
        Object.defineProperty(this, 'highBits', {
            get: function()
            {
                return this._high;
            }
        });

        //
        // Returns the low 32 bits as an unsigned integer.
        //
        Object.defineProperty(this, 'lowBits', {
            get: function()
            {
                return this._low;
            }
        });

        //
        // A missing or null argument is equivalent to 0.
        //
        if(l === undefined || l === null || l == 0)
        {
            return;
        }

        if(l instanceof my.Long)
        {
            this._low = l._low;
            this._high = l._high;
            return;
        }

        var type = typeof(l);

        /*
        if(l is uint)
        {
            fromUint(l);
            return;
        }

        if(l is int)
        {
            fromInt(l as int);
            return;
        }
        */

        if(type === "number")
        {
            this.fromString(l.toFixed(0));
        }
        else if(type === "string")
        {
            this.fromString(l);
        }
        else
        {
            throw new TypeError("Invalid input argument of type " + type);
        }
    }

    //
    // Helper method used to create an Ice.Long from two words, bypassing all checks.
    //
    // This allows us to abuse the representation and treat it as a 64-bit unsigned integer.
    //
    my.Long.create = function(high, low)
    {
        var r = new my.Long();
        r._high = high;
        r._low = low;
        return r;
    }

    //
    // Maximum value is 2^63-1.
    //
    Object.defineProperty(my.Long, 'MAX_VALUE', {
        value: my.Long.create(0x7FFFFFFF, 0xFFFFFFFF)
    });

    //
    // Minimum value is -2^63.
    //
    Object.defineProperty(my.Long, 'MIN_VALUE', {
        value: my.Long.create(0x80000000, 0x00000000)
    });

    //
    // The zero value.
    //
    Object.defineProperty(my.Long, 'ZERO', {
        value: my.Long.create(0, 0)
    });

    //
    // The powers of ten from 10^0 to 10^18.
    //
    Object.defineProperty(my.Long, 'TEN_POWERS', {
        value: [
            my.Long.create(0, 1),                      // 10^0  = 1
            my.Long.create(0, 10),                     // 10^1  = 10
            my.Long.create(0, 100),                    // 10^2  = 100
            my.Long.create(0, 1000),                   // 10^3  = 1000
            my.Long.create(0, 10000),                  // 10^4  = 10000
            my.Long.create(0, 100000),                 // 10^5  = 100000
            my.Long.create(0, 1000000),                // 10^6  = 1000000
            my.Long.create(0, 10000000),               // 10^7  = 10000000
            my.Long.create(0, 100000000),              // 10^8  = 100000000
            my.Long.create(0, 1000000000),             // 10^9  = 1000000000
            my.Long.create(0x2, 0x540BE400),           // 10^10 = 0x2540BE400
            my.Long.create(0x17, 0x4876E800),          // 10^11 = 0x174876E800
            my.Long.create(0xE8, 0xD4A51000),          // 10^12 = 0xE8D4A51000
            my.Long.create(0x918, 0x4E72A000),         // 10^13 = 0x9184E72A000
            my.Long.create(0x5AF3, 0x107A4000),        // 10^14 = 0x5AF3107A4000
            my.Long.create(0x38D7E, 0xA4C68000),       // 10^15 = 0x38D7EA4C68000
            my.Long.create(0x2386F2, 0x6FC10000),      // 10^16 = 0x2386F26FC10000
            my.Long.create(0x1634578, 0x5D8A0000),     // 10^17 = 0x16345785D8A0000
            my.Long.create(0xDE0B6B3, 0xA7640000)      // 10^18 = 0xDE0B6B3A7640000
        ]
    });

    //
    // (_high & SIGN_MASK) != 0 denotes a negative number;
    // that is, the most significant bit is set.
    //
    Object.defineProperty(my.Long, 'SIGN_MASK', {
        value: 0x80000000
    });

    //
    // For 2^63, there are a maximum of 19 base 10 digits
    // 10^0 to 10^18.
    //
    Object.defineProperty(my.Long, 'MAX_POWER', {
        value: 18
    });

    my.Long.prototype.equals = function(other)
    {
        if(!(other instanceof my.Long))
        {
            return false;
        }

        return this._high === other._high && this._low === other._low;
    }

    /**
     * Returns the absolute value.
     **/
    my.Long.prototype.abs = function()
    {
        if(this.sign >= 0)
        {
            return this;
        }
        return this.negate();
    }

    /**
     * Negates the value.
     **/
    my.Long.prototype.negate = function()
    {
        if(this._high === 0 && this._low === 0)
        {
            return this;
        }

        var low;
        if(this.sign > 0)
        {
            low = ~(this._low - 1) & 0xFFFFFFFF;
        }
        else
        {
            if(this.equals(my.Long.MIN_VALUE))
            {
                throw new RangeError("Overflow"); // Out of range
            }
            low = (~(this._low) + 1) & 0xFFFFFFFF;
        }

        return my.Long.create((~this._high) & 0xFFFFFFFF, low);
    }

    /**
     * Add a value and return the sum.
     *
     * @param v Acceptable values are null (equivalent to 0), Long, int, uint, Number,
     * and String. Number values are truncated to the integer portion.
     * @throws ArgumentError Raised if the argument is an invalid type.
     * @throws RangeError Raised if the argument or result is out of range.
     **/
    my.Long.prototype.add = function(v)
    {
        //
        // This method handles addition. The method implementation handles the operation
        // sign and does the addition using internalAdd and internalSubtract than operates
        // always with positive numbers.
        //
        // Note that when converting to and from the twos-complement representation MIN_VALUE
        // doesn't need to be converted, because we abuse the representation to accept
        // -MIN_VALUE as valid.
        //
        var x1 = this;
        var x2 = v instanceof my.Long ? v : new my.Long(v);
        var r;

        var x1Sign = x1.sign;
        var x2Sign = x2.sign;

        //
        // If both operands are 0 just return 0, if one operand
        // is zero return the other.
        //
        if(x1Sign == 0 && x2Sign == 0)
        {
            return my.Long.ZERO;
        }
        else if(x1Sign == 0)
        {
            return x2;
        }
        else if(x2Sign == 0)
        {
            return x1;
        }

        //
        // Addition of two positive numbers.
        //
        if(x1Sign > 0 && x2Sign > 0)
        {
            r = this.internalAdd(x1, x2);
            if(r._high >= 0x80000000)
            {
                throw new RangeError("Overflow");
            }
            return r;
        }

        //
        // Addition of two negative numbers.
        //
        if(x1Sign < 0 && x2Sign < 0)
        {
            if(x1._high != 0x80000000)
            {
                x1 = my.Long.create((~x1._high) & 0xFFFFFFFF, (~x1._low + 1) & 0xFFFFFFFF);
            }

            if(x2._high != 0x80000000)
            {
                x2 = my.Long.create((~x2._high) & 0xFFFFFFFF, (~x2._low + 1) & 0xFFFFFFFF);
            }

            r = this.internalAdd(x1, x2);

            //
            // Check bounds of negative numbers.
            //
            if(r._high > 0x80000000 || (r._high == 0x80000000 && r._low > 0))
            {
                throw new RangeError("Overflow");
            }

            //
            // Convert r to negative.
            //
            if(r._high != 0x80000000)
            {
                r = my.Long.create((~r._high) & 0xFFFFFFFF, ~(r._low - 1) & 0xFFFFFFFF);
            }

            return r;
        }

        //
        // Addition of different sign is a subtraction.
        //

        //
        // Convert negative number to positive.
        //
        if(x1Sign < 0 && x1._high != 0x80000000)
        {
console.log("add: converting x1 to positive");
            x1 = my.Long.create((~x1._high) & 0xFFFFFFFF, (~x1._low + 1) & 0xFFFFFFFF);
        }

        //
        // Convert negative number to positive.
        //
        if(x2Sign < 0 && x2._high != 0x80000000)
        {
console.log("add: converting x2 to positive");
            x2 = my.Long.create((~x2._high) & 0xFFFFFFFF, (~x2._low + 1) & 0xFFFFFFFF);
        }

        var c = x1.unsignedCompareTo(x2);
console.log("add: c = " + c);
        if(c == 0)
        {
            return my.Long.ZERO;
        }

        //
        // x1 was positive so x2 was negative.
        //
        if(x1Sign >= 0)
        {
            //
            // x1 is greater than x2.
            //
            if(c >= 0)
            {
                r = this.internalSubtract(x1, x2);
                if(r._high >= 0x80000000)
                {
                    throw new RangeError("Overflow");
                }
            }
            else
            {
                r = this.internalSubtract(x2, x1);

                //
                // Check bounds of negative numbers.
                //
                if(r._high > 0x80000000 || (r._high == 0x80000000 && r._low > 0))
                {
                    throw new RangeError("Overflow");
                }

                //
                // x2 was greater and the negative number so we need to negate result.
                //
                if(r._high != 0x80000000)
                {
                    r = my.Long.create((~r._high) & 0xFFFFFFFF, (~r._low + 1) & 0xFFFFFFFF);
                }
            }
        }

        //
        // x2 was positive so x1 was negative.
        //
        if(x2Sign >= 0)
        {
            //
            // x1 is greater than x2.
            //
            if(c >= 0)
            {
                r = this.internalSubtract(x1, x2);

                //
                // Check bounds of negative numbers.
                //
                if(r._high > 0x80000000 || (r._high == 0x80000000 && r._low > 0))
                {
                    throw new RangeError("Overflow");
                }

                //
                // x1 was greater and the negative number so we need to negate result.
                //
                if(r._high != 0x80000000)
                {
                    r = my.Long.create((~r._high) & 0xFFFFFFFF, (~r._low + 1) & 0xFFFFFFFF);
                }
            }
            else
            {
                r = this.internalSubtract(x2, x1);
                if(r._high >= 0x80000000)
                {
                    throw new RangeError("Overflow");
                }
            }
        }

        return r;
    }

    //
    // During division, we left-align the divisor with the dividend.
    // The compareTo method considers the sign in comparisons and will
    // produce incorrect results when we use a left-aligned number
    // or an abused unsigned representation. For these cases, comparisons
    // must be done using this method instead of compareTo.
    //
    my.Long.prototype.unsignedCompareTo = function(other)
    {
        if(other === null)
        {
            return this.unsignedCompareTo(my.Long.ZERO);
        }

        if(this._high === other._high)
        {
            if(this._low === other._low)
            {
                return 0;
            }
            if(this._low > other._low)
            {
                return 1;
            }
            return -1;
        }

        if(this._high > other._high)
        {
            return 1;
        }

        return -1;
    }

    my.Long.prototype.fromInt = function(v)
    {
        if(v < 0)
        {
            this._high = 0xFFFFFFFF;
            this._low = ((~(-v)) + 1) & 0xFFFFFFFF;
        }
        else
        {
            this._high = 0;
            this._low = v & 0xFFFFFFFF;
        }
    }

    my.Long.prototype.fromString = function(s)
    {
        if(s === null)
        {
            this._low = 0;
            this._high = 0;
            return;
        }

        //
        // If the leading character is '-' set the sign.
        //
        var valueSign = false;
        if(s.charAt(0) === '-')
        {
            if(s.length == 1)
            {
                //
                // Input is just the sign "-"
                //
                throw new TypeError("Invalid input");
            }
            valueSign = true;
            s = s.substr(1);
        }

        //
        // Skip leading zeros.
        //
        while(s.charAt(0) === '0')
        {
            s = s.substr(1);
        }

        //
        // Empty String or null String is the same as 0.
        //
        if(s.length == 0)
        {
            if(valueSign)
            {
                //
                // Reject -0
                //
                throw new TypeError("Invalid input");
            }
            this._low = 0;
            this._high = 0;
            return;
        }

        //
        // Remaining characters should be digits.
        //
        var zero = "0".charCodeAt(0);
        var nine = "9".charCodeAt(0);
        for(var i = 0; i < s.length; i++)
        {
            var cc = s.charCodeAt(i);
            if(cc < zero || cc > nine)
            {
                throw TypeError("Invalid character `" + s.charAt(i) + "' at index " + i);
            }
        }

        //
        // 2^63 will be a maximun of 19 digits, assuming a decimal representation.
        //
        if(s.length > 19)
        {
            throw RangeError("Value is out of range");
        }

        //
        // Check if the value fits in an integer.
        //
        // Max integer is 2^31 - 1 and will accept a maximum
        // of 10 decimal digits.
        //
        if(s.length < 10)
        {
            this.fromInt(parseInt(s));
            if(valueSign)
            {
                this._low = (~_low + 1) & 0xFFFFFFFF;
                this._high = 0xFFFFFFFF;
            }
            return;
        }

        //
        // Calculate the number from the digits using addition
        // and multiplication of powers of 10.
        //
        var r = my.Long.ZERO;
        for(i = 0; i < s.length; i++)
        {
            var v = parseInt(s.charAt(s.length - i - 1));
            if(v != 0)
            {
                //
                // Note we use internalAdd to avoid overflow when parsing MIN_VALUE.
                //
                r = this.internalAdd(my.Long.create(0, v).multiply(my.Long.TEN_POWERS[i]), r);
            }
        }

        //
        // If parsing a negative number and it is min value, we are done
        // and just need to assign both words.
        //
        if(valueSign && r._high == my.Long.SIGN_MASK && r._low == 0)
        {
            _high = r._high;
            _low = r._low;
            return;
        }

        //
        // Check overflow and convert to twos-complement representation.
        //
        if(valueSign)
        {
            //
            // Check bounds of negative numbers.
            //
            if(r._high > 0x80000000 || (r._high == 0x80000000 && r._low > 0))
            {
                throw new RangeError("Overflow");
            }
            r._low = (~r._low - 1) & 0xFFFFFFFF;
            r._high = ~r._high;
        }
        else
        {
            //
            // Check bounds of positive numbers.
            //
            if(r._high >= 0x80000000)
            {
                throw new RangeError("Overflow");
            }
        }

        _low = r._low;
        _high = r._high;
    }

    //
    // Compute the sum of two 64-bit numbers and return the
    // result as a new 64-bit number. This method always operates
    // in positive numbers.
    //
    my.Long.prototype.internalAdd = function(x1, x2)
    {
        var low = (x1._low + x2._low) & 0xFFFFFFFF;
        var high = (x1._high + x2._high) & 0xFFFFFFFF;

        //
        // If high overflows, there is an overflow.
        //
        if(high < x1._high || high < x2._high)
        {
            throw new RangeError("Overflow");
        }

        if(high > 0x80000000)
        {
            throw new RangeError("Overflow");
        }

        if(low < x1._low || low < x2._low)
        {
            if(high == 0x80000000)
            {
                throw new RangeError("Overflow");
            }
            high++;
        }

        return my.Long.create(high, low);
    }

    //
    // Compute the subtraction of two 64-bit numbers and return the
    // result as a new 64-bit number. This method always operates
    // in positive numbers.
    //
    my.Long.prototype.internalSubtract = function(x1, x2)
    {
        var low = (x1._low - x2._low) & 0xFFFFFFFF;
        var high = (x1._high - x2._high) & 0xFFFFFFFF;

        if(low > x1._low)
        {
            high--;
        }
        if(high > 0x80000000)
        {
            throw new RangeError("Overflow");
        }

        return my.Long.create(high, low);
    }

    return my;
}(Ice || {}));

var l = new Ice.Long();
var l2 = Ice.Long.create(1, 2);
var min = Ice.Long.create(0x80000000, 0x00000000);
var max = Ice.Long.create(0x7FFFFFFF, 0xFFFFFFFF);
//console.log("min = max ? " + min.equals(max));
//console.log("min = MIN_VALUE ? " + min.equals(Ice.Long.MIN_VALUE));
//console.log("min = ZERO ? " + min.equals(Ice.Long.ZERO));
//console.log("min.isZero ? " + min.isZero);
//console.log("min.sign = " + min.sign);
console.log("min.highBits = " + min.highBits);
console.log("min.lowBits = " + min.lowBits);
console.log("max.highBits = " + max.highBits);
console.log("max.lowBits = " + max.lowBits);
//console.log("ZERO.isZero ? " + Ice.Long.ZERO.isZero);
//console.log("ZERO.sign = " + Ice.Long.ZERO.sign);
//console.log("min.sign = " + min.sign);
var one = new Ice.Long(1);
console.log("one.highBits = " + one.highBits);
console.log("one.lowBits = " + one.lowBits);
var two = new Ice.Long(2);
console.log("two.highBits = " + two.highBits);
console.log("two.lowBits = " + two.lowBits);
var three = one.add(two);
console.log("three.highBits = " + three.highBits);
console.log("three.lowBits = " + three.lowBits);
console.log("MIN_VALUE.sign = " + Ice.Long.MIN_VALUE.sign);
var n = Ice.Long.MIN_VALUE.add(one);
console.log("n.highBits = " + n.highBits);
console.log("n.lowBits = " + n.lowBits);
console.log("n.sign = " + n.sign);
