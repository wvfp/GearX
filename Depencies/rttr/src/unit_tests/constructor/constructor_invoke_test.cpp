/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014, 2015 - 2016 Axel Menzel <info@rttr.org>                     *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include <rttr/registration>
#include <catch/catch.hpp>

using namespace rttr;

struct ctor_invoke_test
{
    ctor_invoke_test(bool is_default_ctor = true)
    { 
        default_ctor_invoked = is_default_ctor; 
        func_as_ctor_invoked = is_default_ctor; 
    }
    ctor_invoke_test(const ctor_invoke_test& other) { cpy_ctor_invoked = true; }
    ctor_invoke_test(int param_1, double param_2)
    { 
        value_1 = param_1;
        value_2 = param_2;
        custom_ctor_invoked = true; 
    }
    ctor_invoke_test(ctor_invoke_test&& other) 
    :   value_1(std::move(other.value_1)), value_2(std::move(other.value_2)), _text(std::move(other._text))
    {}

    ctor_invoke_test(int p1, int p2, int p3, int p4, int p5, int p6, const int* p7){ value_1 = p1; value_2 = p2; }

    static ctor_invoke_test create_object() { return ctor_invoke_test(false); }

    bool default_ctor_invoked   = false;
    bool cpy_ctor_invoked       = false;
    bool custom_ctor_invoked    = false;
    bool func_as_ctor_invoked   = false;
    int value_1 = 0;
    double value_2 = 0;
    std::string _text;
};

RTTR_REGISTRATION
{
    registration::class_<ctor_invoke_test>("ctor_invoke_test")
        .constructor<>() (policy::ctor::as_raw_ptr)
        .constructor<const ctor_invoke_test&>() (policy::ctor::as_raw_ptr)
        .constructor<int, double>() (policy::ctor::as_raw_ptr)
        .constructor(&ctor_invoke_test::create_object)
        .constructor<int, int, int, int, int, int, const int* const>() (policy::ctor::as_raw_ptr)
        .constructor<>()
        (
            policy::ctor::as_object
        )
        .constructor<>()
        (
            policy::ctor::as_raw_ptr
        )
        .constructor<>()
        (
            policy::ctor::as_std_shared_ptr
        )
        // policy with arguments...
        .constructor<int, double>()
        (
            policy::ctor::as_object
        )
        .constructor<int, double>()
        (
            policy::ctor::as_raw_ptr
        )
        .constructor<int, double>()
        (
            policy::ctor::as_std_shared_ptr
        );
}

////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - invoke general", "[constructor]") 
{
    type t = type::get<ctor_invoke_test>();
    REQUIRE(t.is_valid() == true);
    
    SECTION("invoke default ctor")
    {
        constructor ctor = t.get_constructor();
        REQUIRE(ctor.is_valid() == true);

        variant var = ctor.invoke();
        CHECK(var.is_valid() == true);
        CHECK(var.get_type() == type::get<ctor_invoke_test*>());
        ctor_invoke_test* obj = var.get_value<ctor_invoke_test*>();
        CHECK(obj->default_ctor_invoked == true);
        t.get_destructor().invoke(var);
    }

    SECTION("invoke copy-ctor")
    {
        constructor ctor = t.get_constructor({type::get<ctor_invoke_test>()});
        REQUIRE(ctor.is_valid() == true);

        variant var = ctor.invoke(12);
        CHECK(var.is_valid() == false);

        ctor_invoke_test obj_default;
        var = ctor.invoke(obj_default);
        CHECK(var.get_type() == type::get<ctor_invoke_test*>());

        ctor_invoke_test* obj = var.get_value<ctor_invoke_test*>();
        CHECK(obj->cpy_ctor_invoked == true);
        t.get_destructor().invoke(var);
    }

    SECTION("invoke custom ctor")
    {
        constructor ctor = t.get_constructor({type::get<int>(), type::get<double>()});
        REQUIRE(ctor.is_valid() == true);

        variant var = ctor.invoke(true, 12);
        CHECK(var.is_valid() == false);

        var = ctor.invoke(23, 42.0);
        CHECK(var.is_valid() == true);
        ctor_invoke_test* obj = var.get_value<ctor_invoke_test*>();
        CHECK(obj->custom_ctor_invoked == true);
        CHECK(obj->value_1 == 23);
        CHECK(obj->value_2 == 42);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - invoke policy", "[constructor]") 
{
   auto ctor_list = type::get<ctor_invoke_test>().get_constructors();

    SECTION("as_object")
    {
        variant var = ctor_list[5].invoke();
        CHECK(var.is_valid() == true);
        CHECK(var.get_type() == type::get<ctor_invoke_test>());

        var = ctor_list[8].invoke(23, 42.0);
        CHECK(var.is_valid() == true);
        CHECK(var.get_type() == type::get<ctor_invoke_test>());
        const ctor_invoke_test& obj = var.get_value<ctor_invoke_test>();
        CHECK(obj.value_1 == 23);
        CHECK(obj.value_2 == 42);
    }

    SECTION("as_raw_ptr")
    {
        variant var = ctor_list[6].invoke();
        CHECK(var.is_valid() == true);
        CHECK(var.get_type() == type::get<ctor_invoke_test*>());

        type::get<ctor_invoke_test>().destroy(var);

        var = ctor_list[9].invoke(23, 42.0);
        CHECK(var.is_valid() == true);
        CHECK(var.get_type() == type::get<ctor_invoke_test*>());
        ctor_invoke_test* obj = var.get_value<ctor_invoke_test*>();
        CHECK(obj->value_1 == 23);
        CHECK(obj->value_2 == 42);

        type::get<ctor_invoke_test>().destroy(var);
    }

    SECTION("as_std_shared_ptr")
    {
        variant var = ctor_list[7].invoke();
        CHECK(var.is_valid() == true);
        CHECK(var.get_type() == type::get<std::shared_ptr<ctor_invoke_test>>());

        var = ctor_list[10].invoke(23, 42.0);
        CHECK(var.is_valid() == true);
        CHECK(var.get_type() == type::get<std::shared_ptr<ctor_invoke_test>>());
        const std::shared_ptr<ctor_invoke_test>& obj = var.get_value<std::shared_ptr<ctor_invoke_test>>();
        CHECK(obj->value_1 == 23);
        CHECK(obj->value_2 == 42);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("constructor - invoke variadic", "[constructor]") 
{
    type t = type::get<ctor_invoke_test>();

    constructor ctor = t.get_constructor({type::get<int>(), type::get<int>(), type::get<int>(), 
                                          type::get<int>(), type::get<int>(), type::get<int>(), 
                                          type::get<const int* const>()});
    REQUIRE(ctor.is_valid() == true);

    variant var = ctor.invoke_variadic({1, 2, 3, 4, 5, 6});
    CHECK(var.is_valid() == false);

    const int value = 23;
    var = ctor.invoke_variadic({1, 2, 3, 4, 5, 6, &value});
    CHECK(var.is_valid() == true);
    ctor_invoke_test* obj = var.get_value<ctor_invoke_test*>();
    CHECK(obj->value_1 == 1);
    CHECK(obj->value_2 == 2);
}

/////////////////////////////////////////////////////////////////////////////////////////
