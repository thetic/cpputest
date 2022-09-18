/*
 * Copyright (c) 2015, Michael Feathers, James Grenning, Bas Vodde
 * and Arnd R. Strube. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE EARLIER MENTIONED AUTHORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/IEEE754ExceptionsPlugin.h"

#if CPPUTEST_USE_STD_C_LIB
    #include <fenv.h>
#endif

#define IEEE754_CHECK_CLEAR(test, result, flag) ieee754Check(test, result, flag, #flag)

bool IEEE754ExceptionsPlugin::inexactDisabled_ = true;

IEEE754ExceptionsPlugin::IEEE754ExceptionsPlugin(const SimpleString& name) : TestPlugin(name) {}

void IEEE754ExceptionsPlugin::preTestAction(UtestShell&, TestResult&)
{
    #if FE_ALL_EXCEPT
    CHECK(!feclearexcept(FE_ALL_EXCEPT));
    #endif
}

void IEEE754ExceptionsPlugin::postTestAction(UtestShell& test, TestResult& result)
{
    if (!test.hasFailed()) {
        #if FE_DIVBYZERO
        IEEE754_CHECK_CLEAR(test, result, FE_DIVBYZERO);
        #endif
        #if FE_OVERFLOW
        IEEE754_CHECK_CLEAR(test, result, FE_OVERFLOW);
        #endif
        #if FE_UNDERFLOW
        IEEE754_CHECK_CLEAR(test, result, FE_UNDERFLOW);
        #endif
        #if FE_INVALID
        IEEE754_CHECK_CLEAR(test, result, FE_INVALID);
        #endif
        #if FE_INEXACT
        IEEE754_CHECK_CLEAR(test, result, FE_INEXACT);
        #endif
    }
}

void IEEE754ExceptionsPlugin::disableInexact()
{
    inexactDisabled_ = true;
}

void IEEE754ExceptionsPlugin::enableInexact()
{
    inexactDisabled_ = false;
}

bool IEEE754ExceptionsPlugin::checkIeee754OverflowExceptionFlag()
{
    #if FE_OVERFLOW
    return fetestexcept(FE_OVERFLOW) != 0;
    #endif
}

bool IEEE754ExceptionsPlugin::checkIeee754UnderflowExceptionFlag()
{
    #if FE_UNDERFLOW
    return fetestexcept(FE_UNDERFLOW) != 0;
#endif
}

bool IEEE754ExceptionsPlugin::checkIeee754InexactExceptionFlag()
{
    #if FE_INEXACT
    return fetestexcept(FE_INEXACT) != 0;
    #endif
}

bool IEEE754ExceptionsPlugin::checkIeee754DivByZeroExceptionFlag()
{
    #if FE_DIVBYZERO
    return fetestexcept(FE_DIVBYZERO) != 0;
    #endif
}

void IEEE754ExceptionsPlugin::ieee754Check(UtestShell& test, TestResult& result, int flag, const char* text)
{
    result.countCheck();
    #if FE_INEXACT
    if (inexactDisabled_)
        CHECK(!feclearexcept(FE_INEXACT));
    #endif
    #if FE_ALL_EXCEPT
    if ((flag & FE_ALL_EXCEPT) && fetestexcept(flag)) {
        CHECK(!feclearexcept(FE_ALL_EXCEPT));
        CheckFailure failure(&test, __FILE__, __LINE__, "IEEE754_CHECK_CLEAR", text);
        result.addFailure(failure);
    }
    #endif
}
